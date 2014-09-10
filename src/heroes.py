from messages import *
from creatures import Creature

### ------------ Hero ----------

class Hero (Creature):
  def __init__(self, card ):
      Creature.__init__(self, card, None )
      self.armor = 0
      self.hero_power = card.hero_power


### -------- instanciation of heroes --------------------

from cards import Card_Minion

class Card_Hero (Card_Minion):
    def hero_power(self):
        assert 0  # return an action


### - Mage -

class Card_Mage (Card_Hero):
    def __init__(self):
        Card_Minion.__init__(self, "Janna", 0, 30, 0, cls="mage", name_fr='Jenna' )

    def hero_power(self):
        actions = lambda self: [Msg_HeroDamage(self.caster,self.choices[0],1)]
        return Act_HeroPower(self, self.engine.board.list_characters(), actions=actions)

### - Priest -

class Card_Priest (Card_Hero):
    def __init__(self):
        Card_Minion.__init__(self, "Anduin", 0, 30, 0, cls="priest", name_fr='Anduin' )

    def hero_power(self):
        actions = lambda self: [Msg_HeroHeal(self.caster,self.choices[0],1)]
        return Act_HeroPower(self, self.engine.board.list_characters(), actions=actions)




































