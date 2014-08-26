import sys, pdb

from board import Board
from messages import *




class HSEngine:
  def __init__(self, player1, player2 ):
    self.players = [player1, player2]
    self.board = Board(player1.hero, player2.hero)
    self.turn = 0
    self.stack = []
    self.executing = False

  def send_message(self, message, priority=False ):
    if type(messages)!=list:
      if priority:
        self.stack.insert(0, message)
      else:
        self.stack.append(message)
    else:
      self.stack += messages
    
    self.exec_messages()

  def exec_messages(self):
    if self.executing:  return # alredy doing it !
    self.executing = True
    
    while messages:
      msg = messages.pop(0)
      assert type(msg)==Message
      
      # let minions react first
      for reader in self.board.listeners:
        msg = reader.react( msg )
    
      # then execute the message
      msg.execute()
    
    self.executing = False

  def play_hero(self):
    self.turn += 1
    player = self.player[self.turn%2]
    hero = player.hero
    self.exec_message( Msg_StartTurn(hero) )
    
    action = None
    while type(action)!=Msg_EndTurn:
      actions = hero.list_actions() 
      action = player.choose_actions(actions)  # action can be Msg_EndTurn
      self.exec_message(action)

  def is_game_ended(self):
    return self.board.is_game_ended()




# test of a human VS random game


if __name__=='__main__':
  
  deck1 = fake_deck()
  hero1 = Mage()
  player1 = HumanPlayer('jerome', hero1)
  player1.set_deck(deck1)
  
  deck2 = fake_deck()
  hero2 = Mage('matttis')
  player2 = RandomPlayer('IA')
  player2.set_deck(deck2)
  
  engine = HSEngine( player1, player2 )
  
  # initialize global variables
  Minion.set_engine(engine)
  
  while not engine.is_game_ended():
    engine.play_hero()
  
  t = engine.turn
  print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)

