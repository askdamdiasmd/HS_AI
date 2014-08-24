import sys, pdb

from board import Board
from messages import *




class HSEngine:
  def __init__(self, hero1, hero2, deck1, deck2 ):
    self.board = Board(hero1, deck1, hero2, deck2)
    self.turn = 0
    self.msg_readers = [] # messages readers

  def exec_message(self, messages ):
    if type(messages)!=list:
      messages = [messages]
    
    while messages:
      msg = messages.pop(0)
      
      # let minions react
      for reader in self.msg_readers:
        m = reader.react( msg )
        if m: messages.append(m)
    
      # then execute the message
      msg.execute()

  def play_hero(self):
    self.turn += 1
    hero = self.board.heroes[self.turn%2]
    self.exec_message( Msg_StartTurn(hero) )
    
    action = None
    while type(action)!=Msg_EndTurn:
      actions = hero.list_actions() 
      action = self.choose_actions(actions)  # action can be Msg_EndTurn
      self.exec_message(action)

  def is_game_ended(self):
    return self.board.is_game_ended()

  def choose_actions(self, actions):
      assert 0, "must be overloaded"



if __name__=='__main__':
  
  deck1 = fake_deck()
  hero1 = Mage('jerome')
  
  deck2 = fake_deck()
  hero2 = Mage('matttis')
  
  engine = HSEngine( hero1, deck1, hero2, deck2 )
  
  # initialize global variables
  Minion.set_engine(engine)
  
  while not engine.is_game_ended():
    engine.play_hero()
  
  t = engine.turn
  print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)

