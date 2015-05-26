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

void Eff_DeathRattle::trigger(Signal s) const {
  assert((ev&Event::ThingDead) && caster == owner && CAST(caster, Thing)->is_dead());
  action(this, ev, caster);
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
      SEND_DISPLAY_MSG(Msg_Arrow, GETPT(caster,Thing), CASTP(i, Thing), '*', "RED_on_BLACK");
      me->engine->board.damage(caster, me->damage, i.get());
    }
    else {
      SEND_DISPLAY_MSG(Msg_Arrow, GETPT(caster,Thing), CASTP(i, Thing), '+', "GREEN_on_BLACK");
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

Eff_Aura::Eff_Aura(int atq, int hp, int st_eff) :
  Effect(FUNCEFFECT{ return caster->player == eff->owner->player && !CAST(eff->owner, Thing)->is_dead(); }),
  atq(atq), hp(hp), st_eff(st_eff) {}

void Eff_Aura::bind_to(PEffect me, Instance* owner) {
  Effect::bind_to(me, owner);
  CAST(owner, Thing)->state.eff_auras.push_back(me);
  CAST(owner, Thing)->add_static_effect(Thing::StaticEffect::aura, false);
  if (engine) 
    engine->board.register_trigger(this, get_triggers());
}

void Eff_Aura::undo(bool die) const {
  engine->board.unregister_trigger(this, get_triggers());
}

void Eff_Aura::update_minion(Minion* m, const char way) const {
  assert(abs(way) == 1);
  if (m && !m->is_dead()) {
    if (hp)   m->change_hp(way*hp, 'a');
    if (atq)  m->change_atq(way*atq, 'a');
    if (st_eff) {
      if (way > 0)
        m->add_static_effect(Thing::StaticEffect(st_eff), 'a');
      else
        m->remove_static_effect(Thing::StaticEffect(st_eff), 'a');
    }
  }
  #undef update
}

void Eff_Aura_Friends::trigger(Signal s) const {
  // we know that owner is not dead and that caster has same player than owner
  // because of is_triggered(.)
  if (caster == owner) {
    // aura-maker just appeared
    // simulate addition/removal of other minions
    assert(ev == Event::AddMinion);
    for (auto& pm : owner->player->state.minions) {
      Minion* m = pm.get();
      if (m != owner)  trigger(ev, m, nullptr, 0);
    }
  }
  else {
    Minion* m = CAST(caster, Minion);
    if ((m->breed & breed) == breed) {
      if (ev == Event::AddMinion) {
        CONSTCAST(this, Eff_Aura_Friends)->minions.push_back(m);
        update_minion(m, 1);
      }
      else {
        remove(CONSTCAST(this, Eff_Aura_Friends)->minions, m);
        update_minion(m, -1);
      }
    }
  }
}

void Eff_Aura_Friends::undo(bool die) const {
  // remember aura-ed minions even after death
  for (auto& m : minions)
    update_minion(m, -1);
  Eff_Aura::undo(die);
}

Minion* Eff_Aura_Ngh::get_neighbor(const char way) const {
  assert(abs(way) == 1);
  Minion* owner = CAST(this->owner, Minion);
  Slot pos = engine->board.get_minion_pos(owner);
  ListPMinion& minions = owner->player->state.minions;
  assert(minions[pos.pos].get() == owner);
  const int i = pos.pos + way;
  return 0 <= i && i < len(minions) ? minions[i].get() :  nullptr;
}

void Eff_Aura_Ngh::trigger(Signal s) const {
  // we know that owner is not dead and that caster has same player than owner
  // because of is_triggered(...)
  // so the call is valid, and we don't care whether it's an addition or 
  // a removal, since we just update the neighbors in any case.
  Minion* left = get_neighbor(-1);
  if (left_m != left) {
    update_minion(left_m, -1);
    CONSTCAST(this, Eff_Aura_Ngh)->left_m = left;
    update_minion(left_m, 1);
  }
  Minion* right = get_neighbor(1);
  if (right_m != right) {
    update_minion(right_m, -1);
    CONSTCAST(this, Eff_Aura_Ngh)->right_m = right;
    update_minion(right_m, 1);
  }
}
void Eff_Aura_Ngh::undo(bool die) const {
  update_minion(left_m, -1);  // we know that they still exist in memory somewhere
  update_minion(right_m, -1);
  Eff_Aura::undo(die);
}


// presence effects ---------

void Eff_Presence::bind_to(PEffect me, Instance* owner) {
  Effect::bind_to(me, owner);
  CAST(owner, Thing)->state.presence_effects.push_back(CASTP(me,Eff_Presence));
}


// trigger effect ----------

void Eff_Trigger::bind_to(PEffect me, Instance* owner) {
  Effect::bind_to(me, owner);
  CAST(owner, Thing)->add_static_effect(Thing::StaticEffect::trigger, false);
  CAST(owner, Thing)->state.effects.push_back(me);
  if (engine) engine->board.register_trigger(this, triggers);
}

void Eff_Trigger::undo(bool die) const {
  engine->board.unregister_trigger(this, triggers);
}

Eff_Knife::Eff_Knife(Event triggers, int damage, Target targets, int nb, FuncEffect is_triggered) :
  Eff_Trigger(triggers, is_triggered,
  FUNCEFFECT{
    const Eff_Knife* me = CAST(eff, const Eff_Knife);
    for (int n = 0; n < me->nb; n++) {
      ListPInstance to = me->targets.resolve(me->owner->player, me->owner);
      if (to.empty()) break;
      Engine* engine = eff->engine;
      me->engine->board.damage(me->owner, me->damage, to[0].get());
      SEND_DISPLAY_MSG(Msg_Arrow, GETPT(me->owner, Thing), CASTP(to[0], Thing), '|', "WHITE_on_BLACK");
    }
    return true;
  }), damage(damage), nb(nb), targets(targets) {}
