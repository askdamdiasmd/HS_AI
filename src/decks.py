"""
a deck
"""
import random


class Deck:
    @classmethod
    def set_engine(cls,engine):
      cls.engine = engine

    def __init__(self, hero, cards):
      self.hero = hero
      self.cards = cards
      assert len(cards)==30
    
    def draw_init_cards(self, nb):
      cards = [self.draw_card() for n in range(nb)]
      mulligan = self.engine.mulligan(cards)
      
      # draw replacement cards
      for c in sorted(mulligan)[::-1]:
        cards.append( self.draw_card() )
      
      # put mulliganed card back in the deck
      for c in sorted(mulligan)[::-1]:
        self.deck.append(cards.pop(c))

    def draw_card(self):
      r = random.randint(len(cards))
      card = self.cards.pop(r)
      card.set_owner(self.hero)
      return card
    






