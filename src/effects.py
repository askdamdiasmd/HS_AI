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
    def __init__(self, owner):
        self.owner = owner    # effect belongs to this minion
    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine
    def triggers(self):
        return {}  # effect is triggered by this Action/Message class
    def filter(self, action):
        pass  # default does nothing


### ----------- Action filters (applies on Actions) ---------------


class Acf_NotSpellTargetable (Effect):
    """ not targetable by spells or hero power"""
    def triggers(self):
        return {Act_SingleSpellDamage}
    def filter(self, action):
        try:  # remove ourselves from potential targets
          action.choices[0].remove(self.owner)
        except ValueError:
          pass
        return action


### ---------- effects (applies on Messages) ----------------

class Acf_IncSpellDamage (Effect):
    """ increase spelle damages """
    def triggers(self):
        return {Act_SpellDamageCard}
    def filter(self, action):
        pass
























