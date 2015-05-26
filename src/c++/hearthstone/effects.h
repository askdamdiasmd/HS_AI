#ifndef __EFFECTS_H__
#define __EFFECTS_H__
#include "common.h"


/* Effects CAN'T HAVE INTERNAL VARIABLES 
   (everything should be const)
*/

struct Effect {
  SET_ENGINE();
  const FuncEffect is_triggered;
  Instance * owner;

  Effect(FuncEffect is_triggered = FUNCEFFECT{ return true; }) :
    is_triggered(is_triggered),
    owner(nullptr) {}

  virtual PEffect copy() const { return PEffect(); }  // default = useless
  // only usefull for auras effect, as they contain internal variables

  virtual void bind_to(PEffect me, Instance* owner);
  virtual void bind_copy_to(Instance* owner) const = 0;

  virtual void trigger(Signal s) const = 0;
  virtual void undo(bool die) const {}
};

#define DECLARE_COPY_BIND_TO(cls) \
  virtual void bind_copy_to(Instance* owner) const { \
    PEffect res = NEWP(cls, *this); \
    res->owner = nullptr; \
    res->bind_to(res, owner); \
  }



// death rattles ----------

struct Eff_DeathRattle : public Effect {
  const FuncEffect action;

  Eff_DeathRattle(FuncEffect action) :
    Effect(), action(action) {}

  virtual void bind_to(PEffect me, Instance* owner);
  virtual void trigger(Signal s) const;
};

struct Eff_DR_Invoke_Minion : public Eff_DeathRattle {
  PConstCardMinion card;
  Eff_DR_Invoke_Minion(PConstCardMinion popup);
  DECLARE_COPY_BIND_TO(Eff_DR_Invoke_Minion)
};

struct Eff_DR_Damage : public Eff_DeathRattle {
  const Target target;
  const int damage;
  Eff_DR_Damage(Target target, int damage);
  DECLARE_COPY_BIND_TO(Eff_DR_Damage)
};
struct Eff_DR_Heal : public Eff_DR_Damage {
  Eff_DR_Heal(Target target, int heal) :
    Eff_DR_Damage(target, -heal) {}
  DECLARE_COPY_BIND_TO(Eff_DR_Heal)
};

struct Eff_DR_ZoneDamage : public Eff_DeathRattle {
  const Target target;
  const int damage;
  Eff_DR_ZoneDamage(Target target, int damage);
  DECLARE_COPY_BIND_TO(Eff_DR_ZoneDamage)
};
struct Eff_DR_ZoneHeal : public Eff_DR_ZoneDamage {
  Eff_DR_ZoneHeal(Target target, int heal) :
    Eff_DR_ZoneDamage(target, -heal) {}
  DECLARE_COPY_BIND_TO(Eff_DR_ZoneHeal)
};

// auras ----------

struct Eff_Aura : public Effect {
  const int atq, hp;
  const int st_eff;
  Event get_triggers() const { return Event(Event::MinionPopup | Event::RemoveMinion); }

  Eff_Aura(int atq, int hp, int st_eff=0);

  void update_minion(Minion* m, const char way) const;

  virtual void bind_to(PEffect me, Instance* owner);
  virtual void undo(bool die) const;
};

struct Eff_Aura_Friends : public Eff_Aura {
  const Creature::Breed breed;
  ListMinion minions;

  Eff_Aura_Friends(int atq, int hp, Creature::Breed breed=Creature::Breed::None) :
    Eff_Aura(atq, hp), breed(breed) {}

  void update_friend(PMinion m, const char way) const;

  DECLARE_COPY_BIND_TO(Eff_Aura_Friends)
  virtual void trigger(Signal s) const;
  virtual void undo(bool die) const;
};

struct Eff_Aura_Ngh : public Eff_Aura {
  Minion *left_m, *right_m;
  Eff_Aura_Ngh(int atq, int hp) :
    Eff_Aura(atq, hp), left_m(nullptr), right_m(nullptr) {}

  Minion* Eff_Aura_Ngh::get_neighbor(const char way) const;
  void update_ngh(PMinion ngh, const char way);

  DECLARE_COPY_BIND_TO(Eff_Aura_Ngh)
  virtual void trigger(Signal s) const;
  virtual void undo(bool die) const;
};


// presence effects ---------

struct Eff_Presence : public Effect {
  FuncEffect popup, depop;

  Eff_Presence(FuncEffect popup, FuncEffect depop) :
    Effect(), popup(popup), depop(depop) {}

  DECLARE_COPY_BIND_TO(Eff_Presence)
  virtual void bind_to(PEffect me, Instance* owner);
  virtual void trigger(Signal s) const {
    assert(ev & Event::ThingPopup);
    assert(owner == caster);
    popup(this, ev, caster);
  }
  virtual void undo(bool die) const {
    depop(this, Event::RemoveThing, owner);
  }
};


// trigger effect ----------

struct Eff_Trigger : public Effect {
  const Event triggers;
  const FuncEffect action;

  Eff_Trigger(Event triggers, FuncEffect is_triggered, FuncEffect action) :
    Effect(is_triggered), triggers(triggers), action(action) {}

  virtual void bind_to(PEffect me, Instance* owner);
  DECLARE_COPY_BIND_TO(Eff_Trigger)
  virtual void trigger(Signal s) const { action(this, ev, caster); }
  virtual void undo(bool die) const;
};

struct Eff_Knife : public Eff_Trigger {
  const int damage, nb;
  const Target targets;

  Eff_Knife(Event triggers, int damage = 1, 
    Target targets = Target::random | Target::enemy | Target::characters, int nb = 1, 
    FuncEffect is_triggered = FUNCEFFECT{ 
      return caster!=eff->owner && 
      caster->player == eff->owner->player && 
      !CAST(eff->owner, Thing)->is_dead(); });

  DECLARE_COPY_BIND_TO(Eff_Knife)
};


#undef DECLARE_COPY_BIND_TO
#endif