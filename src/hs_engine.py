import sys, pdb
from messages import Message, Msg_StartTurn, Msg_EndTurn

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



class HSEngine:
  def __init__(self, player1, player2 ):
    from board import Board
    self.players = [player1, player2]
    self.board = Board(player1, player2)
    self.turn = 0
    self.messages = []
    self.executing = False
    self.executed = []  # messages that were executed (for display purposes)
    
    # init global variables : everyone can access board or send messages
    Board.set_engine(self)
    from creatures import Thing
    Thing.set_engine(self)
    from actions import Action
    Action.set_engine(self)
    from effects import Effect
    Effect.set_engine(self)
    from cards import Card
    Card.set_engine(self)
    Message.set_engine(self)

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

  def get_current_player(self):
    return self.players[self.turn%2]

  def get_other_player(self):
    return self.players[(self.turn+1)%2]

  def start_game(self):
    self.players[0].draw_init_cards(3)
    self.players[1].draw_init_cards(4)

  def play_turn(self):
    player = self.get_current_player()
    self.send_message( Msg_StartTurn(player) )
    
    action = None
    while type(action)!=Msg_EndTurn:
      actions = player.list_actions() 
      action = player.choose_actions(actions)  # action can be Msg_EndTurn
      self.exec_message(action)
    
    self.turn += 1

  def is_game_ended(self):
    return self.board.is_game_ended()




