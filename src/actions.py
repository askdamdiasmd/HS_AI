'''
list of all possible actions.
An action includes 
 - a set of possible targets and 
 - an effect (eg. to inflict damages)
'''

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



### ---------- Actions ---------------


class Action:
  def __init__(self, origin, target=None ):
    self.origin = origin  # entity (hero/card) which initiated the action
    self.target = target or Tar_All()


class Act_Damage (Action):
  def __init__(self, damage, target=None ):
    Action.__init__(self, target)
    self.damage = damage





