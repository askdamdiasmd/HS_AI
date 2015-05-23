#include "common.h"
#include "engine.h"
#include "actions.h"
#include "Cards.h"
#include "creatures.h"
#include "players.h"
#include "effects.h"
#include "messages.h"


Engine* Effect::engine = nullptr;

void Effect::bind_to(PEffect me, Instance* owner) {
  assert(this == me.get() && !this->owner);
  this->owner = owner;
}


// death rattles ----------

void Eff_DeathRattle::bind_to(PEffect me, Instance* owner) {
  Effect::bind_to(me, owner);
  CAST(owner, Thing)->add_static_effect(Thing::StaticEffect::death_rattle, false);
  CAST(owner, Thing)->state.death_rattles.push_back(CASTP(me, Eff_DeathRattle));
}

bool Eff_DeathRattle::trigger(Event ev, Instance* caster) {
  assert((ev&Event::ThingDead) && caster == owner && CAST(caster, Thing)->is_dead());
  return action(this, ev, caster);
}

Eff_DR_Invoke_Minion::Eff_DR_Invoke_Minion(PConstCardMinion popup)
  :Eff_DeathRattle(FUNCEFFECT {
  Minion* dead = CAST(eff->owner, Minion);
  Slot pos = eff->engine->board.get_minion_pos(eff->owner);
  PMinion born = CAST(eff, const Eff_DR_Invoke_Minion)->card->instanciate(pos.player);
  return eff->engine->board.add_thing(caster, born, pos);
  }), card(popup) {}

Eff_DR_Damage::Eff_DR_Damage(Target target, int damage)
  :Eff_DeathRattle(FUNCEFFECT{
  const Eff_DR_Damage* me = CAST(eff, const Eff_DR_Damage);
  ListPInstance targets = me->target.resolve(caster->player,caster);
  assert(len(targets) <= 1);  // use zone damage/heal if more targets
  for (auto& i : targets)
    if (me->damage > 0) {
      SEND_DISPLAY_MSG(Msg_Arrow, GETP((Thing*)caster), CASTP(i,Thing), '*', "RED_on_BLACK");
      me->engine->board.damage(caster, me->damage, i.get());
    }
    else {
      SEND_DISPLAY_MSG(Msg_Arrow, GETP((Thing*)caster), CASTP(i, Thing), '+', "GREEN_on_BLACK");
      me->engine->board.heal(caster, -me->damage, i.get());
    }
  return true;
  }), target(target), damage(damage){}

Eff_DR_ZoneDamage::Eff_DR_ZoneDamage(Target target, int damage)
  :Eff_DeathRattle(FUNCEFFECT{
    const Eff_DR_ZoneDamage* me = CAST(eff, const Eff_DR_ZoneDamage);
    if (me->damage>0)
      engine->board.damage_zone(caster, me->damage, me->target);
    else
      engine->board.heal_zone(caster, me->damage, me->target);
    return true;
  }), target(target), damage(damage){}


// auras ----------

Eff_Aura::Eff_Aura(int atq, int hp) :
  Effect(FUNCEFFECT{ return !CAST(eff->owner,Thing)->is_dead() && caster->player == eff->owner->player; }),
  atq(atq), hp(hp) {}

void Eff_Aura::bind_to(PEffect me, Instance* owner) {
  Effect::bind_to(me, owner);
  CAST(owner, Thing)->state.effects.push_back(me);
  CAST(owner, Thing)->add_static_effect(Thing::StaticEffect::aura, false);
  if (engine) 
    engine->board.register_trigger(this, get_triggers());
}

void Eff_Aura::undo(bool die) {
  engine->board.unregister_trigger(this, get_triggers());
}

PMinion Eff_Aura_Ngh::get_neighbor(const char way) {
  assert(abs(way) == 1);
  Minion* owner = CAST(this->owner, Minion);
  Player* player = owner->player;
  Slot pos = engine->board.get_minion_pos(owner);
  const int i = pos.pos + way;
  if (0 <= i && i < len(player->state.minions))
    return player->state.minions[i];
  else
    return PMinion();
}

void Eff_Aura_Ngh::update_ngh(PMinion ngh, const char way) {
  assert(abs(way) == 1);
  #define update(who,what,c)  \
    if (who && !who->is_dead())  \
      who->change_##what(c*what, 'a')
  if (hp)   update(ngh, hp, way);
  if (atq)  update(ngh, atq, way);
  #undef update
}
bool Eff_Aura_Ngh::trigger(Event ev, Instance* caster) {
  PMinion left = get_neighbor(-1);
  if (left_m != left) {
    update_ngh(left_m, -1);
    left_m = left;
    update_ngh(left_m, 1);
  }
  PMinion right = get_neighbor(1);
  if (right_m != right) {
    update_ngh(right_m, -1);
    right_m = right;
    update_ngh(right_m, 1);
  }
  return true;
}
void Eff_Aura_Ngh::undo(bool die) {
  update_ngh(left_m,-1);
  update_ngh(right_m,-1);
  Eff_Aura::undo(die);
}


// presence effects ---------

void Eff_Presence::bind_to(PEffect me, Instance* owner) {
  Effect::bind_to(me, owner);
  CAST(owner, Thing)->state.presence_effects.push_back(CASTP(me,Eff_Presence));
}