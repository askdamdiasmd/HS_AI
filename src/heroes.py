from messages import *
from creatures import Creature

### ------------ Hero ----------

class Hero (Creature):
  def __init__(self, card ):
      Creature.__init__(self, card )
      self.armor = 0
      self.hero_power = card.hero_power
      self.popup()

  def set_owner(self, owner):
      self.owner = owner
      self.card.owner = owner

  def __str__(self):
      return "[%s (%s) %dHP]" % (self.owner.name,self.card.name,self.hp)

  def death(self):
    Creature.death(self)
    return Msg_DeadHero(self)


### -------- instanciation of heroes --------------------

from cards import Card_Minion
from actions import Act_HeroPower

class Card_Hero (Card_Minion):
    def __init__(self, *args, **kwargs):
        Card_Minion.__init__(self,*args,**kwargs)
        self.effects = ['charge'] # if weapon can attack immediately
    def hero_power(self):
        assert 0  # return an action


### - Mage -

class Card_Mage (Card_Hero):
    def __init__(self):
        Card_Hero.__init__(self, "Jaina", 0, 0, 30, cls="mage", name_fr='Jenna',
                             desc="deals 1 damage to a character")
        self.power_text = "Fire blast"
        self.power_subtext = "Deal 1 damage"
        self.power_cost = 2

    def hero_power(self):
        actions = lambda self: [Msg_HeroDamage(self.caster,self.choices[0],1)]
        return Act_HeroPower(self.owner, self.power_cost, 
                             self.engine.board.list_characters(), actions)

### - Priest -

class Card_Priest (Card_Hero):
    def __init__(self):
        Card_Hero.__init__(self, "Anduin", 0, 0, 30, cls="priest", name_fr='Anduin',
                             desc="heals a character by 2HP")
        self.power_text = "Lesser heal"
        self.power_subtext = "Restore 2 health"
        self.power_cost = 2

    def hero_power(self):
        actions = lambda self: [Msg_HeroHeal(self.caster,self.choices[0],2)]
        return Act_HeroPower(self.owner, self.power_cost, 
                             self.engine.board.list_characters(), actions)




































