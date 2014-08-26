'''
list of all minions

Minions are instanciated by cards (hp, att).
The rest of their abilities is coded here.
'''
from actions import *




class AncientWatcher (Minion):
  def __init__(self, card):
    Minion.__init__(self, card)
    self.effects.append( Eff_NoAttack() )


class FeericDragon (Minion):
     def __init__(self, card):
       Minion.__init__(self, card)
       self.action_filters = AcF_NoTargetable()





