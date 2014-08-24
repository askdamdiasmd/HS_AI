"""
effects and action filters

action_filters:
  they influence potential action (for instance delete some of them, due to special card power)
  for instance, "not targetables by spells or hero power" is an action filter that wil prevent (delete) those actions.
  Each time that an action is hypothetized, all action filters from the seed card and all targets are run on it.
  
effects:
  permanent minion effects, like "this minion's attack is always equal to its hp".
  They are looking at messages, (each time a new message is issued, all effects are executed),
  and they can influence the messages/actions stack.

Both are disabled (destroyed) by silence.

"""
from cards import *


class Effect:
    def __init__(self, parent):
      self.parent = parent    # effect belongs to this minion


### ----------- Action filters (are effects in broad sense) ---------------

class AcF_NotTargetable (Effect):
    """ not targetable by spells or hero power"""
    def filter(self, action):
      ori = type(action.origin)
      if issubclass(ori, Card_Spell) or issubclass(ori, Hero):
        action.targets.remove(self.parent)


### ---------- effects ----------------



