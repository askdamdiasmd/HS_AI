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
    def bind_to(self, owner, caster=None):
        self.owner = owner  # when binded to a creature
        owner.effects.append(self) # associate effect to a minion/card
    def filter(self, action):
        assert False, "must be implemented"  # when triggered by owner.action_filters
    def trigger(self, msg):
        assert False, "must be implemented"  # when triggered by owner.triggers
    def modify(self, msg):
        assert False, "must be implemented"  # when triggered by owner.modifiers
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
    def bind_to(self, minion, caster=None):
        minion.silence()


class Eff_DR_Invoke_Minion (Effect):
    """ death rattle effect """
    def __init__(self, card):
      Effect.__init__(self)
      self.card = card
    def __str__(self):
      return "Invoke a %s" % str(self.card)
    def bind_to(self, owner, caster=None):
      self.owner = owner
      owner.effects.append('death_rattle')
      owner.triggers.append((Msg_Dead,self)) # because we are already disappeared when it triggers
    def trigger(self, msg):
      if msg.caster is self.owner: # I'm dead !
        pos = self.engine.board.get_minion_pos(self.owner)
        from creatures import Minion
        player = self.owner.owner
        self.card.owner = player  # set card owner
        death_rattle = Msg_AddMinion(player, Minion(self.card),pos)
        self.engine.send_message(Msg_DeathRattle(self.owner, death_rattle),immediate=True)


class Eff_BuffMinion (Effect):
    """ buff a minion (just for this turn of permanent) """
    def __init__(self, atq, hp, temp=False, others=''):
        Effect.__init__(self)
        self.atq = atq    # buff atq
        self.hp = hp      # buff hp
        self.temp = temp  # temporary effect ? (one turn)
        assert type(others)==str
        assert not(others and self.temp), "error: buff cannot be temporary if other effects"
        self.others = others.split() # simple effects like taunt, windfury ...
    def __str__(self):
        buff = "buff %+d/%+d" % (self.atq, self.hp) if (self.hp or self.atq) else ''
        others = (' '+' '.join([str(e) for e in self.others])) if self.others else ''
        temp = self.temp and " (temporary)" or ""
        return "%s%s%s" % (buff, others, temp)
    def bind_to(self, owner, caster=None):
        self.owner = owner
        if self.hp or self.atq:  # we don't care if just taunt
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
    def undo(self):
        if self.hp: self.owner.change_hp(-self.hp)
        if self.atq: self.owner.change_atq(-self.atq)


class Eff_DieSoon (Effect):
    """ buff a minion (just for this turn of permanent) """
    def __init__(self, death_trigger, condition=lambda self,msg:True):
        Effect.__init__(self)
        self.death_trigger = death_trigger    # type, ex: Msg_EndTurn
        self.condition = condition
    def __str__(self):
        return "Death on %s" % (self.death_trigger.__name__)
    def bind_to(self, owner, caster=None):
        self.owner = owner
        owner.triggers.append((self.death_trigger, self))
    def trigger(self, msg): # end of temporary effect
        if self.condition(self,msg):
          self.owner.dead = True
          self.engine.send_message(Msg_CheckDead(self.owner))
          #self.owner.ask_for_death()


class Eff_Absorb (Effect):
    """ buff a minion (just for this turn of permanent) """
    def __init__(self):
        Effect.__init__(self)
    def __str__(self):
        return "Absorb hp and atq"
    def bind_to(self, owner, caster=None):
        eff = Eff_BuffMinion(owner.atq,owner.hp)
        self.engine.send_message(Msg_BindEffect(owner,caster,eff),immediate=True)
        owner.dead = True
        self.engine.send_message(Msg_CheckDead(owner))


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


class Eff_DrawCard (Effect):
    def __init__(self, trigger, condition, immediate):
        Effect.__init__(self)
        self.trig_msg = trigger
        self.condition = condition
        self.immediate = immediate
    def bind_to(self, owner, caster=None):
        self.owner = owner
        owner.effects.append("effect")
        owner.triggers.append((self.trig_msg, self))
    def trigger(self, msg):
        if self.condition(self,msg):
          caster = self.owner.owner
          self.engine.send_message(Msg_DrawCard(caster),immediate=self.immediate)













