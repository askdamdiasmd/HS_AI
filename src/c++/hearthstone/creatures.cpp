#include "creatures.h"
#include "Cards.h"
#include "engine.h"
#include "Board.h"
#include "players.h"
#include "messages.h"
#include "effects.h"

Engine* Instance::engine = nullptr;

PHero Instance::hero() { 
  return player->state.hero; // shortcut
}
PConstHero Instance::hero() const {
  return player->state.hero; // shortcut
}

// Thing -------------------------------------------------------------


Thing::Thing(int atq, int hp, int static_effects) :
  Instance() {
  const int static_part_size = ((char*)(&state.effects)) - ((char*)(&state));
  memset(&state, 0, static_part_size);
  state.hp = state.max_hp = hp;
  state.atq = state.max_atq = atq;
  state.n_remaining_power = 1;
  state.static_effects = static_effects;
  add_static_effect(StaticEffect::fresh, false);
}

Thing::Thing(const Thing& copy) :
  Instance(copy) {
  // fast copy of static params
  const int static_part_size = ((char*)(&state.effects)) - ((char*)(&state));
  memcpy(&state, &copy.state, static_part_size);

  // deep-copy the special effects
  for (auto& eff : copy.state.effects)
    eff->bind_copy_to(this);
  for (auto& eff : copy.state.presence_effects)
    eff->bind_copy_to(this);
  for (auto& eff : copy.state.death_rattles)
    eff->bind_copy_to(this);
}

PConstCardThing Thing::card_thing() const { 
  return issubclassP(card, const Card_Thing); 
}

void Thing::add_static_effect(StaticEffect eff, bool inform) {
  inform &= (state.static_effects & eff)!=eff; // don't inform if no change
  state.static_effects |= eff;
  if (engine && inform) {
    if (eff & (StaticEffect::charge | StaticEffect::windfury))
      state.n_max_atq = is_windfury() ? 2 : 1;
    UPDATE_THING_STATE("add_static_effect");
  }
}

void Thing::remove_static_effect(StaticEffect eff, bool inform) {
  inform &= (state.static_effects | eff)!=0; // don't inform if no change
  state.static_effects &= ~eff;
  if (inform) UPDATE_THING_STATE("remove_static_effect");
}

void Thing::popup() { // executed when it reaches the board
  state.n_max_atq = is_windfury() ? 2 : 1;
  if (state.spell_power)
    player->state.spell_power += state.spell_power;
  
  // trigger popup effects
  for (auto& eff : state.presence_effects)
    eff->trigger(Event::MinionPopup, this);
}

void Thing::start_turn(Player* current) {
  if (player == current) {
    remove_static_effect(StaticEffect::fresh, false); // we were here before
    state.n_atq = 0;  // didn't attack yet this turn
  }
}

void Thing::end_turn(Player* current) {
  // remove temporary buffs
  if (state.tmp_atq) 
    change_atq(-state.tmp_atq, 't');

  if (player == current) {
    if (is_frozen() && state.n_atq == 0)
      remove_static_effect(StaticEffect::frozen);
  }
}

int Thing::hurt(int damage, Instance* caster) {
  assert(damage > 0);
  if (is_insensible())  
    return 0;
  if (is_divine_shield()) {
    remove_static_effect(StaticEffect::divine_shield);
    UPDATE_THING("hurt divine_shield", Msg_Damage, GETP(caster), GETPTHING(this), 0);
    return 0;
  }

  const int absorbed = min(damage, state.armor);
  if (absorbed)
    state.armor -= absorbed;
  damage -= absorbed;
  damage -= max(0, damage - state.hp);
  state.hp -= damage;
  //if( state.enraged_trigger ) state.enraged_trigger.trigger()
  Thing* caster_thing = issubclass(caster, Thing);
  if (damage && caster_thing && caster_thing->is_freezer())
    add_static_effect(StaticEffect::frozen);
  UPDATE_THING("hurt hp armor", Msg_Damage, GETP(caster), GETPTHING(this), damage + absorbed);
  check_dead();
  return damage + absorbed;
}

int Thing::heal(int hp, Instance* caster) {
  assert(hp>0);
  hp -= max(0, state.hp + hp - state.max_hp);
  state.hp += hp;
  //if( state.enraged_trigger ) state.enraged_trigger.trigger()
  UPDATE_THING("heal hp", Msg_Heal, GETP(caster), GETPTHING(this), hp);
  return hp;
}

void Thing::change_hp(int hp, char type) {
  state.max_hp += hp;
  switch (type) {
  case 'n': break;  // normal
  case 'a': state.aura_hp += hp; assert(state.aura_hp >= 0); break;
  default:assert(false);
  }
  assert(state.max_hp >= 0);
  state.hp += max(0, hp);  // only add if positive
  state.hp = min(state.hp, state.max_hp);
  //if state.enraged_trigger: state.enraged_trigger.trigger()
  UPDATE_THING_STATE("change_hp");
  check_dead();
}

void Thing::change_atq(int atq, char type) {
  state.max_atq += atq;
  switch (type) {
  case 'n': break;  // normal
  case 't': state.tmp_atq += atq; assert(state.tmp_atq >= 0);  break;
  case 'a': state.aura_atq += atq; assert(state.aura_atq >= 0);  break;
  default:assert(false);
  }
  state.atq += atq;
  UPDATE_THING_STATE("change_atq");
}

void Thing::silence(bool die) {
  // reset static stuffs
  player->state.spell_power -= state.spell_power;

  if (!die) {
    state.atq -= state.tmp_atq;
    state.tmp_atq = 0;
    state.n_max_atq = 1;
    state.static_effects &= (StaticEffect::dead | StaticEffect::fresh);
    state.spell_power = 0;
  }

  // reset dynamic stuffs
  #define remove_effects(list, die) \
    while (!list.empty()) { \
      list.back()->undo(die); \
      list.pop_back(); }
  remove_effects(state.effects, die);
  remove_effects(state.presence_effects, true);
  if (!die) remove_effects(state.death_rattles, die);
  
  if (!die) UPDATE_THING_STATE("silence");
}

void Thing::check_dead() {
  if (state.hp <= 0) destroy(false);
}

void Thing::destroy(bool clean_now) {
  add_static_effect(StaticEffect::dead);
  engine->board.state.n_dead++;
  
  // cancel popup effects
  remove_effects(state.presence_effects, true);

  if (clean_now) 
    engine->board.clean_deads();
}

void Thing::signal_death() {
  if (!state.death_rattles.empty()) {
    for (auto& dr : state.death_rattles) 
      dr->trigger(Event::ThingDead, this);
  }
}

// Creature -------------------------------------------------------------

bool Creature::attack(Creature* target) {
  if (is_dead() || target->is_dead())
    return false; // on annule tout !

  state.n_atq += 1;
  assert(state.n_atq <= state.n_max_atq);
  remove_static_effect(StaticEffect::stealth);

  // signal attack
  engine->board.signal(this, Event::Attack);
  SEND_DISPLAY_MSG(Msg_Attack, GETP(this), GETP(target));

  target->hurt(state.atq, this);
  int target_atq = target->state.atq;
  if (target_atq)
    hurt(target_atq, target);
  
  return true;
}

// Minion -------------------------------------------------------------

Minion::Minion(const Minion& copy, Player* player) :
  Minion(copy) {
  init(issubclassP(copy.card, const Card_Minion), player);
}

PConstCardMinion Minion::card_minion() const {
  return issubclassP(card, const Card_Minion);
}

string Minion::tostr() const {
  return string_format("%s (%X): %d:%d", card_minion()->name_fr.c_str(), this, state.atq, state.hp);
}

void Minion::list_actions(ListAction& actions) const {
  if (can_attack())
    actions.push_back( &act_attack );
}

void Minion::popup() { // executed when created
  Thing::popup();
  engine->board.signal(this, Event::MinionPopup);
}

void Minion::signal_death() {
  engine->board.signal(this, Event::MinionDead);
  Thing::signal_death();
}

// Weapon -------------------------------------------------------------

Weapon::Weapon(const Weapon& copy, Player* controller) :
  Thing(copy) {
  init(issubclassP(copy.card, const Card_Weapon), controller);
}

PConstCardWeapon Weapon::card_weapon() const {
  return issubclassP(card, const Card_Weapon);
}

string Weapon::tostr() const {
  return string_format("%s (%X): %d:%d", card_weapon()->name_fr.c_str(), this, state.atq, state.hp);
}

void Weapon::popup() {
  Thing::popup();
  player->state.hero->equip_weapon();
  engine->board.signal(this, Event::WeaponPopup);
}

void Weapon::signal_death() {
  player->state.hero->unequip_weapon();
  engine->board.signal(this, Event::WeaponDead);
  Thing::signal_death();
}

float Weapon::score_situation() {
  if (is_dead()) return 0;
  int atq = is_frozen() ? 0 : state.atq;
  atq *= 1 + is_windfury();
  return float(state.hp*0.6 + atq*0.57);
}