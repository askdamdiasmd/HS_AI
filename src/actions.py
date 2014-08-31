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

from messages import *



### ---------- Actions ---------------


class Action:
  def __init__(self, caster, cost=0 ):
      self.caster = caster  # entity (minion/hero) which initiated the action
      self.cost = cost      # mana cost
      self.choices = []

  @classmethod
  def set_engine(cls, engine):
      cls.engine = engine

  def num_choices(self):
      return len(self.choices)

  def cardinality(self):
      res = 1
      for ch in self.choices:
        res *= len(ch)
      return res

  def select(self, nums):
      assert len(nums)==self.num_choices()     
      for i,n in enumerate(nums):
        assert 0<=num<len(self.choices[i])
        self.choices[i] = self.choices[i].pop(n)

  def randomness(self):
      # number of possible outcomes
      return 1   # default = 1 = no randomness

  def execute(self):
      assert 0, "must be overloaded"


class Act_PlayCard (Action):
    ''' hero plays a card '''
    def ___init___(self, caster, card):
        Action.__init__(self,caster,card.cost)
        self.card = card
    def execute(self):
        self.engine.send_message(Msg_BurnCard(self.caster,self.card))



### -------- Minions -----------------

class Act_PlayMinionCard (Act_PlayCard):
    ''' hero plays a minion card '''
    def __init__(self, caster, card, pos):
        Act_PlayCard.___init___(self,caster,card)
        self.choices = [pos]
    def execute(self):
        pos = self.choices[0]
        assert type(pos)==int
        Act_PlayCard.execute(self)
        minion = Minion(self.card, self.caster)
        self.engine.send_message(Msg_AddMinion(self.caster, minion, pos))


class Act_Attack (Action):
    ''' when something attacks something'''
    def __init__(self, caster, targets):
        Action.__init__(self, caster)
        self.choices = [targets]
    def execute(self):
        target = self.choices[0]
        assert type(target)!=list
        self.caster.n_remaining_attack -= 1
        assert self.caster.n_remaining_attack>=0
        msgs = [Msg_StartAttack(self.caster),
                [Msg_Damage(self.caster, target, self.caster.att),
                Msg_Damage(target, self.caster, target.att)],
                Msg_EndAttack(self.caster) ]
        if not target.att: msgs.pop(2) # remove useless message
        self.engine.send_message(msgs)


class Act_MinionAttack (Act_Attack):
    ''' when one minion attacks something'''
    pass

class Act_WeaponAttack (Act_Attack):
    ''' when a hero attacks something'''
    pass


### ------------- Weapon cards ------------------------

class Act_PlayWeaponCard (Act_PlayCard):
    ''' hero plays a weapon card '''


### ------------- Card Spells ------------------------

class Act_PlaySpellCard (Act_PlayCard):
    ''' hero plays a generic spell card, specified using "actions" '''
    def __init__(self, caster, targets, card, actions=None):
        Act_PlayCard.___init___(self, caster, targets, card)
        self.actions = actions  # execution is defined by card
    def execute(self):
        Act_PlaySpellCard.execute(self)
        self.engine.send_message([
          Msg_StartSpell(),
          self.actions(self), 
          Msg_EndSpell(),
        ])


class Act_SingleSpellDamageCard (Act_PlaySpellCard):
    ''' inflict damage to a single target'''
    def __init__(self, caster, target, card, damage ):
        Act_PlaySpellCard.__init__(self, caster, target, card)
        self.damage = damage
    def execute(self):
        Act_PlaySpellCard.execute(self)
        target = self.choices[0]
        assert type(target)!=list
        self.engine.send_message([
          Msg_StartSpell(),
          [Msg_SpellDamage(self.caster,target,self.damage)], 
          Msg_EndSpell(),
        ])

class Act_MultiSpellDamageCard (Act_PlaySpellCard):
    ''' inflict damage to multiple targets'''
    def __init__(self, caster, target, card, damage ):
        Act_PlaySpellCard.__init__(self, caster, targets, card, damage=damage)
    def execute(self):
        Act_PlaySpellCard.execute(self)
        self.engine.send_message([
          Msg_StartSpell(),
          [Msg_SpellDamage(self.caster,t,self.damage) for t in self.choices[0]], 
          Msg_EndSpell(),
        ])

class Act_RandomSpellDamageCard (Act_PlaySpellCard):
    ''' inflict damage to random targets'''
    def __init__(self, caster, target, card, damage ):
        Act_PlaySpellCard.__init__(self, caster, target, card, damage=damage)
    def execute(self):
        Act_PlaySpellCard.execute(self)
        self.engine.send_message([
          Msg_StartSpell(),
          [Msg_MultiRandomSpellDamage(self.caster,self.choices[0],self.damage)], 
          Msg_EndSpell(),
        ])



### ------------------- Hero powers -----------------

class Act_HeroPower (Action):
    '''hero power applied to something'''
    def __init__(self, caster, targets, actions=None):
        Act_PlayCard.___init___(self, caster, targets, card)
        self.actions = actions  # execution is defined by card
    def execute(self):
        self.engine.send_message([
          Msg_StartHeroPower(),
          self.actions(self), 
          Msg_EndHeroPower(),
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
