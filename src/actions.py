'''
list of all possible actions.
An action includes 
 - a set of possible targets and 
 - an effect (eg. to inflict damages)
'''

# ------------ Targets ---------------

class Target:
  def __init__(self, foe=None ):
    # foe = friends or enemy: 
    #   None = whichever
    #   hero = belonging to this hero
    self.foe = foe

  def filter_foe(self, targets):
    if self.foe!=None:
      return [t for t in targets if t.hero == self.foe]
    else:
      return targets

  def filter_targets(self, targets):
    return targets # default = does nothing

  def get_targets(self):
    targets = self.board.everybody()
    targets = self.filter_foe( targets )
    return self.filter_targets( targets )

#---
class Tar_All (Target):
  pass

#---
class Tar_Minions (Target):
  def get_targets(self):
    targets = self.board.minions()
    targets = self.filter_foe( targets )
    return self.filter_targets( targets )



### ---------- Actions ---------------


class Action:
  def __init__(self, target=None ):
    self.target = target or Tar_All()



class Act_Damage (Action):
  def __init__(self, damage, target=None ):
    Action.__init__(self, target)
    self.damage = damage



