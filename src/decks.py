"""
a deck
"""
import random


class Deck (object):
    @classmethod
    def set_engine(cls,engine):
      cls.engine = engine

    def __init__(self, cards):
      self.cards = cards
      assert len(cards)==30
    
    def set_owner(self, owner):
      for card in self.cards:
        card.owner = owner

    def draw_one_card(self):
      r = random.randint(0, len(self.cards)-1)
      card = self.cards.pop(r)
      return card
    
    def draw_init_cards(self, nb, mulligan):
      cards = [self.draw_one_card() for n in range(nb)]
      discard = mulligan(cards)
      
      # put mulliganed card back in the deck
      kept = [c for c in cards if c not in discard]
      self.cards += discard
      
      # draw replacement cards
      cards = kept
      for c in discard:
        cards.append( self.draw_one_card() )
      return cards





