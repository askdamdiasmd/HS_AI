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
from messages import *


class Effect (object):
    def __init__(self, owner=None):
        self.owner = owner    # effect belongs to this minion
    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine
    def init(self, owner):
        self.owner = owner  # when binded to a creature
    def execute(self):
        pass  # when effect is set-up
    def filter(self, action):
        pass  # when triggered by owner.action_filters
    def trigger(self, msg):
        pass  # when triggered by owner.triggers
    def modify(self, msg):
        return None  # when triggered by owner.modifiers
    def undo(self):
        pass  # undo its effect when silenced


### ----------- Action filters (applies on Actions) ---------------


class Acf_NotSpellTargetable (Effect):
    """ not targetable by spells or hero power"""
#    def execute(self):
#        return Act_SingleSpellDamage
    def filter(self, action):
        try:  # remove ourselves from potential targets
          action.choices[0].remove(self.owner)
        except ValueError:
          pass
        return action



class Acf_IncSpellDamage (Effect):
    """ increase spelle damages """
#    def triggers(self):
#        return Act_SpellDamageCard
    def filter(self, action):
        pass


### ---------- effects (applies on Messages) ----------------

class Eff_InvokeCard (Effect):
    @staticmethod
    def create_death_rattle(self, card):
      self.effects.append('death_rattle')
      self.triggers.append(('death_rattle',Eff_InvokeCard(card)))
    def __init__(self, card):
      Effect.__init__(self)
      self.card = card
    def __str__(self):
      return "Invoke a %s" % str(self.card)
    def init(self, owner):
      self.owner = self.card.owner = owner
      self.pos = self.engine.board.get_minion_pos(owner)
    def execute(self):
      assert self.owner and self.pos
      from creatures import Minion
      minion = Minion(self.card)
      self.engine.send_message(Msg_AddMinion(self.owner,minion,self.pos))


class Eff_Silence (Effect):
    def __init__(self):
        Effect.__init__(self)
    def __str__(self):
        return "silence"
    def execute(self):
        self.owner.silence()

class Eff_BuffMinion (Effect):
    def __init__(self, atq, hp, temp):
        Effect.__init__(self)
        self.atq = atq    # buff atq
        self.hp = hp      # buff hp
        self.temp = temp  # temporary effect ? (one turn)
    def __str__(self):
        return "buff %+d/%+d%s" % (self.atq, self.hp, self.temp and " (temporary)" or "")
    def execute(self):
        target = self.owner
        if self.hp:   target.change_hp(self.hp)
        if self.atq:  target.change_atq(self.atq)
        target.effects.append(self)
        if self.temp: target.triggers.append((Msg_EndTurn, self))
    def trigger(self, msg):
        assert self.temp and issubclass(type(msg),Msg_EndTurn), pdb.set_trace()
        self.owner.triggers.remove((Msg_EndTurn, self))
        self.owner.effects.remove(self)
        self.undo()
        return True
    def undo(self):
        if self.hp: self.owner.change_hp(-self.hp)
        if self.atq: self.owner.change_atq(-self.atq)


















