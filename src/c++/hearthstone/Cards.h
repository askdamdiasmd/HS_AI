#ifndef __CARDS_H__
#define __CARDS_H__
#include "common.h"
#include "actions.h"

struct VizCard;
typedef shared_ptr<VizCard> PVizCard;

struct Card {
  SET_ENGINE();

  int cost, id;
  string name, name_fr;
  enum HeroClass {
    None = 0,
    Chaman,
    Hunter,
    Druid,
    Mage,
    Paladin,
    Priest,
    Rogue,
    Warrior,
    Warlock,
  } cls;
  string desc, desc_fr;
  bool collectible;
  PVizCard viz;

  Card(int cost, const string& name) :
    cost(cost), id(-1), name(name), name_fr(name),
    collectible(true), cls(HeroClass::None) {}
  
  NAMED_PARAM(Card, HeroClass, cls);
  NAMED_PARAM(Card, bool, collectible);
  NAMED_PARAM(Card, string, desc);
  NAMED_PARAM(Card, string, name_fr);
  NAMED_PARAM(Card, string, desc_fr);

  virtual PCard copy() const = 0; // copy itself

  string get_name_fr() const { return name_fr.size() ? name_fr : name; }
  string get_desc_fr() const { return desc_fr.size() ? desc_fr : desc; }

  virtual string tostr() const = 0;

  // helper function to resovle a target at runtime
  ListPCreature list_targets(const string& Target);

  // what the player can do with this card ?
  virtual void list_actions(ListAction& list) const = 0;
};

struct Card_Instance : public Card {
  const PConstInstance instance;  // exemplar is copied when card is actionned

  Card_Instance(int cost, string name, PInstance instance) :
    Card(cost, name), instance(instance) {}
};


struct Card_Thing : public Card_Instance {
  const PConstThing thing() const { return issubclassP(instance, const Thing); }

  Card_Thing(int cost, string name, PThing minion) :
    Card_Instance(cost, name, dynamic_pointer_cast<Instance>(minion)) {}
};

struct Card_Minion : public Card_Thing {
  const Act_PlayMinionCard act_play;
  const PConstMinion minion() const { return issubclassP(instance, const Minion); }

  Card_Minion(int cost, string name, PMinion minion) :
    Card_Thing(cost, name, dynamic_pointer_cast<Thing>(minion)), act_play(this) {}

  Card_Minion(const Card_Minion& m) :
    Card_Thing(m), act_play(this) {}

  virtual string tostr() const;

  virtual PCard copy() const { return NEWP(Card_Minion, *this); }

  // what the player can do with this card ?
  virtual void list_actions(ListAction& list) const;
};

struct Card_HeroAbility : public Card {
  const Act_HeroPower action;

  Card_HeroAbility(int cost, string name, FuncAction actions, Target target);

  virtual string tostr() const {
    return string_format("Card Hero Ability: %s %s", name.c_str(), desc.c_str());
  }

  virtual PCard copy() const {
    return NEWP(Card_HeroAbility, *this);
  }

  virtual void list_actions(ListAction& list) const;
};

struct Card_Hero : public Card_Thing {
  PCardHeroAbility ability;

  PConstHero hero() const { return issubclassP(instance, const Hero); }

  Card_Hero(string name, HeroClass cls, PHero hero, PCardHeroAbility ability);

  virtual string tostr() const;

  virtual PCard copy() const {
    return NEWP(Card_Hero, *this);
  }

  virtual void list_actions(ListAction& list) const;
};

/*
//  effects = tolist(effects) # list of effects : {'taunt', 'stealth', or buffs that can be silenced}
//score = 0 # estimated real mana cost, dependent on a specific deck it's in
//  if desc == '':
//#assert all([type(e) == str for e in effects]), "error: description is missing"
//desc = '. '.join(['%s%s' % (e[0].upper(), e[1:]) for e in effects if type(e) == str])
//  if desc_fr == '':
//fr = [eff_trad_fr[e] for e in effects if type(e) == str]
//  fr = '. '.join([e for e in fr if e])
//  if len(desc) < len(fr) + 2 : desc_fr = fr # only if description is basic


### --------------- Minion cards ----------------------


struct Card_Minion_BC(Card_Minion) :
  """ Minion with a battle cry """
  def __init__(cost, atq, hp, name, battlecry, Target = None, hidden_target = None, **kwargs) :
  Card_Minion.__init__(cost, atq, hp, name, **kwargs)
  battlecry = battlecry
  Target = Target
  hidden_target = hidden_target
  def list_actions() :
  Target = list_targets(Target) if Target else None
  hidden_target = hidden_target #list_targets(hidden_target) if hidden_target else None
  return Act_PlayMinionCard_BC(battlecry, Target, hidden_target)


### --------------- Weapon cards ----------------------

struct Card_Weapon(Card) :
  def __init__(cost, atq, hp, name, **kwargs) :
  Card.__init__(cost, name, **kwargs)
  hp = hp    # health point = weapon durability
  atq = atq  # attack

  virtual string tostr() const
  return "Weapon %s (%d): %d/%d %s" % (name_fr, cost, atq, hp, desc)

  def list_actions() :
  return Act_PlayWeaponCard()
*/



// ### ----------------- Spell cards -------------------------


struct Card_Spell : public Card {

  Card_Spell(int cost, string name) :
    Card(cost, name) {}
  
  virtual string tostr() const {
    return string_format("[SpellCard] %s (%d): %s", get_name_fr().c_str(), cost, get_desc_fr().c_str());
  }
};

struct Card_TargetedSpell : public Card_Spell {
  const Act_TargetedSpellCard action;

  Card_TargetedSpell(int cost, string name, FuncAction actions, Target targets = 0) :
    Card_Spell(cost, name), action(this, actions, targets) {}

  virtual void list_actions(ListAction& list) const {
    list.push_back(&action);
  }
};

struct Card_AreaSpell : public Card_Spell {
  const Act_AreaSpellCard action;

  Card_AreaSpell(int cost, string name, FuncAction actions, Target targets = 0) :
    Card_Spell(cost, name), action(this, actions, targets) {}

  virtual void list_actions(ListAction& list) const {
    list.push_back(&action);
  }
};

struct Card_Coin : public Card_AreaSpell {
  Card_Coin();

  virtual PCard copy() const;
};

/*
struct Card_DamageSpell(Card_Spell) :
  def __init__(cost, damage, name, Target = "characters", name_fr = "", desc = "", cls = None) :
  Card_Spell.__init__(cost, name, None, Target, name_fr = name_fr, desc = desc, cls = cls)
  damage = damage
  def list_actions() :
  return Act_SingleSpellDamageCard(list_targets(Target), damage)


struct Card_FakeDamageSpell(Card_DamageSpell) :
  def __init__(damage, Target = "characters") :
  Card_DamageSpell.__init__(damage - 1, damage, "Fake Damage Spell %d"%damage, Target,
  name_fr = "Faux Sort de dommage %d"%damage,
  desc = "Deal %d points of damage"%damage)

  */

#endif