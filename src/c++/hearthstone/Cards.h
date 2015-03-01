#ifndef __CARDS_H__
#define __CARDS_H__
#include "common.h"

struct Card {
  SET_ENGINE();

  Player* player;
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

  Card(int cost, const string& name) :
    player(nullptr), cost(cost), id(-1), name(name), name_fr(name),
    collectible(true), cls(HeroClass::None) {}
  
  NAMED_PARAM(Card, HeroClass, cls);
  NAMED_PARAM(Card, bool, collectible);
  NAMED_PARAM(Card, string, desc);
  NAMED_PARAM(Card, string, name_fr);
  NAMED_PARAM(Card, string, desc_fr);

  virtual PCard copy() const = 0; // copy itself

  // helper function to resovle a target at runtime
  ListCreature list_targets(const string& Target);

  // what the player can do with this card ?
  virtual void list_actions(ListAction& list) const = 0;

  virtual string tostr() const = 0;
};

struct Card_Instance : public Card {
  PInstance instance;

  Card_Instance(int cost, string name, PInstance instance) :
    Card(cost, name), instance(instance) {}
};

struct Card_Minion : public Card_Instance {
  Card_Minion(int cost, string name, PMinion minion) :
    Card_Instance(cost, name, dynamic_pointer_cast<Instance>(minion)) {}

  virtual string tostr() const;

  virtual PCard copy() const {
    return NEWP(Card_Minion, *this);
  }

  // what the player can do with this card ?
  virtual void list_actions(ListAction& list) const;
};

// see heroes.h for Card_Hero and Card_HeroAbility

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




### ----------------- Spell cards -------------------------


struct Card_Spell(Card) :
  def __init__(cost, name, actions, Target = 'none', **kwargs) :
  Card.__init__(cost, name, **kwargs)
  actions = actions # lambda self : [Msg_* list]
  assert type(Target) == str, pdb.set_trace()
  Target = "targetable " + Target # see list_targets()
  virtual string tostr() const
  return "%s (%d): %s" % (name_fr, cost, desc)
  def list_actions() :
  return Act_PlaySpellCard(list_targets(Target), actions)


struct Card_Coin(Card_Spell) :
  def __init__(owner) :
  Card_Spell.__init__(0, "The coin", lambda self : [Msg_GainMana(caster, 1)],
  desc = "Gain one mana crystal this turn only")
  owner = owner

  '''
struct Card_Wrath(Card_Spell) :
  """ Druid : Wrath (2 choices) """
  def __init__() :
  Card_Spell.__init__(2, "Wrath", cls = "Druid", name_fr = "Colere")
  def list_actions() :
  Target = engine.board.get_characters()
  first = Act_SingleSpellDamageCard(Target, damage = 3)
  actions = lambda self : [Msg_SpellDamage(caster, choices[0], damage),
  Msg_DrawCard(owner)]
  second = Act_SingleSpellDamageCard(Target, damage = 1, actions = actions)
  return[first, second]
  '''

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