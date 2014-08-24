from messages import *
from cards import fake_deck

### ------- Heroes -------------

class Hero:
  @classmethod
  def set_engine(cls, engine):
    cls.engine = engine

  def __init__(self, name, hp=30, turn=0 ):
    self.name = name
    self.hp = self.max_hp = hp
    self.armor = 0
    self.secrets = []
    self.weapon = None
    self.mana = self.max_mana = 0
    self.deck = self.cards = None # initial cards

  def set_deck(self, deck, begin=True):
     self.deck = deck
     if begin:
       self.cards = deck.draw_init_cards(3)
     else:
       self.cards = deck.draw_init_cards(4) + [Card_Coin()]

  def start_turn(self):
    # draw cards
    self.card.append( self.deck.draw_card() )
    
    if self.max_mana<10:
      self.max_mana += 1
    # set mana
    self.mana = self.max_mana

  def end_turn(self):
    self.engine.send_message( Msg_EndTurn() )

  def list_actions(self):
    # first, hero power
    if self.mana >= 2:
      res = [self.hero_power()]
    else:
      res = []
    
    # then, weapon's attack (if any)
    if self.weapon:
      res += self.weapon.list_actions()

    # then, all card's actions
    for card in self.cards:
      res += card.list_actions()
    
    return res



class Mage (Hero):
  def __init__(self):
    Hero.__init__(self, 'Janna' )

  def hero_power(self):
    return Act_Damage(self, self.board.list_characters(), 1)


class Priest (Hero):
  def __init__(self):
    Hero.__init__(self, 'Anduin' )

  def hero_power(self):
    return Act_Heal(self, self.board.list_characters(), 1)


































