"""
a deck
"""
import random
from messages import *


class Deck (object):
    @classmethod
    def set_engine(cls,engine):
      cls.engine = engine

    def __init__(self, cards):
      self.cards = cards
      #assert len(cards)==30
      self.owner = None
      self.fatigue = 0

    def set_owner(self, owner):
      self.owner = owner
      for card in self.cards:
        card.owner = owner

    def __len__(self):
      return len(self.cards)

    def draw_one_card(self):
      if self.cards:
        r = random.randint(0, len(self.cards)-1)
        card = self.cards.pop(r)
        return card
      else:
        self.fatigue += 1
        self.engine.send_message(Msg_Fatigue(self.owner,self.fatigue))
        return None

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




### -------- Heathstone cardbook collection --------------

def fake_deck(cardbook, debug=False):
    from copy import deepcopy
    deck = []
    if debug:
      cards = ["Wicked Knife","Limon des marais acides"]
      for c in cards:
        deck += [deepcopy(cardbook[c]) for i in range(30/len(cards))]      
    else:
      cards = list(set([c for c in cardbook.values() if c.collectible]))
      deck = [deepcopy(cards[random.randint(0,len(cards)-1)])  for i in range(30)]
    
    return Deck(deck)









































