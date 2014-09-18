"""
IA or manual players
"""
import pdb, random
from creatures import *
#from messages import *
from actions import Act_EndTurn

class Player (object):
  def __init__(self, hero, name, deck):
    self.hero = hero
    self.name = name
    self.deck = deck
    self.cards = None
    self.minions = []
    self.mana = self.max_mana = 0
    self.weapon = None
    self.secrets = []
    
    # init ownership
    self.hero.set_owner(self)
    self.deck.set_owner(self)

  def __str__(self):
      return str(self.hero)

  def add_thing(self, m, pos=0):
      if issubclass(type(m), Weapon):
        self.weapon = m
      elif issubclass(type(m), Secret):
        self.secrets.append(m)
      else:
        self.minions.insert(pos.pos, m)

  def remove_thing(self, m):
      if m is self.weapon:
        self.weapon = None
      else:
        self.minions.remove(m)

  def add_mana_crystal(self, nb):
    self.mana = min(10,self.mana+nb)
    self.max_mana = min(10,self.max_mana+nb)

  def start_turn(self):    # activated by Msg_StartTurn(player)
    self.hero.start_turn()
    for m in self.minions:
      m.start_turn()
    
    self.cards.append(self.deck.draw_one_card())
    self.mana = self.max_mana
    self.add_mana_crystal(1)    

  def end_turn(self):
    self.hero.end_turn()
    for m in self.minions:
      m.end_turn()

  def list_actions(self):
    # first, hero power
    res = [Act_EndTurn(self), self.hero.hero_power()]
    # then, all card's actions
    for card in self.cards:
      actions = card.list_actions()
      res += actions if type(actions)==list else [actions]
    # then, weapon's attack (if any)
    if self.weapon:
      res.append(self.weapon.list_actions())
    # then, all minions actions
    for m in self.minions:
      act = m.list_actions()
      if act: res += act if type(act)==list else [act]
    return res
  
  def burn_card(self, card, cost):
      self.cards.remove(card)
      self.mana -= cost
      assert self.mana>=0

  def mulligan(self, cards):
      assert 0, "must be overloaded"
      return discarded

  def draw_init_cards(self, nb):
      self.cards = self.deck.draw_init_cards(nb,self.mulligan)

  def choose_actions(self, actions):
      assert 0, "must be overloaded"


# instanciation of players 

### ----------- Manual (human) player -----------

def get_choice(range):
  while True:
    try:
      n = int(raw_input())
      if n in range:  
        return n
    except ValueError:
      pass

class HumanPlayer (Player):
  ''' human player : ask the player what to do'''
  def mulligan(self, cards):
      assert 0, "to be implemented by user interface"
     
  def choose_actions(self, actions):
      assert 0, "to be implemented by user interface"


### ------ stupidest player ever ------------

class RandomPlayer (Player):
  ''' random player : just do random things'''
  def mulligan(self, cards):
      # keep everything without changing
      return []

  def choose_actions(self, actions):
      # select one action in the list
      r = random.randint(0,len(actions)-1)
      action = actions[r]
      # select one target for this action
      choices = [ch[random.randint(0,len(ch)-1)] for ch in action.choices]
      return action.select(choices)


### ------ attempter simple IA based on heuristics without forecasting ------------

class SimpleIA (Player):
  ''' todo 
  attempter simple IA based on heuristics without forecasting'''
  pass







