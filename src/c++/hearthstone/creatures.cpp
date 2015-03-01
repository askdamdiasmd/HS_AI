#include "creatures.h"
#include "Cards.h"
#include "engine.h"
#include "Board.h"
#include "players.h"
#include "messages.h"

Engine* Instance::engine = nullptr;

PHero Instance::hero() const { 
  assert(0); return PHero();
  //return controller->hero; 
}

Thing::Thing() :
  Instance() {
  memset(&state, 0, sizeof(state));
  assert(0);
  //state.atq = state.max_atq = card->atq;
  //state.hp = state.max_hp = card->hp;
  //add_static_effect(StaticEffect::fresh);
  state.n_max_atq = 1; // number of times we can attack per turn

  //state.add_effects(deepcopy(card.effects), inform = False)
}

void Thing::hurt(int damage, Thing* caster) {
  assert(damage > 0);
  if (is_insensible())  return;
  int absorbed = min(damage, state.armor);
  if (absorbed) {
    state.armor -= absorbed;
    UPDATE_STATUS("armor");
  }
  damage -= absorbed;
  state.hp -= damage;
  //if( state.enraged_trigger ) state.enraged_trigger.trigger()
  UPDATE_STATUS("hp");
  if (caster && caster->is_freezer())
    add_static_effect(StaticEffect::frozen);
  check_dead();
}

void Thing::heal(int hp) {
  assert(hp>0);
  state.hp = min(state.max_hp, state.hp + hp);
  //if( state.enraged_trigger ) state.enraged_trigger.trigger()
  UPDATE_STATUS("hp");
}

void Thing::change_hp(int hp) {
  state.max_hp += hp;
  assert(state.max_hp >= 1);
  state.hp += max(0, hp);  // only add if positive
  state.hp = min(state.hp, state.max_hp);
  //if state.enraged_trigger: state.enraged_trigger.trigger()
  UPDATE_STATUS("hp max_hp");
  check_dead();
}

void Thing::change_atq(int atq) {
  state.atq += atq;
  state.max_atq += atq;
  UPDATE_STATUS("atq max_atq");
}

void Thing::silence() {
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
  assert(0);
}

void Thing::check_dead() {
  if (state.hp <= 0)
    add_static_effect(StaticEffect::dead);
  assert(0);//engine->board.signal_dead(this);
}

void Thing::kill_me() {
  silence();
  engine->board.remove_thing(PInstance(dynamic_cast<Instance*>(this)));
}

void Creature::attack(Creature* target) {
  state.n_atq += 1;
  assert(state.n_atq <= state.n_max_atq);
  if (is_stealth()) {
    remove_static_effect(StaticEffect::stealth);
    UPDATE_STATUS("static_effects");
  }
  target->hurt(state.atq, this);
  if (target->state.atq) hurt(target->state.atq, target);
}

ListAction Minion::list_actions() {
  //if (state.n_atq >= state.n_max_atq ||
  //  state.atq <= 0 || is_frozen() ||
  //  (is_fresh() && !is_charge()) )
  //  return {};
  //else 
  //  return { Act_MinionAttack(this, engine->board.get_attackable_characters(controller)) };
  assert(0); return{};
}

Minion::Minion(PCardMinion card) :
  Minion(*dynamic_pointer_cast<Minion>(card->instance)) {
  init(card, card->player);
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