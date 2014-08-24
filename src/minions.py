'''
list of all minions

Minions are instanciated by cards (hp, att).
The rest of their abilities is coded here.
'''
from actions import *


class Minion:
  def __init__(self, card ):
    self.name = card.name
    self.hp = self.max_hp = card.hp
    self.att = card.att
    
    # some status
    self.dead = False
    self.frozen = False
    self.n_remaining_att = 0
    
    # what is below is what can be silenced
    self.effects = []
    # usage of Action filter : given an action, return the filtered action 
    self.action_filter = lambda x: x

  @classmethod
  def set_engine(cls, engine):
    cls.engine = engine

  def list_actions(self):
    if self.n_remaining_att<=0 or self.att==0 or self.frozen:
      return None
    else:
      res = Act_Attack(self, self.board.list_attackable_characters())
      return self.action_filter(res)

  def popup(self):  # executed when created
    if 'charge' in self.effects:
        self.n_remaining_att = 1
  
  def start_turn(self):
    self.frozen = False
    
    # undo temporary buffs
    i = 0
    while i<len(self.effects):
        e = self.effects[i]
        if e[0]=='temporary':
            self.effects.pop(i)
            e.undo()
        else:
            i+=1

  def hurt(self, damage):
    self.hp -= damage
    if self.hp <= 0:
      self.dead = True
      self.engine.send_message( Msg_Dying(self) )

  def heal(self, heal):
      self.hp = min(self.max_hp, self.hp+heal)

  def silence(self):
      self.action_filter = None
      while self.effects:
          e = self.effects.pop()
          e.undo(self)




class AncientWatcher (Minion):
  def __init__(self, card):
    Minion.__init__(self, card)
    self.effects.append( Eff_NoAttack() )


class FeericDragon (Minion):
     def __init__(self, card):
       Minion.__init__(self, card)
       self.action_filters = AcF_NoTargetable()





