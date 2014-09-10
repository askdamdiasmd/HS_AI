"""
IA or manual players
"""
import random
from creatures import *
from messages import *


class Player:
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
    self.hero.owner = self
    for card in self.deck:
      card.owner = self

  def add_thing(self, m, pos=0):
      if issubclass(type(m), Weapon):
        self.weapon = m
      elif issubclass(type(m), Secret):
        self.secrets.append(m)
      else:
        self.minions.insert(pos, m)

  def remove_thing(self, m):
      if m is self.weapon:
        self.weapon = None
      else:
        self.minions.remove(m)

  def add_mana_crystal(self, nb, full=True):
    self.max_mana = min(10,self.max_mana+nb)
    if not empty:
      self.mana = min(10,self.mana+nb)

  def start_turn(self):
    # activated by Msg_StartTurn(player)
    self.engine.send_message([
      Msg_DrawCard(self,1),
      Msg_AddManaCrystal(self,1,full=True),
      Msg_ResetMana(self)],
      immediate = True)

  def list_actions(self):
    # first, hero power
    res = [self.hero.hero_power()]
    
    # then, all card's actions
    for card in self.cards:
      res += card.list_actions()
    
    # then, weapon's attack (if any)
    if self.hero.weapon:
      res.append(self.weapon.list_actions())
    
    # then, all minions actions
    for m in self.minions:
      res += m.list_actions()
    
    return res, self.mana
  
  def mulligan(self, cards):
      assert 0, "must be overloaded"
      return cards, [] # [kept], [discarded]

  def choose_actions(self, actions):
      assert 0, "must be overloaded"


# instanciation of players 

### ----------- Manual (human) player -----------

class HumanPlayer (Player):
  ''' human player : ask the player what to do'''
  def mulligan(self, cards):
      assert 0, "must be overloaded"
      return cards, [] # [kept], [discarded]
     
  def choose_actions(self, actions):
      assert 0, "must be overloaded"


### ------ stupidest player ever ------------

class RandomPlayer (Player):
  ''' random player : just do random things'''
  def mulligan(self, cards):
      # keep everything without changing
      return [] # list of card's index that we mulligan (=replace)

  def choose_actions(self, actions):
      # select one action in the list
      r = random.randint(len(actions))
      action = actions[r]
      # select one target for this action
      s = random.randint(len(action))
      return action.select(s)


### ------ attempter simple IA based on heuristics without forecasting ------------

class SimpleIA (Player):
  ''' todo 
  attempter simple IA based on heuristics without forecasting'''
  pass







