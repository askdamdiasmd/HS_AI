'''
list of all possible actions.
An action includes 
 - a set of possible targets and 
 - an effect (eg. to inflict damages)
'''

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
from messages import *



### ---------- Actions ---------------


class Action:
  def __init__(self, origin, target ):
      self.origin = origin  # entity (hero/card) which initiated the action
      self.target = target  # target (can be multiple or none)

  def __len__(self):
      return 1

  def select(self, num):
      assert 0<=num<len(self)
      return self

  def randomness(self):
      # number of possible outcomes
      return 1   # default = no randomness

  def execute(self):
      assert 0, "must be overloaded"
      return Message(self)



class Act_Minion (Action):
    def __init__(self, owner, instanciate_func, pos):
        Action.__init__(self, owner, None)
        self.instanciate = instanciate_func
        self.pos = pos
    def execute(self):
        minion = self.instanciate(self.owner)
        self.engine.board.insert_minion(minion, pos)
        minion.popup()  # execute when created



class Act_Damage (Action):
    ''' just inflict damage to someone(s)'''
    def __init__(self, damage, target ):
        Action.__init__(self, target)
        self.damage = damage
    
    def execute(self):
        if type(self.target)!=list:
            self.target = [self.target]
        return [Msg_Damage(self.origin,t) for t in self.target]



class Act_Attack (Action):
    ''' when one minion attacks another someone'''
    def execute(self):
        self.origin.n_remaining_attack -= 1
        msgs = [Msg_Damage(self.origin, self.target, self.origin.att)]
        if self.target.att: 
          msgs.append(Msg_Damage(self.target, self.origin, self.target.att))
        return msgs






