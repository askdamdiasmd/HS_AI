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
    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine
    def __init__(self, owner=None):
        self.owner = owner    # effect belongs to this minion
    def bind_to(self, owner):
        self.owner = owner  # when binded to a creature
        owner.effects.append(self) # associate effect to a minion/card
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
  

class Eff_Silence (Effect):
    def __init__(self):
        Effect.__init__(self)
    def __str__(self):
        return "silence"
    def bind_to(self, minion):
        minion.silence()


class Eff_DR_Invoke_Minion (Effect):
    """ death rattle effect """
    def __init__(self, card):
      Effect.__init__(self)
      self.card = card
    def __str__(self):
      return "Invoke a %s" % str(self.card)
    def bind_to(self, owner):
      self.owner = owner
      owner.effects.append('death_rattle')
      owner.triggers.append(('death_rattle',self))
    def execute(self):
      pos = self.engine.board.get_minion_pos(self.owner)
      from creatures import Minion
      player = self.owner.owner
      self.card.owner = player  # set card owner
      msg = Msg_AddMinion(player, Minion(self.card),pos)
      self.engine.send_message(Msg_DeathRattle(self.owner, msg))


class Eff_BuffMinion (Effect):
    """ buff a minion (just for this turn of permanent) """
    def __init__(self, atq, hp, temp=False, others=None):
        Effect.__init__(self)
        self.atq = atq    # buff atq
        self.hp = hp      # buff hp
        self.temp = temp  # temporary effect ? (one turn)
        assert not(others and self.temp), "error: buff cannot be temporary if other effects"
        if type(others)==str: others=others.split()
        self.others = others
    def __str__(self):
        return "buff %+d/%+d%s" % (self.atq, self.hp, self.temp and " (temporary)" or "")
    def bind_to(self, owner):
        self.owner = owner
        owner.effects.append(self)
        if self.temp: 
          owner.triggers.append((Msg_EndTurn, self))
        self.execute()
    def execute(self):
        if self.hp:   self.owner.change_hp(self.hp)
        if self.atq:  self.owner.change_atq(self.atq)
        if self.others: self.owner.add_effects(self.others)
    def trigger(self, msg): # end of temporary effect
        assert self.temp and issubclass(type(msg),Msg_EndTurn), pdb.set_trace()
        self.owner.triggers.remove((Msg_EndTurn, self))
        self.owner.effects.remove(self)
        self.undo()
        return True
    def undo(self):
        if self.hp: self.owner.change_hp(-self.hp)
        if self.atq: self.owner.change_atq(-self.atq)


class Eff_BuffLeftRight (Effect):
    """ permanent buff of left and right minion's neighbors """ 
    def __init__(self, atq, hp):
        Effect.__init__(self)
        self.atq = atq    # buff atq
        self.hp = hp      # buff hp
        self.targets = set()
    def __str__(self):
        return "buff neighbors by %+d/%+d" % (self.atq, self.hp)
    def bind_to(self, owner):
        self.owner = owner
        owner.effects.append(self)
        owner.triggers += [(Msg_Popup,self), (Msg_Dead,self)]
    def get_neighbors(self):
        minion = self.owner
        player = minion.owner
        i = player.minions.index(minion)
        targets = set(player.minions[max(0,i-1):i+2])
        targets.remove(minion)
        return targets
    def execute(self):
        for target in self.targets:
          if self.hp:   target.change_hp(self.hp)
          if self.atq:  target.change_atq(self.atq)
    def trigger(self, msg):
        if msg.caster.owner is self.owner.owner: # avoid useless checks
          ngh = self.get_neighbors()
          if self.targets != ngh: # there has been change of neighbors
            self.undo()
            self.targets = ngh
            self.execute()
    def undo(self):
        for target in self.targets:
          if self.hp: target.change_hp(-self.hp)
          if self.atq: target.change_atq(-self.atq)

















