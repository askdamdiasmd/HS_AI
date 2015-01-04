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
      self.saved = dict()

    def save_state(self, num=0):
      self.saved[num] = dict(fatigue=self.fatigue, cards=list(self.cards))
    def restore_state(self, num=0):
      self.__dict__.update(self.saved[num])
      self.cards = list(self.cards)
    def end_simulation(self):
      self.saved = dict()

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

def fake_deck(cardbook, debug=False, fake_cards=None):
    from copy import deepcopy
    deck = []
    if debug:
      if type(fake_cards)!=list:  fake_cards = [fake_cards]
      assert fake_cards
      for c in fake_cards:
        deck += [deepcopy(cardbook[c]) for i in range(30/len(fake_cards))]      
    else:
      cards = list(set([c for c in cardbook.values() if c.collectible]))
      deck = [deepcopy(cards[random.randint(0,len(cards)-1)])  for i in range(30)]
    
    return Deck(deck)



### ------- Automated Learning to play a deck --------


class ConstructedDeck (Deck):
    ''' This class is for user-constructed, optimized, decks.
        It is able to learn automatically how to play it.
    '''
    def __init__(self, cards):
      Deck.__init__(self, cards)
    
    @classmethod
    def load(cls, f):
      cards = cPickle.load(f)
      return ConstructedDeck( cards )

    def save(self, f):
      cPickle.dump(self.cards,f)
      
    def learn_to_play(self):
      """ this function computes the real costs of cards,
      based on potential combos present in the deck.
      Idea: try many random mid-game situations 10 mana, 
            and see what can happen when we play several cards together
            This way, we realize that some cards are better played consecutively than alone.
      """
      assert 0



































