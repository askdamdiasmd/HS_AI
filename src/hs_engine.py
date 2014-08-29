import sys, pdb

#from board import Board
#from messages import *



def tree_go_up( level, stack ):
    while not level: 
        level = stack.pop() # go up one level
        level.pop(0)  # remove first one which is an empty list
    return level

def tree_go_down( level, stack=[] ):
    while type(level[0])==list:
      stack.append(level) # remember from where we came
      level = level[0]
    return level


# tree test
#messages = [1,[2,3,[4,5],6],7]
#level, stack = messages, []
#while messages:
#    level = tree_go_up(level, stack)
#    level = tree_go_down( level, stack )
#    msg = level.pop(0)
#    print msg



class HSEngine:
  def __init__(self, player1, player2 ):
    self.players = [player1, player2]
    self.board = Board(player1.hero, player2.hero)
    self.turn = 0
    self.messages = []
    self.executing = False
    self.executed = []  # messages that were executed (for display purposes)

  def send_message(self, messages, immediate=False ):
    if type(messages)!=list:  
      messages = [messages]
    
    deep_level = tree_go_down( self.messages )
    
    if immediate:
      while messages:  
        deep_level.insert(0,messages.pop())
    else:
      deep_level += messages
    
    self.exec_messages()

  def exec_messages(self):
    if self.executing:  return # already doing it !
    self.executing = True
    
    level, stack = self.messages, []
    while self.messages:
      # go up the tree if empty level
      level = tree_go_up(level, stack)
      # go down the tree if new level
      level = tree_go_down( level, stack )
      
      msg = level.pop(0)
      
      # let minions modify first
      for reader in self.board.listeners:
        msg = reader.modify( msg )
      
      # let minions react 
      for reader in self.board.listeners:
        reader.react( msg )
      
      # then execute the message
      msg.execute()
      self.executed.append(msg)
    
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

