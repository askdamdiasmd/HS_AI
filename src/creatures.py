'''
Generalistic classes
'''
import pdb
from messages import *
from copy import copy


### ------------ generalistic thing (hero, weapon, minon) -------

class Thing (object):
  def __init__(self, card ):
      self.card = card
      self.owner = card.owner
      self.hp = self.max_hp = card.hp
      self.atq = self.max_atq = card.atq

      # some status
      self.dead = False

      # what is below is what can be silenced
      self.action_filters = copy(card.action_filters)  # reacting to actions before selection: [(Act_class, handler),...]
      self.modifiers = copy(card.modifiers) # reacting to event at emission time, list: [(Msg_Class, event),...]
      self.effects = copy(card.effects) # list of effects without triggers: {'taunt','stealth',...}
      self.triggers = copy(card.triggers) # reacting to events at execution time, list: [(Msg_Class, event),...]

  @classmethod
  def set_engine(cls, engine):
      cls.engine = engine

  def __str__(self):
      return "%s %s %d/%d" %(type(self).__name__, self.card.name, self.atq, self.hp)

  def list_actions(self):
      assert 0, 'must be overloaded'

  def filter_action(self,action):
      for trigger,event in self.action_filters:
        if issubclass(type(msg),trigger):
          action = event.execute(self,action)
      return action # default = do nothing

  def modify_msg(self, msg):
      for trigger,event in self.modifiers:
        if issubclass(type(msg),trigger):
          msg = event.execute(self,msg)
      return msg

  def react_msg(self, msg):
      res = []
      for trigger,event in self.triggers:
        if type(trigger)==str: continue
        if issubclass(type(msg),trigger):
          m = event.execute(self,msg)
          if m: res.append(m)
      return res

  def has_effect(self, effect):
      return effect in self.effects

  def popup(self):  # executed when created
      self.n_max_atq = 2 if self.has_effect('windfury') else 1
      if self.has_effect('charge'):
        self.n_atq = self.n_max_atq
      else:
        self.n_atq = 0
      
  def start_turn(self):
      self.n_atq = 0 if self.has_effect('frozen') else self.n_max_atq

  def end_turn(self):
      pass

  def silence(self):
      self.action_filter = []
      self.modifiers = []
      self.triggers = []
      while self.effects:
        e = self.effects.pop()
        if type(e)!=str:  e.undo(self)
      self.effects = ['silence']  
      self.engine.display_msg(Msg_Status(self,'effects'))

  def death(self):
      pos = self.engine.board.get_minion_pos(self)
      if self.has_effect('death_rattle'):
        for trigger,eff in self.triggers:
          if trigger=='death_rattle':
            eff.init(self.owner,pos)
            self.engine.send_message(Msg_DeathRattle(self,eff), immediate=True)
      self.engine.board.remove_thing(self)


### ------------ Secret ----------

class Secret (Thing):
  def __init__(self, card, hero ):
      Thing.__init__(self, card )
      self.hero = hero
      self.active = False

  def list_actions(self):
      return None

  def death(self):
    Thing.death(self)
    self.engine.send_message( Msg_DeadSecret(self), immediate=True)


### ------------ Weapon ----------

class Weapon (Thing):
  def __init__(self, card ):
      Thing.__init__(self, card )
      self.hero = card.owner.hero

  def list_actions(self):
      if self.hero.n_atq<=0:
        return []
      else:
        from actions import Act_WeaponAttack
        return [Act_WeaponAttack(self, self.engine.board.list_attackable_characters(self.owner))]

  def attacks(self, target):
      self.hero.n_atq -= 1
      assert self.hero.n_atq>=0
      msgs = [Msg_Damage(self, target, self.atq)]
      if target.atq: msgs.append(Msg_Damage(target, self.hero, target.atq))
      self.hp -= 1  # lose one durability
      self.engine.display_msg(Msg_Status(self,'hp'))
      if self.hp <= 0:
        self.dead = True
        msgs.append( Msg_CheckDead(self) )
      self.engine.send_message([Msg_StartAttack(self,target),
                                msgs,
                                Msg_EndAttack(self)])

  def death(self):
    Thing.death(self)
    self.engine.send_message( Msg_DeadWeapon(self), immediate=True)


### ------------ Creature (hero or minion) ----------

class Creature (Thing):
  def __init__(self, card ):
      Thing.__init__(self, card )

  def hurt(self, damage):
      self.hp -= damage
      self.engine.display_msg(Msg_Status(self,'hp'))
      self.check_dead()

  def heal(self, hp):
      self.hp = min(self.max_hp, self.hp+hp)
      self.engine.display_msg(Msg_Status(self,'hp'))

  def change_hp(self, hp):
        self.hp += hp
        self.max_hp += hp
        self.engine.display_msg(Msg_Status(self,'hp max_hp'))
        self.cheack_dead()

  def check_dead(self):
      if self.hp <= 0:
        self.dead = True
        self.engine.send_message( Msg_CheckDead(self) )

  def attacks(self, target):
      self.n_atq -= 1
      assert self.n_atq>=0
      msgs = [Msg_Damage(self, target, self.atq)]
      if target.atq: msgs.append(Msg_Damage(target, self, target.atq))
      self.engine.send_message([Msg_StartAttack(self,target),
                                msgs,
                                Msg_EndAttack(self)])




### ------------ Minion ----------


class Minion (Creature):
  def __init__(self, card ):
      Creature.__init__(self, card )

  def has_taunt(self):
      return 'taunt' in self.effects

  def list_actions(self):
      if self.n_atq<=0 or self.atq==0:
        return []
      else:
        from actions import Act_MinionAttack
        return [Act_MinionAttack(self, self.engine.board.list_attackable_characters(self.owner))]

  def death(self):
    Creature.death(self)
    self.engine.send_message( Msg_DeadMinion(self), immediate=True)

















