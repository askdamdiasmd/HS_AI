from messages import *
from creatures import Creature

### ------------ Hero ----------

class Hero (Creature):
  def __init__(self, card ):
      Creature.__init__(self, card )
      self.weapon = None
      self.minions = []
      self.armor = 0
      self.mana = self.max_mana = 0
      self.deck = self.cards = None # initial cards
      self.weapon = None
      self.secrets = []

  @classmethod
  def set_engine(cls, engine):
      cls.engine = engine

  def set_deck(self, deck, begin=True):
     self.deck = deck
     if begin:
       self.cards = deck.draw_init_cards(3)
     else:
       self.cards = deck.draw_init_cards(4) + [Card_Coin()]

  def add_thing(self, m, pos=0):
      if issubclass(type(m), Weapon):
        self.weapon = m
      elif if issubclass(type(m), Secret):
        self.secrets.append(m)
      else:
        self.minions.insert(pos, m)

  def remove_thing(self, m):
      if m==self.weapon:
        self.weapon = None
      else:
        self.minions.remove(m)

  def start_turn(self):
    # draw cards
    self.card.append( self.deck.draw_card() )
    
    if self.max_mana<10:
      self.max_mana += 1
    
    # reset mana
    self.mana = self.max_mana
    
    # execute minion's start turn
    for m in self.engine.board.get_friendly_minions():
        m.start_turn()

  def end_turn(self):
    self.engine.send_message( Msg_EndTurn() )

  def list_actions(self):
    # first, hero power
    res = [self.card.hero_power()]
    
    # then, weapon's attack (if any)
    if self.weapon:
      res.append(self.weapon.list_actions())

    # then, all card's actions
    for card in self.cards:
      res += card.list_actions()
    
    return res, self.mana



### -------- instanciation of heroes --------------------

from cards import Card_Minion

class Card_Hero (Card_Minion):
    def hero_power(self):
        assert 0  # return an action


### - Mage -

class Card_Janna (Card_Hero):
    def __init__(self):
        Card_Minion.__init__(self, "Janna", 0, 30, 0, cls="mage", name_fr='Jenna' )

    def hero_power(self):
        actions = lambda self: [Msg_HeroDamage(self.caster,self.choices[0],1)]
        return Act_HeroPower(self, self.engine.board.list_characters(), actions=actions)

### - Priest -

class Priest (Hero):
    def __init__(self):
        Card_Minion.__init__(self, "Anduin", 0, 30, 0, cls="priest", name_fr='Anduin' )

    def hero_power(self):
        actions = lambda self: [Msg_HeroHeal(self.caster,self.choices[0],1)]
        return Act_HeroPower(self, self.engine.board.list_characters(), actions=actions)




































