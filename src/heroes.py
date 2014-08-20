from messages import *
from cards import fake_deck

### ------- Heroes -------------

class Hero:
  @classmethod
  def set_engine(cls, engine):
    cls.engine = engine

  def __init__(self, name, deck, hp=30, turn=0 ):
    self.name = name
    self.hp = hp
    self.deck = deck
    self.weapon = None
    self.mana = self.turn = 0

  def start_turn(self):
    # draw cards
    if self.turn==0:
      self.cards = self.deck.draw_cards_init(self.begin)
    else:
      self.cards = self.deck.draw_one_card()
    
    self.turn += 1
    # set mana
    self.mana = min(10, self.turn)
    
    self.engine.send_message( Msg_StartTurn() )

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
    Hero.__init__(self, 'Janna', fake_deck() )

  def hero_power(self):
    return Act_Damage( 1 )


































