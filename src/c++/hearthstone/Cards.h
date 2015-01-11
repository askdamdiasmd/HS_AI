#include "common.h"
#include "engine.h"
#include "players.h"

struct Card {
  Player* owner;
  int cost;
  string name, name_fr;
  enum HeroClass {
    None = 0,
    Druid,
    Priest,
  } cls;
  string desc, desc_fr;
  //ListEffects effects;
  bool collectible;
  
  Card(int cost, const string& name ) :
    owner(nullptr), cost(cost), name(name), collectible(true), cls(Classes::None) {}
  
#define NAMED_PARAM(type, param)  Card& set_##param(type v) { param = v; return *this; }
  NAMED_PARAM(HeroClass, cls);
  NAMED_PARAM(bool, collectible);
  NAMED_PARAM(string, desc);
  NAMED_PARAM(string, desc);
  NAMED_PARAM(string, name_fr);
  NAMED_PARAM(string, desc_fr);
#undef NAMED_PARAM

  //  effects = tolist(effects) # list of effects : {'taunt', 'stealth', or buffs that can be silenced}
  //score = 0 # estimated real mana cost, dependent on a specific deck it's in
  //  if desc == '':
  //#assert all([type(e) == str for e in effects]), "error: description is missing"
  //desc = '. '.join(['%s%s' % (e[0].upper(), e[1:]) for e in effects if type(e) == str])
  //  if desc_fr == '':
  //fr = [eff_trad_fr[e] for e in effects if type(e) == str]
  //  fr = '. '.join([e for e in fr if e])
  //  if len(desc) < len(fr) + 2 : desc_fr = fr # only if description is basic

  static Engine* e;
  static void set_engine( Engine* en) {
    engine = e;
  }

  virtual ListAction list_actions() const = 0;

  ListCreatures list_targets(self, const string& targets) {
    return engine->board.list_targets(owner, targets);
  }
};

typedef shared_ptr<Card> PCard;

class Card_Minion : public Card {
  int hp, atq;
  enum Category {
    None, 
    Beast,

  } cat;
  struct StaticEffect {
    bool taunt;
    bool divine_shield;
  } eff;
  ListEffect effects;
  
  Card_Minion(int cost, int atq, int hp, string name, cat = Category::None )
    Card.__init__(self, cost, name, **kwargs)
    hp = hp    # health point = life
    atq = atq  # attack
    cat = cat  # category of minion = 'beast', ...

  string tostr() {
    return string_format("%s (%d): %d/%d %s", name_fr, cost, atq, hp, desc);
  }

  virtual ListAction list_actions() const {
    return Act_PlayMinionCard(self);
  }
};
//Properties props; // taunt and so on


/*

### --------------- Minion cards ----------------------


class Card_Minion_BC(Card_Minion) :
  """ Minion with a battle cry """
  def __init__(self, cost, atq, hp, name, battlecry, targets = None, hidden_target = None, **kwargs) :
  Card_Minion.__init__(self, cost, atq, hp, name, **kwargs)
  battlecry = battlecry
  targets = targets
  hidden_target = hidden_target
  def list_actions(self) :
  targets = list_targets(targets) if targets else None
  hidden_target = hidden_target #list_targets(hidden_target) if hidden_target else None
  return Act_PlayMinionCard_BC(self, battlecry, targets, hidden_target)


### --------------- Weapon cards ----------------------

class Card_Weapon(Card) :
  def __init__(self, cost, atq, hp, name, **kwargs) :
  Card.__init__(self, cost, name, **kwargs)
  hp = hp    # health point = weapon durability
  atq = atq  # attack

  def __str__(self) :
  return "Weapon %s (%d): %d/%d %s" % (name_fr, cost, atq, hp, desc)

  def list_actions(self) :
  return Act_PlayWeaponCard(self)




### ----------------- Spell cards -------------------------


class Card_Spell(Card) :
  def __init__(self, cost, name, actions, targets = 'none', **kwargs) :
  Card.__init__(self, cost, name, **kwargs)
  actions = actions # lambda self : [Msg_* list]
  assert type(targets) == str, pdb.set_trace()
  targets = "targetable " + targets # see list_targets()
  def __str__(self) :
  return "%s (%d): %s" % (name_fr, cost, desc)
  def list_actions(self) :
  return Act_PlaySpellCard(self, list_targets(targets), actions)


class Card_Coin(Card_Spell) :
  def __init__(self, owner) :
  Card_Spell.__init__(self, 0, "The coin", lambda self : [Msg_GainMana(caster, 1)],
  desc = "Gain one mana crystal this turn only")
  owner = owner

  '''
class Card_Wrath(Card_Spell) :
  """ Druid : Wrath (2 choices) """
  def __init__(self) :
  Card_Spell.__init__(self, 2, "Wrath", cls = "Druid", name_fr = "Colere")
  def list_actions(self) :
  targets = engine.board.get_characters()
  first = Act_SingleSpellDamageCard(self, targets, damage = 3)
  actions = lambda self : [Msg_SpellDamage(caster, choices[0], damage),
  Msg_DrawCard(owner)]
  second = Act_SingleSpellDamageCard(self, targets, damage = 1, actions = actions)
  return[first, second]
  '''

class Card_DamageSpell(Card_Spell) :
  def __init__(self, cost, damage, name, targets = "characters", name_fr = "", desc = "", cls = None) :
  Card_Spell.__init__(self, cost, name, None, targets, name_fr = name_fr, desc = desc, cls = cls)
  damage = damage
  def list_actions(self) :
  return Act_SingleSpellDamageCard(self, list_targets(targets), damage)


class Card_FakeDamageSpell(Card_DamageSpell) :
  def __init__(self, damage, targets = "characters") :
  Card_DamageSpell.__init__(self, damage - 1, damage, "Fake Damage Spell %d"%damage, targets,
  name_fr = "Faux Sort de dommage %d"%damage,
  desc = "Deal %d points of damage"%damage)

  */

