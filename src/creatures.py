'''
Generalistic classes 
'''

### ------------ generalistic thing (hero, weapon, minon) -------

class Thing:
    def __init__(self, name, att, action_filters, listeners ):
      self.att = card.att
      
      # some status
      self.dead = False
      self.frozen = False
      self.n_remaining_att = 0
      
      # what is below is what can be silenced
      self.action_filter = action_filter  # usage of Action filter : given an action, return the filtered action 
      self.listeners = listeners # reacting to events

    def list_actions(self):
      assert 0, 'must be overloaded'

    def hurt(self, damage):
      self.hp -= damage
      if self.hp <= 0:
        self.dead = True
        self.engine.send_message( Msg_Dying(self) )



### ------------ Weapon ----------

class Weapon (Thing):
    def __init__(self, card, hero ):
      Thing.__init__(self, card.name, card.hp, card.att, 
                        card.action_filters, card.listeners )
      self.hero = hero

    def list_actions(self):
      res = Act_Attack(self, self.board.list_attackable_characters())
      return self.action_filter(res)




### ------------ Creature (hero or minion) ----------

class Creature (Thing):
    def __init__(self, name, hp, att ):
      self.name = name
      self.hp = self.max_hp = hp
      self.att = att
      
      self.temp_effects = []

    def heal(self, heal):
        self.hp = min(self.max_hp, self.hp+heal)

    def start_turn(self):
      self.frozen = False
      
      # undo temporary buffs
      i = 0
      while self.temp_effects:
        e = self.temp_effects.pop()
        e.undo()


### ------------ Hero ----------

class Hero (Creature):
    def __init__(self, name, hp, att ):
      Creature.__init__(self, name, hp, att )
      self.weapon = None



### ------------ Minion ----------

class Minion (Creature):
    def __init__(self, card ):
      Creature.__init__(self, card.name, card.hp, card.att, 
                        card.action_filters, card.listeners )
      self.card = card
      
      self.taunt = False
      
      self.effects = [] # buffs/debuffs : can be silenced

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

    def hurt(self, damage):
      self.hp -= damage
      if self.hp <= 0:
        self.dead = True
        self.engine.send_message( Msg_Dying(self) )

    def heal(self, heal):
        self.hp = min(self.max_hp, self.hp+heal)

    def change_hp(self, hp):
        self.hp += hp
        self.max_hp += hp

    def silence(self):
        self.action_filter = None
        while self.effects:
            e = self.effects.pop()
            e.undo(self)


















