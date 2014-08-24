"""
IA or manual players
"""
import random


class Player:
  def __init__(self, hero, name):
    self.hero = hero
    self.name = name
  
  def choose_actions(self, actions):
      assert 0, "must be overloaded"

  def mulligan(self, cards):
      assert 0, "must be overloaded"
      return [] # list of card's index that we mulligan (=replace)


# instanciation of players 

### ----------- Manual (human) player -----------

class HumanPlayer (Player):
  ''' human player : ask the player what to do'''
  def mulligan(self, cards):
      assert 0, "must be overloaded"
      return [] # list of card's index that we mulligan (=replace)

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







