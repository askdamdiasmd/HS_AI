"""
a deck
"""
import random


class Deck:
    @classmethod
    def set_engine(cls,engine):
      cls.engine = engine

    def __init__(self, cards):
      self.cards = cards
      assert len(cards)==30
    
    def draw_card(self):
      r = random.randint(len(cards))
      card = self.cards.pop(r)
      card.set_owner(self.hero)
      return card
    
    def draw_init_cards(self, nb, mulligan):
      cards = [self.draw_card() for n in range(nb)]
      keep, discard = mulligan(cards)
      
      # put mulliganed card back in the deck
      self.deck += discard
      
      # draw replacement cards
      for c in range(len(discard)):
        cards.append( self.draw_card() )






