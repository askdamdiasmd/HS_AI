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
  def __init__(self, caster, target ):
      self.caster = caster  # entity (hero/card) which initiated the action
      self.target = target  # target (can be multiple or none)

  def select(self, num):
      assert 0<=num<len(self)
      return self

  def randomness(self):
      # number of possible outcomes
      return 1   # default = no randomness

  def execute(self):
      assert 0, "must be overloaded"
      self.engine. Message(self)


### -------- Minions -----------------

class Act_Minion (Action):
    def __init__(self, caster, pos, card):
        Action.__init__(self, caster, pos)
        self.card = card
    def execute(self):
        minion = Minion(self.card, self.caster)
        self.engine.send_message(Msg_AddMinion(self.caster, minion, self.target))


class Act_Attack (Action):
    ''' when one minion attacks another someone'''
    def execute(self):
        self.caster.n_remaining_attack -= 1
        assert self.caster.n_remaining_attack>=0
        
        msgs = [Msg_StartAttack(self.caster),
                Msg_Damage(self.caster, self.target, self.caster.att),
                Msg_Damage(self.target, self.caster, self.target.att),
                Msg_EndAttack(self.caster) ]
        
        if not self.target.att: msgs.pop(2) # remove useless message
        self.engine.send_message(msgs)



### ------------- Spells ------------------------

class Act_SpellDamage (Action):
    ''' just inflict damage to someone(s)'''
    def __init__(self, damage, target ):
        Action.__init__(self, target)
        self.damage = damage
    
    def execute(self):
        targets = self.target if type(self.target)==list else [self.target]
        
        self.engine.send_message([
          Msg_StartSpell(),
          [Msg_SpellDamage(self.caster,t) for t in targets], 
          Msg_EndSpell(),
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
