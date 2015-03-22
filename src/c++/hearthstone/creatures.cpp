#include "creatures.h"
#include "Cards.h"
#include "engine.h"
#include "Board.h"
#include "players.h"
#include "messages.h"

Engine* Instance::engine = nullptr;

PHero Instance::hero() { 
  return player->state.hero; // shortcut
}
PConstHero Instance::hero() const {
  return player->state.hero; // shortcut
}


PConstCardThing Thing::card_thing() const { 
  return issubclassP(card, const Card_Thing); 
}

Thing::Thing(int atq, int hp, int static_effects) :
  Instance() {
  memset(&state, 0, sizeof(state));
  state.hp = state.max_hp = hp;
  state.atq = state.max_atq = atq;
  state.static_effects = static_effects;
}

void Thing::add_static_effect(StaticEffect eff, bool inform) {
  inform &= (state.static_effects & eff)!=eff; // don't inform if no change
  state.static_effects |= eff;
  if (inform) UPDATE_THING_STATE("static_effects");
}

void Thing::remove_static_effect(StaticEffect eff, bool inform) {
  inform &= (state.static_effects | eff)!=0; // don't inform if no change
  state.static_effects &= ~eff;
  if (inform) UPDATE_THING_STATE("static_effects");
}

void Thing::popup() { // executed when created
  add_static_effect(StaticEffect::fresh, false);
  state.n_max_atq = is_windfury() ? 2 : 1;
}

void Thing::start_turn(Player* current) {
  if (player == current) {
    remove_static_effect(StaticEffect::fresh, false); // we were here before
    state.n_atq = 0;  // didn't attack yet this turn
    // for heroes
    state.n_remaining_power = 1;
    remove_static_effect(StaticEffect::insensible);
  }
}

void Thing::end_turn(Player* current) {
  if (state.tmp_atq) {
    state.atq -= state.tmp_atq;
    state.tmp_atq = 0;
    UPDATE_THING_STATE("atq");
  }
  if (state.tmp_hp) {
    state.hp -= state.tmp_hp;
    state.tmp_hp = 0;
    UPDATE_THING_STATE("hp");
  }
  if (player == current) {
    if (is_frozen() && state.n_atq == 0)
      remove_static_effect(StaticEffect::frozen);
  }
}

int Thing::hurt(int damage, Thing* caster) {
  assert(damage > 0);
  if (is_insensible())  return 0;
  int absorbed = min(damage, state.armor);
  if (absorbed)
    state.armor -= absorbed;
  damage -= absorbed;
  damage -= max(0, damage - state.hp);
  state.hp -= damage;
  //if( state.enraged_trigger ) state.enraged_trigger.trigger()
  if (damage && caster && caster->is_freezer())
    add_static_effect(StaticEffect::frozen);
  UPDATE_THING("hp armor", Msg_Damage, caster, this, damage+absorbed);
  check_dead();
  return damage + absorbed;
}

int Thing::heal(int hp, Thing* caster) {
  assert(hp>0);
  hp -= max(0, state.hp + hp - state.max_hp);
  state.hp += hp;
  //if( state.enraged_trigger ) state.enraged_trigger.trigger()
  UPDATE_THING("hp", Msg_Heal, caster, this, hp);
  return hp;
}

void Thing::change_hp(int hp) {
  state.max_hp += hp;
  assert(state.max_hp >= 1);
  state.hp += max(0, hp);  // only add if positive
  state.hp = min(state.hp, state.max_hp);
  //if state.enraged_trigger: state.enraged_trigger.trigger()
  UPDATE_THING_STATE("hp max_hp");
  check_dead();
}

void Thing::change_atq(int atq) {
  state.atq += atq;
  state.max_atq += atq;
  UPDATE_THING_STATE("atq max_atq");
}

void Thing::silence() {
  NI;
  //state.action_filters = {};
  //state.modifiers = []
  //state.triggers = []
  //state.enraged_trigger = None
  //while state.effects :
  //e = state.effects.pop()
  //  if type(e) != str :
  //    e.undo()
  //    elif e == "windfury" :
  //    state.n_max_atq = 1  // undo windfury
  //    state.effects = ['silence']
  //    if not state.dead :
  //      state.engine.send_UPDATE_STATUS(Msg_UPDATE_STATUS('hp max_hp atq max_atq effects'))
}

void Thing::check_dead() {
  if (state.hp <= 0) {
    add_static_effect(StaticEffect::dead);
    player->state.n_dead++;
  }
}

void Thing::kill_me() {
  silence();
  PThing me = findP(engine->board.state.everybody, this);
  engine->board.remove_thing(me);
}

void Creature::attack(Thing* target) {
  state.n_atq += 1;
  assert(state.n_atq <= state.n_max_atq);
  remove_static_effect(StaticEffect::stealth);
  target->hurt(state.atq, this);
  int target_atq = target->state.atq;
  if (target_atq)
    hurt(target_atq, target);
}

Minion::Minion(const Minion& copy, Player* player) :
Minion(copy) {
  init(issubclassP(copy.card, const Card_Minion), player);
}

PConstCardMinion Minion::card_minion() const {
  return issubclassP(card, const Card_Minion);
}

string Minion::tostr() const {
  return string_format("%s (%X): %d/%d", card_minion()->name_fr.c_str(), this, state.atq, state.hp);
}

void Minion::popup() { // executed when created
  Thing::popup();
  engine->signal(this, Event::MinionPopup);
}

void Minion::list_actions(ListAction& actions) const {
  if (state.n_atq < state.n_max_atq && 
      state.atq > 0 && !is_frozen() &&
      (!is_fresh() || is_charge()) )
    actions.push_back( &act_attack );
}


//PInstance Minion::copy() const {
//  return NEWP(Minion, *this);
//}


float Weapon::score_situation() {
  if (is_dead()) return 0;
  int atq = is_frozen() ? 0 : state.atq;
  atq *= 1 + is_windfury();
  return float(state.hp*0.6 + atq*0.57);
}