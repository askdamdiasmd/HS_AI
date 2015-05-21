#ifndef __EFFECTS_H__
#define __EFFECTS_H__
#include "common.h"


struct Effect {
  SET_ENGINE();
  const FuncEffect is_triggered;
  Instance * owner;

  Effect(FuncEffect is_triggered = FUNCEFFECT{ return true; }) :
    is_triggered(is_triggered),
    owner(nullptr) {}

  virtual void bind_to(PEffect me, Instance* owner);
  virtual void bind_copy_to(Instance* owner) const = 0;

  virtual bool trigger(Event ev, Instance* caster) = 0;
  virtual void undo(bool die) {}
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
  virtual bool trigger(Event ev, Instance* caster);
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
  Event get_triggers() const { return Event(Event::AddMinion | Event::RemoveMinion); }

  Eff_Aura(int atq, int hp);

  virtual void bind_to(PEffect me, Instance* owner);
  virtual void undo(bool die);
};

struct Eff_Aura_Ngh : public Eff_Aura {
  PMinion left_m, right_m;

  Eff_Aura_Ngh(int atq, int hp) :
    Eff_Aura(atq, hp) {}

  PMinion get_neighbor(const char way);
  void update_ngh(PMinion ngh, const char way);

  DECLARE_COPY_BIND_TO(Eff_Aura_Ngh)
  virtual bool trigger(Event ev, Instance* caster);
  virtual void undo(bool die);
};


// presence effects ---------

struct Eff_Presence : public Effect {
  FuncEffect popup, depop;

  Eff_Presence(FuncEffect popup, FuncEffect depop) :
    Effect(), popup(popup), depop(depop) {}

  DECLARE_COPY_BIND_TO(Eff_Presence)
  virtual void bind_to(PEffect me, Instance* owner);
  virtual bool trigger(Event ev, Instance* caster) {
    assert(ev & Event::ThingPopup);
    assert(owner == caster);
    return popup(this, ev, caster);
  }
  virtual void undo(bool die) {
    depop(this, Event::RemoveThing, owner);
  }
};



#undef DECLARE_COPY_BIND_TO
#endif