#ifndef __ACTIONS_H__
#define __ACTIONS_H__
#include "common.h"
#include "messages.h"
#include "creatures.h"
#include "Cards.h"
#include "Board.h"


// ---------- Actions ---------------

struct Action {
  SET_ENGINE();
  Instance* const caster;
  const int cost;

  Action(Thing* caster, int cost = 0) : 
    caster(caster), cost(cost), num_choices(0) {}
  
  virtual bool is_valid() {
    return true;
  }

  virtual bool neighbors() {
    return false;  // return True if self = minion with neighbor aura / battlecry
  }

  int get_cost() const {
    return cost;
  }

  //int cardinality() {
  //  int res = 1;
  //  for (auto ch : choices)
  //    res *= ch.size();
  //  return res;
  //}
  
  //Action* select(int ch_num, int instance_num) {
  //  assert(ch_num < num_choices);
  //  selection[ch_num] = choices[ch_num][instance_num];
  //}

  //bool has_chosen() const {
  //  bool all = true;
  //  for (int i = 0; i < num_choices; i++)
  //     all &= selection[i]!=nullptr;  // non null
  //  return all;
  //}

  //int randomness() {
  //  # number of possible outcomes
  //  return 1   # default = 1 = no randomness
  //}

  virtual string tostr() const = 0;

  virtual void execute() = 0;
};

typedef shared_ptr<Action> PAction;
typedef vector<PAction> ListAction;



/// Basic game actions

struct Act_EndTurn : public Action {
  virtual string tostr() const {
    return "End turn";
  }
  
  virtual void execute() {
    engine->board.end_turn(caster);
  }
};


/// Play card

struct Act_PlayCard : public Action { 
  PCard card;
  Act_PlayCard(PCard card) :
    Action(card->player, card->cost), card(card) {}

  virtual string tostr() const {
    return string_format("Play Card %s", self.card);
  }

  virtual void execute() {
    engine->PlayCard(caster, card, get_cost());
  }
};



// -------- Minions -----------------

struct Act_PlayMinionCard : public Act_PlayCard {
  Board::ListSlot slots;
  Board::Slot sel_slot;

  Act_PlayMinionCard(PCard card) :
    Act_PlayCard(card), slots(engine->board.get_free_slots(card->player)) {}

  virtual bool is_valid() const {
    return !slots.empty();  // no more free slots
  }

  void select_slot(Board::Slot slot) {
    sel_slot = slot;
  }

  //virtual bool neighbors() {
  //  from effects import Eff_BuffNeighbors
  //    return any([type(e) == Eff_BuffNeighbors for e in self.card.effects])
  //}

  virtual void execute() {
    engine->board.add_thing(caster, Minion(card), sel_slot);
  }
};


/*struct Act_PlayMinionCard_BC(Act_PlayMinionCard) :
  ''' hero plays a minion card with battlecry '''
  def __init__(card, battlecry, chosable_targets = None, hidden_target = None) :
  Act_PlayMinionCard.___init___(card)
  self.choices = [self.engine.board.get_free_slots(card.owner)]
  self.hidden_target = None
  if chosable_targets : self.choices += [chosable_targets]
    elif hidden_target : self.hidden_target = hidden_target
    self.battlecry = battlecry
    def neighbors() :
    return self.hidden_target == 'neighbors' or Act_PlayMinionCard.neighbors()
    def execute() :
    Act_PlayCard.execute()
    pos = self.choices[0]
    assert type(pos).__name__ == 'Slot', pdb.set_trace()
    from creatures import Minion
    minion = Minion(self.card)
    actions = [Msg_AddMinion(self.caster, minion, pos)]
    if self.hidden_target == 'neighbors':
for target in self.card.owner.minions[max(0, pos.index - 1):pos.index + 1] :
actions.append(Msg_BindEffect(minion, target, self.battlecry()))
elif len(self.choices)>1 or self.hidden_target:
target = self.hidden_target or self.choices[1]
actions.append(Msg_BindEffect(minion, target, self.battlecry()))
self.engine.send_message(ac*/tions)


struct Act_Attack(Action) :
  ''' when something attacks something'''
  def __init__(caster, targets) :
  Action.__init__(caster)
  self.choices = [targets]
  virtual string tostr() const
  if self.has_chosen() :
    return "Attack with %s on %s" % (self.caster, self.choices[0])
  else :
  return "Attack with %s" % self.caster
  def execute() :
  target = self.choices[0]
  assert type(target) != list
  self.caster.attacks(target)

struct Act_MinionAttack(Act_Attack) :
  ''' when one minion attacks something'''
  pass

struct Act_HeroAttack(Act_Attack) :
  ''' when the hero (druid) attacks something'''
  pass

struct Act_WeaponAttack(Act_Attack) :
  ''' when a hero attacks something'''
  pass


// ------------- Weapon cards ------------------------

//struct Act_PlayWeaponCard(Act_PlayCard) :
//  ''' hero plays a weapon card '''
//  def __init__(card) :
//  Act_PlayCard.___init___(card)
//  self.choices = []
//  def execute() :
//  Act_PlayCard.execute()
//  from creatures import Weapon
//  self.engine.send_message(Msg_AddWeapon(self.caster, Weapon(self.card)))


### ------------- Card Spells ------------------------

//struct Act_PlaySpellCard(Act_PlayCard) :
//  ''' hero plays a generic spell card, specified using "actions" '''
//  def __init__(card, targets, actions) :
//  Act_PlayCard.___init___(card)
//  self.choices = [targets] if targets != None else[]
//  self.actions = actions  # execution is defined by card
//  def is_valid() :
//  return all([bool(ch) for ch in self.choices]) # choices cannot be empty for a spell
//  def execute() :
//  Act_PlayCard.execute()
//  self.engine.send_message([
//    Msg_StartSpell(self.caster, self.card),
//      self.actions(),
//      Msg_EndSpell(self.caster, self.card)])
//
//
//struct Act_SingleSpellDamageCard(Act_PlaySpellCard) :
//  ''' inflict damage to a single target'''
//  def __init__(card, targets, damage) :
//  def actions() :
//  target = self.choices[0]
//  assert type(target) != list
//  return[Msg_SpellDamage(self.caster, target, self.damage)]
//  Act_PlaySpellCard.__init__(card, targets, actions)
//  self.damage = damage
//
//struct Act_MultiSpellDamageCard(Act_PlaySpellCard) :
//  ''' inflict damage to multiple targets'''
//  def __init__(target, card, damage) :
//  Act_PlaySpellCard.__init__(card, targets, damage = damage)
//  def execute() :
//  Act_PlayCard.execute()
//  self.engine.send_message([
//    Msg_StartSpell(self.caster, self.card),
//      [Msg_SpellDamage(self.caster, t, self.damage) for t in self.choices[0]],
//      Msg_EndSpell(self.caster, self.card),
//  ])
//
//struct Act_RandomSpellDamageCard(Act_PlaySpellCard) :
//  ''' inflict damage to random targets'''
//  def __init__(card, target, damage) :
//  Act_PlaySpellCard.__init__(card, target, damage = damage)
//  def execute() :
//  Act_PlaySpellCard.execute()
//  self.engine.send_message([
//    Msg_StartSpell(self.caster, self.card),
//      [Msg_MultiRandomSpellDamage(self.caster, self.choices[0], self.damage)],
//      Msg_EndSpell(self.caster, self.card),
//  ])
//
//
//struct Act_PlaySecretCard(Act_PlaySpellCard) :
//  pass
//
//
//### ------------------- Hero powers -----------------
//
//struct Act_HeroPower(Action) :
//  '''hero power applied to something'''
//  def __init__(caster, cost, targets, actions) :
//  Action.__init__(caster, cost)
//  self.choices = [targets]
//  self.actions = actions  # execution is defined by card
//  virtual string tostr() const
//  if self.has_chosen() :
//    return "Hero Power (%d): %s on %s" % (self.cost, self.caster.hero.card.desc, self.choices[0])
//  else :
//  return "Hero Power (%d): %s" % (self.cost, self.caster.hero.card.desc)
//  def execute() :
//  self.engine.send_message([
//    Msg_UseMana(self.caster, self.cost),
//      Msg_StartHeroPower(self.caster),
//      self.actions(),
//      Msg_EndHeroPower(self.caster),
//  ])



#endif