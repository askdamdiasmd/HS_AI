'''
Generalistic classes 
'''
import pdb
from messages import *


### ------------ generalistic thing (hero, weapon, minon) -------

class Thing (object):
  def __init__(self, card ):
      self.card = card
      self.owner = card.owner
      self.hp = self.max_hp = card.hp
      self.atq = card.atq
      
      # some status
      self.dead = False
      
      # what is below is what can be silenced
      self.action_filters = card.action_filters  # reacting to actions before selection: [(Act_class, handler),...]
      self.modifiers = card.modifiers # reacting to event at emission time, list: [(Msg_Class, event),...]
      self.effects = card.effects # list of effects without triggers: {'taunt','stealth',...}
      self.triggers = card.triggers # reacting to events at execution time, list: [(Msg_Class, event),...]

  @classmethod
  def set_engine(cls, engine):
      cls.engine = engine

  def __str__(self):
      return "%s %s %d/%d" %(type(self).__name__, self.card.name, self.atq, self.hp)

  def list_actions(self):
      assert 0, 'must be overloaded'

  def filter_action(self,action):
      t = type(action)
      for trigger,event in self.action_filters:
        t = type(msg)
        while t!=Action:
          if issubclass(t,trigger):
            action = event.execute(self,action)
          t = t.__base__
      return action # default = do nothing

#    def check_trigger(self, trigger):
#      for tr,event in self.triggers:
#        if tr==trigger: 
#          event.execute(self)

  def modify_msg(self, msg):
      for trigger,event in self.modifiers:
        t = type(msg)
        while t!=Message:
          if issubclass(t,trigger):
            msg = event.execute(self,msg)
          t = t.__base__
      return msg

  def react_msg(self, msg):
      t = type(msg)
      res = []
      for trigger,event in self.triggers:
        t = type(msg)
        while t!=Message:
          if issubclass(t,trigger):
            m = event.execute(self,msg)
            if m: res.append(m)
          t = t.__base__
      return res

  def popup(self):  # executed when created
      self.n_max_att = 2 if 'windfury' in self.effects else 1
      self.n_remaining_att = 0
      #self.check_trigger('battlecry')
  
  def start_turn(self):
      self.n_remaining_att = self.n_max_att

  def end_turn(self):
      pass

  def attacks(self):
      self.n_remaining_att -= 1
      assert self.n_remaining_att>=0

  def hurt(self, damage):
      self.hp -= damage
      self.check_dead()

  def silence(self):
        self.action_filter = []
        self.modifiers = []
        self.triggers = []
        while self.effects:
          e = self.effects.pop()
          if type(e)!=str:  e.undo(self)
    
  def check_dead(self):
        if self.hp <= 0:
          self.dead = True
          self.engine.send_message( Msg_CheckDead(self) )

  def death(self):
        pos = self.engine.board.remove_thing(self)
        for e in self.effects:
          if e[0]=='deathrattle':
            self.engine.send_message(Msg_DeathRattle(self,e[1],pos))
        self.silence()

### ------------ Weapon ----------

class Weapon (Thing):
  def __init__(self, card, hero ):
      Thing.__init__(self, card )
      self.hero = hero

  def list_actions(self):
      if self.n_remaining_att<=0:
        return None
      else:
        return Act_Attack(self, self.board.list_attackable_characters(self.owner))

  def popup(self):  # executed when created
      Thing.popup(self)
      self.n_remaining_att = 1  # has charge naturally
   
  def hurt(self, damage):
      self.hero.hurt(damage)
      self.hp -= 1
      if self.hp <= 0:
        self.dead = True
        self.engine.send_message( Msg_CheckDead(self) )


### ------------ Secret ----------

class Secret (Thing):
  def __init__(self, card, hero ):
      Thing.__init__(self, card )
      self.hero = hero
      self.active = False

  def list_actions(self):
      return None



### ------------ Creature (hero or minion) ----------

class Creature (Thing):
  def __init__(self, card ):
      Thing.__init__(self, card )

  def heal(self, heal):
        self.hp = min(self.max_hp, self.hp+heal)

  def change_hp(self, hp):
        self.hp += hp
        self.max_hp += hp
        self.cheack_dead()


### ------------ Minion ----------
from actions import Act_Attack


class Minion (Creature):
  def __init__(self, card ):
      Creature.__init__(self, card )

  def is_taunt(self):
      return 'taunt' in self.effects

  def list_actions(self):
      if self.n_remaining_att<=0 or self.atq==0:
        return None
      else:
        return Act_Attack(self, self.board.list_attackable_characters(self.owner))

  def popup(self):  # executed when created
      Creature.popup(self)
      if 'charge' in self.effects:
        self.n_remaining_att = self.n_max_att


















