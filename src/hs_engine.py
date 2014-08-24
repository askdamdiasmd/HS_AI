import sys, pdb

from board import Board
from messages import *




class HSEngine:
  def __init__(self, hero1, hero2, deck1, deck2 ):
    self.heroes = [hero1, hero2]
    self.board = Board(hero1, deck1, hero2, deck2)

#  def save_sate(self):
#    # return current board + decks + heroes
#    return

#  def load_sate(self):
#    return

  def play_hero_manual(self, num):
    board = self.board
    
    board.start_turn( self.heroes[num] )
    
    board.end_turn( self.heroes[num] )

#  def play_hero_auto(self, num):
#    if 

  def is_game_ended(self):
    return None


if __name__=='__main__':
  
  deck1 = fake_deck()
  hero1 = Mage('jerome')
  
  deck2 = fake_deck()
  hero2 = Mage('matttis')
  
  engine = HSEngine( hero1, deck1, hero2, deck2 )
  
  # initialize global variables
  Minion.set_engine(engine)
  
  t = 0
  while not engine.is_game_ended():
    engine.play_hero_manual(t%2)
    t += 1
  
  print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)

