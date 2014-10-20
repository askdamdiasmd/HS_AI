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
    from copy import deepcopy as copy
    deck = []
    if debug:
      cards = ["Sunfury Protector"]
      for c in cards:
        deck += [copy(cardbook[c]) for i in range(30/len(cards))]      
    else:
      deck += [copy(cardbook["Wisp"]) for i in range(2)]
      deck += [copy(cardbook["Power Overwhleming"]) for i in range(2)]
      deck += [copy(cardbook["Abusive Sergeant"]) for i in range(2)]
      deck += [copy(cardbook["Goldshire Footman"]) for i in range(2)]
      deck += [copy(cardbook["Nerubian Egg"]) for i in range(2)]
      deck += [copy(cardbook["Sunfury Protector"]) for i in range(2)]
      deck += [copy(cardbook["Defender of Argus"]) for i in range(2)]
      deck += [copy(cardbook["Harvest Golem"]) for i in range(2)]
      deck += [copy(cardbook["River Crocolisk"]) for i in range(2)]
      deck += [copy(cardbook["Chillwind Yeti"]) for i in range(2)]
      deck += [copy(cardbook["Sludge Belcher"]) for i in range(2)]
      deck += [copy(cardbook["Savannah Highmane"]) for i in range(2)]
      deck += [copy(cardbook["Cairne"]) for i in range(1)]
      for i in range(1,5):
        deck += [copy(cardbook["Fake Weapon %d"%i])]
      for i in range(1,5):
        deck += [copy(cardbook["Fake Damage Spell %d"%i])]
    
    from decks import Deck
    return Deck(deck)









































