import sys, pdb

from board import Board
from messages import *




class HSEngine:
  def __init__(self, hero1, hero2, deck1, deck2 ):
    self.hero1 = hero1
    self.hero2 = hero2
    self.board = Board(hero1, hero2)
    
    self.deck1 = deck1
    self.deck2 = deck2

  def save_sate(self):
    # return current board + decks + heroes
    return

  def load_sate(self):
    return

  def ee():
    




if __name__=='__main__':
  
  deck1 = fake_deck()
  hero1 = Mage('jerome')
  
  deck2 = fake_deck()
  hero2 = Mage('matttis')
  
  
