from messages import *
from creatures import Creature

### ------------ Hero ----------

class Hero (Creature):
  def __init__(self, card ):
      Creature.__init__(self, card )
      self.armor = 0
      self.hero_power = card.hero_power

  def set_owner(self, owner):
      self.owner = owner
      self.card.owner = owner

  def start_turn(self):
    pass

  def end_turn(self):
    pass

  def __str__(self):
      return "%s (%s) %dHP" % (self.owner.name,self.card.name,self.hp)

### -------- instanciation of heroes --------------------

from cards import Card_Minion
from actions import Act_HeroPower

class Card_Hero (Card_Minion):
    def hero_power(self):
        assert 0  # return an action


### - Mage -

class Card_Mage (Card_Hero):
    def __init__(self):
        Card_Minion.__init__(self, "Jaina", 0, 30, 0, cls="mage", name_fr='Jenna',
                             desc="deals 1 damage to a character")

    def hero_power(self):
        actions = lambda self: [Msg_HeroDamage(self.caster,self.choices[0],1)]
        return Act_HeroPower(self.owner, 2, self.engine.board.list_characters(), actions)

### - Priest -

class Card_Priest (Card_Hero):
    def __init__(self):
        Card_Minion.__init__(self, "Anduin", 0, 30, 0, cls="priest", name_fr='Anduin',
                             desc="heals a character by 2HP")

    def hero_power(self):
        actions = lambda self: [Msg_HeroHeal(self.caster,self.choices[0],1)]
        return Act_HeroPower(self.owner, 2, self.engine.board.list_characters(), actions)




































