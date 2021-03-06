'''
list of all possible actions.

An action includes
 - a set of possible targets and
 - an effect (eg. to inflict damages)

  Actions are potential choices for the player.

  When an action is selected, it is executed, i.e.,
  it is decomposed into atomic messages, which are
  themselves executed.
'''
import pdb
from messages import *
from copy import deepcopy


### ---------- Actions ---------------


class Action (object):
  def __init__(self, caster, cost=0 ):
      self.caster = caster  # entity (minion/hero) which initiated the action
      self.cost = cost      # mana cost
      self.choices = []

  @classmethod
  def set_engine(cls, engine):
      cls.engine = engine

  def is_valid(self):
      return True

  def num_choices(self):
      return len(self.choices)

  def neighbors(self):
      return False # return True if self = minion with neighbor aura/battlecry

  def cardinality(self):
      res = 1
      for ch in self.choices:
        res *= len(ch)
      return res

  def select(self, choices):
      assert len(self.choices)==len(choices), pdb.set_trace()
      self.choices = tuple(choices)
      return self

  def has_chosen(self):
      return type(self.choices)==tuple

  def randomness(self):
      # number of possible outcomes
      return 1   # default = 1 = no randomness

  def execute(self):
      assert 0, "must be overloaded"


class Act_EndTurn (Action):
    def __init__(self, caster):
        Action.__init__(self,caster)
    def __str__(self):
        return "End turn"
    def execute(self):
        self.engine.send_message(Msg_EndTurn(self.caster))


class Act_PlayCard (Action):
    ''' hero plays a card '''
    def ___init___(self, card):
        Action.__init__(self,card.owner,card.cost)
        self.card = card
    def __str__(self):
        return "Play Card %s" % self.card
    def execute(self):
        self.engine.send_message([
            Msg_PlayCard(self.caster,self.card, self.cost)] )



### -------- Minions -----------------

class Act_PlayMinionCard (Act_PlayCard):
    ''' hero plays a minion card '''
    def __init__(self, card):
        Act_PlayCard.___init___(self,card)
        self.choices = [self.engine.board.get_free_slots(card.owner)]
    def is_valid(self):
        player = self.caster
        return len(player.minions)<7
    def neighbors(self):
        from effects import Eff_BuffNeighbors
        return any([type(e)==Eff_BuffNeighbors for e in self.card.effects])
    def execute(self):
        Act_PlayCard.execute(self)
        pos = self.choices[0]
        assert type(pos).__name__=='Slot', pdb.set_trace()
        from creatures import Minion
        self.engine.send_message(Msg_AddMinion(self.caster, Minion(self.card), pos))


class Act_PlayMinionCard_BC (Act_PlayMinionCard):
    ''' hero plays a minion card with battlecry '''
    def __init__(self, card, battlecry, chosable_targets=None, hidden_target=None):
        Act_PlayMinionCard.___init___(self,card)
        self.choices = [self.engine.board.get_free_slots(card.owner)]
        self.hidden_target = None
        if chosable_targets:  self.choices += [chosable_targets]
        elif hidden_target:  self.hidden_target = hidden_target
        self.battlecry = battlecry
    def neighbors(self):
        return self.hidden_target=='neighbors' or Act_PlayMinionCard.neighbors(self)
    def execute(self):
        Act_PlayCard.execute(self)
        pos = self.choices[0]
        assert type(pos).__name__=='Slot', pdb.set_trace()
        from creatures import Minion
        minion = Minion(self.card)
        actions = [Msg_AddMinion(self.caster, minion, pos)]
        if self.hidden_target=='neighbors':
          for target in self.card.owner.minions[max(0,pos.index-1):pos.index+1]:
            actions.append(Msg_BindEffect(minion, target, self.battlecry()))
        elif len(self.choices)>1 or self.hidden_target:
          target = self.hidden_target or self.choices[1]
          actions.append(Msg_BindEffect(minion, target, self.battlecry()))
        self.engine.send_message(actions)


class Act_Attack (Action):
    ''' when something attacks something'''
    def __init__(self, caster, targets):
        Action.__init__(self, caster)
        self.choices = [targets]
    def __str__(self):
        if self.has_chosen():
          return "Attack with %s on %s" % (self.caster, self.choices[0])
        else:
          return "Attack with %s" % self.caster
    def execute(self):
        target = self.choices[0]
        assert type(target)!=list
        self.caster.attacks(target)

class Act_MinionAttack (Act_Attack):
    ''' when one minion attacks something'''
    pass

class Act_HeroAttack (Act_Attack):
    ''' when the hero (druid) attacks something'''
    pass

class Act_WeaponAttack (Act_Attack):
    ''' when a hero attacks something'''
    pass


### ------------- Weapon cards ------------------------

class Act_PlayWeaponCard (Act_PlayCard):
    ''' hero plays a weapon card '''
    def __init__(self, card):
        Act_PlayCard.___init___(self,card)
        self.choices = []
    def execute(self):
        Act_PlayCard.execute(self)
        from creatures import Weapon
        self.engine.send_message(Msg_AddWeapon(self.caster, Weapon(self.card)))


### ------------- Card Spells ------------------------

class Act_PlaySpellCard (Act_PlayCard):
    ''' hero plays a generic spell card, specified using "actions" '''
    def __init__(self, card, targets, actions):
        Act_PlayCard.___init___(self, card)
        self.choices = [targets] if targets!=None else []
        self.actions = actions  # execution is defined by card
    def is_valid(self):
        return all([bool(ch) for ch in self.choices]) # choices cannot be empty for a spell
    def execute(self):
        Act_PlayCard.execute(self)
        self.engine.send_message([
          Msg_StartSpell(self.caster,self.card),
          self.actions(self),
          Msg_EndSpell(self.caster,self.card)])


class Act_SingleSpellDamageCard (Act_PlaySpellCard):
    ''' inflict damage to a single target'''
    def __init__(self, card, targets, damage ):
        def actions(self):
            target = self.choices[0]
            assert type(target)!=list
            return [Msg_SpellDamage(self.caster,target,self.damage)]
        Act_PlaySpellCard.__init__(self, card, targets, actions)
        self.damage = damage

class Act_MultiSpellDamageCard (Act_PlaySpellCard):
    ''' inflict damage to multiple targets'''
    def __init__(self, target, card, damage ):
        Act_PlaySpellCard.__init__(self, card, targets, damage=damage)
    def execute(self):
        Act_PlayCard.execute(self)
        self.engine.send_message([
          Msg_StartSpell(self.caster,self.card),
          [Msg_SpellDamage(self.caster,t,self.damage) for t in self.choices[0]],
          Msg_EndSpell(self.caster,self.card),
        ])

class Act_RandomSpellDamageCard (Act_PlaySpellCard):
    ''' inflict damage to random targets'''
    def __init__(self, card, target, damage ):
        Act_PlaySpellCard.__init__(self, card, target, damage=damage)
    def execute(self):
        Act_PlaySpellCard.execute(self)
        self.engine.send_message([
          Msg_StartSpell(self.caster,self.card),
          [Msg_MultiRandomSpellDamage(self.caster,self.choices[0],self.damage)],
          Msg_EndSpell(self.caster,self.card),
        ])


class Act_PlaySecretCard (Act_PlaySpellCard):
    pass


### ------------------- Hero powers -----------------

class Act_HeroPower (Action):
    '''hero power applied to something'''
    def __init__(self, caster, cost, targets, actions):
        Action.__init__(self, caster, cost )
        self.choices = [targets]
        self.actions = actions  # execution is defined by card
    def __str__(self):
        if self.has_chosen():
          return "Hero Power (%d): %s on %s" % (self.cost, self.caster.hero.card.desc, self.choices[0])
        else:    
          return "Hero Power (%d): %s" % (self.cost, self.caster.hero.card.desc)
    def execute(self):
        self.engine.send_message([
          Msg_UseMana(self.caster,self.cost),
          Msg_StartHeroPower(self.caster),
          self.actions(self),
          Msg_EndHeroPower(self.caster),
        ])



























'''
    FINALLY BAD IDEA
# ------------ Targets ---------------

class Target:
  def __init__(self, hero=None ):
    # hero = friends or enemy:
    #   None = whichever
    #   hero = belonging to this hero
    self.hero = hero

  def filter_hero(self, targets):
    if self.hero!=None:
      return [t for t in targets if t.hero == self.hero]
    else:
      return targets

  def filter_targets(self, targets):
    return targets # default = does nothing

  def get_targets(self):
    targets = self.board.everybody()
    targets = self.filter_hero( targets )
    return self.filter_targets( targets )

#---
class Tar_All (Target):
  pass

#---
class Tar_Minions (Target):
  def get_targets(self):
    targets = self.board.minions()
    targets = self.filter_hero( targets )
    return self.filter_targets( targets )
'''
