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
    def __init__(self):
        self.owner = None
        self.caster = None
    def bind_to(self, owner, caster=None):
        assert self.owner == None, pdb.set_trace()
        self.owner = owner    # effect belongs to this minion
        self.caster = caster  # who applied the effect
    def filter(self, action):
        assert False, "must be implemented"  # when triggered by owner.action_filters
    def trigger(self, msg):
        assert False, "must be implemented"  # when triggered by owner.triggers
    def modify(self, msg):
        assert False, "must be implemented"  # when triggered by owner.modifiers
    def undo(self):
        pass  # undo its effect when silenced


### ----------- Action filters (applies on Actions) ---------------


class Acf_IncSpellDamage (Effect):
    """ increase spelle damages """
#    def triggers(self):
#        return Act_SpellDamageCard
    def filter(self, action):
        pass


class Acf_SpellCost (Effect):
    """ modify spell cost """
    def __init__(self, additional_cost, min_cost=0):
        Effect.__init__(self)
        self.cost = additional_cost
        self.min_cost = min_cost
    def __str__(self):
        return "Spells cost (%d) more" % self.cost
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        owner.action_filters.append((Act_PlaySpellCard,self))
    def filter(self, act):
        if act.caster is self.owner.owner:
          act.cost = max(self.min_cost, act.cost+self.cost)
        return act


class Acf_Delete (Effect):
    """ delete a specific action """
    def __init__(self, trigger, condition):
        Effect.__init__(self)
        self.trigger = trigger
        self.condition = condition
    def __str__(self):
        return "Delete action %s" % self.trigger.__name__
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        owner.action_filters.append((self.trigger,self))
    def filter(self, act):
        if self.condition(self,act):
          act = None
        return act


### ---------- effects (applies on Messages) ----------------
  

class Eff_Silence (Effect):
    def __str__(self):
        return "silence"
    def bind_to(self, minion, caster=None):
        minion.silence()


class Eff_SpellDamage (Effect):
    """ increases spell damage """
    def __init__(self, damage):
        Effect.__init__(self)
        self.damage = damage
    def __str__(self):
        return "Spell damage %+d" % self.damage
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        owner.triggers.append((Msg_SpellDamage,self))
    def trigger(self, msg):
        if msg.caster is self.owner.owner:
          msg.damage += self.damage
        return msg


class Eff_Message (Effect):
    """ send a message to someone """
    def __init__(self, action, immediate=True):
        Effect.__init__(self)
        self.action = action
        self.immediate = immediate
    def __str__(self):
        return "Send a message: %s" % str(self.action(self))
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        self.engine.send_message(self.action(self), immediate=self.immediate)


class Eff_BuffWeapon (Effect):
    """ change stat of a weapon """
    def __init__(self, atq=0, damage=0, hp=0, temp=False, destroy=False, immediate=True):
        Effect.__init__(self)
        self.atq = atq
        self.hp = hp
        assert damage>=0
        self.damage = damage
        assert temp==False, "todo"
        self.destroy = destroy
        self.temp = temp
        self.immediate = immediate
    def __str__(self):
        which = str(self.owner.owner)+"'s" if self.owner else ''
        if self.destroy:
          return "Destroy %s weapon" % which
        else:
          return "Buff %s weapon by %+d/%+d" % (which, self.atq, self.hp or -self.damage)
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        weapon = owner
        assert type(weapon).__name__ == 'Weapon', pdb.set_trace()
        if weapon:
          if self.destroy:
            self.engine.send_message(Msg_DeadWeapon(weapon), immediate=self.immediate)
          else:
            if self.atq: weapon.change_atq(self.atq)
            if self.hp: weapon.change_hp(self.hp)
            if self.damage: weapon.hurt(self.damage)



class Eff_DeathRattle (Effect):
    """ death rattle effect """
    def __init__(self, action):
      Effect.__init__(self)
      self.action = action
    def __str__(self):
      return "Deathrattle"
    def bind_to(self, owner, caster=None):
      Effect.bind_to(self, owner, caster)
      owner.effects.append('death_rattle')
      owner.triggers.append((Msg_Dead,self)) # because we are already disappeared when it triggers
    def trigger(self, msg):
      if msg.caster is self.owner: # I'm dead !
        pos = self.engine.board.get_minion_pos(self.owner)
        self.engine.send_message(Msg_DeathRattle(self.owner,self.action(self,pos)),immediate=True)


class Eff_DR_Invoke_Minion (Effect):
    """ death rattle effect """
    def __init__(self, card):
      Effect.__init__(self)
      self.card = card
    def __str__(self):
      return "Invoke a %s" % str(self.card)
    def bind_to(self, owner, caster=None):
      Effect.bind_to(self, owner, caster)
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
    def __init__(self, atq=0, hp=0, temp=False, armor=0, others=''):
        Effect.__init__(self)
        self.atq = atq    # buff atq
        self.hp = hp      # buff hp
        self.temp = temp  # temporary effect ? (one turn)
        assert armor>=0, "error: cannot remove armor"
        self.armor = armor  # note: cannot be silenced
        assert type(others)==str
        assert not(others and self.temp), "error: buff cannot be temporary if other effects"
        self.others = others.split() # simple effects like taunt, windfury ...
    def __str__(self):
        buff = "buff %+d/%+d" % (self.atq, self.hp) if (self.hp or self.atq) else ''
        others = (' '+' '.join([str(e) for e in self.others])) if self.others else ''
        temp = self.temp and " (temporary)" or ""
        return "%s%s%s" % (buff, others, temp)
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        if self.hp or self.atq:  # we don't care if just taunt
          owner.effects.append(self)
          if self.temp: 
            owner.triggers.append((Msg_EndTurn, self))
        if self.armor:
          from heroes import Hero
          assert type(owner)==Hero, pdb.set_trace()
        self.execute()
    def execute(self):
        if self.hp:   self.owner.change_hp(self.hp)
        if self.atq:  self.owner.change_atq(self.atq)
        if self.armor:  self.owner.add_armor(self.armor)
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
        Effect.bind_to(self, owner, caster)
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


class Eff_BuffNeighbors (Effect):
    """ permanent buff of left and right minion's neighbors """ 
    def __init__(self, atq, hp):
        Effect.__init__(self)
        self.atq = atq    # buff atq
        self.hp = hp      # buff hp
        self.targets = set()
    def __str__(self):
        return "buff neighbors by %+d/%+d" % (self.atq, self.hp)
    def bind_to(self, owner):
        Effect.bind_to(self, owner, caster)
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


class Eff_BuffFriends (Effect):
    """ permanent buff of left and right minion's neighbors """ 
    def __init__(self, atq, hp, cat=None):
        Effect.__init__(self)
        self.atq = atq    # buff atq
        self.hp = hp      # buff hp
        self.cat = cat
        self.targets = set()
    def __str__(self):
        return "buff friends by %+d/%+d" % (self.atq, self.hp)
    def bind_to(self, owner):
        Effect.bind_to(self, owner, caster)
        owner.effects.append(self)
        owner.triggers += [(Msg_Popup,self), (Msg_Dead,self)]
    def get_neighbors(self):
        minion = self.owner
        player = minion.owner
        targets = set([m for m in player.minions if not self.cat or m.card.cat==self.cat])
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


class Eff_Enrage (Effect):
    def __init__(self, atq=0, windfury=False):
        Effect.__init__(self)
        self.atq = atq
        self.windfury = windfury
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        owner.effects.append(self)
        owner.enraged_trigger = self
    def trigger(self, caster):
        assert caster is self.owner, pdb.set_trace()
        enraged = self.owner.is_damaged()
        if enraged!=self.owner.enraged:
          if enraged:
            if self.atq:  self.owner.change_atq(self.atq)
            if self.windfury: self.owner.add_effects(['windfury'])
            self.owner.enraged = True
          else:
            self.undo()
    def undo(self):
        if self.owner.enraged: # owner is enraged
          if self.atq:  self.owner.change_atq(-self.atq)
          if self.windfury: self.owner.remove_effect('windfury')
          self.owner.enraged = False
  

class Eff_While (Effect):
    """ give a target an effect while a condition is met """
    def __init__(self, triggers, condition, effect, 
                 prerequisite=lambda self,msg: msg.caster is self.owner,
                 target=lambda self: self.owner):
        Effect.__init__(self)
        self.triggers = triggers
        self.condition = condition
        self.effect = effect
        self.target = target
        self.prerequisite = prerequisite
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        for trigger in self.triggers:
          owner.triggers.append((trigger, self))
    def trigger(self, msg):
        if self.prerequisite(self,msg):
          targ = self.target(self)
          if self.condition(self,msg):
            targ.add_effects((self.effect,))
          else:
            targ.remove_effect(self.effect)
    def undo(self):
        self.owner.remove_effect(self.effect)



class Eff_DrawCard (Effect):
    def __init__(self, trigger, condition, immediate):
        Effect.__init__(self)
        self.trig_msg = trigger
        self.condition = condition
        self.immediate = immediate
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        owner.effects.append("effect")
        owner.triggers.append((self.trig_msg, self))
    def trigger(self, msg):
        if self.condition(self,msg):
          caster = self.owner.owner
          self.engine.send_message(Msg_DrawCard(caster),immediate=self.immediate)


class Eff_GiveCharge (Effect):
    """ give charge to others minions while on the board """
    def __init__(self, condition, retroactive=False, permanent=True):
        Effect.__init__(self)
        self.condition = condition
        self.retroactive = retroactive  # give charge to minions already on board
        self.permanent = permanent
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        owner.triggers.append((Msg_MinionPopup, self))
    def non_permanent_effect(self):
        card_id = self.owner.card.id # (tundra rhino) card id
        return Eff_While((Msg_MinionPopup, Msg_DeadMinion),
          lambda self,msg: any([m.card.id==card_id for m in self.owner.owner.minions if not m.dead]),  # while tundra rhino exists
          'charge', prerequisite=lambda self,msg: self.owner.owner is msg.caster.owner) # check that msg is for same player
    def trigger(self, msg):
        if self.retroactive and msg.caster is self.owner:  
          for m in self.owner.owner.minions:
            if self.condition(self,m) and not m.has_effect('charge'):
              m.add_effects(['charge'])
              if not self.permanent: # has charge while owner exist
                charge = Msg_BindEffect(self.owner,m,self.non_permanent_effect())
                self.engine.send_message(charge,immediate=True)
        elif msg.caster.owner is self.owner.owner and self.condition(self,msg.caster):
          if not msg.caster.has_effect("charge"):
            m = msg.caster
            m.add_effects(['charge'])
            if not self.permanent: # has charge while owner exist
              charge = Msg_BindEffect(self.owner,m,self.non_permanent_effect())
              self.engine.send_message(charge,immediate=True)



class Eff_Trigger (Effect):
    """ generic effect: send a message when it triggers """
    def __init__(self, trigger, condition, msg_func, immediate=True):
        Effect.__init__(self)
        self.trig_msg = trigger
        self.condition = condition
        self.msg_func = msg_func
        self.immediate = immediate
    def bind_to(self, owner, caster=None):
        Effect.bind_to(self, owner, caster)
        owner.triggers.append((self.trig_msg, self))
    def trigger(self, msg):
        if self.condition(self,msg):
          self.engine.send_message(self.msg_func(self,msg),immediate=self.immediate)

























