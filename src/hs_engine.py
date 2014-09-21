import pdb
from creatures import Thing
from messages import Message, Msg_StartTurn
from actions import Action, Act_EndTurn
from effects import Effect
from cards import Card
from players import Player

def tree_go_up( level, stack ):
    while not level:
        level = stack.pop() # go up one level
        level.pop(0)  # remove first one which is an empty list
    return level

def tree_go_down( level, stack=[] ):
    while level and type(level[0])==list:
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
    Thing.set_engine(self)
    Action.set_engine(self)
    Effect.set_engine(self)
    Card.set_engine(self)
    Message.set_engine(self)
    Player.set_engine(self)

  def send_message(self, messages ):
    if type(messages)!=list:
      messages = [messages]

    # let minions modify first
    for i,msg in enumerate(messages):
      for reader in self.board.everybody:
        messages[i] = reader.modify_msg( msg )

    deep_level = tree_go_down( self.messages )
    deep_level += messages

    self.exec_messages()

  def send_status(self, messages):
    """ status messages are only usefull for the UI """
    if type(messages)!=list:
      messages = [messages]
    self.executed += messages

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

      # let minions react
      immediate = []
      for reader in self.board.everybody:
        res = reader.react_msg( msg )
        if res: immediate.append(res)

      # then execute the message
      #print "[%s] %s" %(type(msg).__name__,msg)
      self.executed.append(msg)
      res = msg.execute()

      if res: immediate.append(res)
      while immediate: # add immediate-effect messages
        level.insert(0,immediate.pop())

    self.executing = False

  def filter_actions(self, actions):
    for i,a in enumerate(actions):
      for o in self.board.everybody:
        a = o.filter_action(a)
      actions[i] = a
    return actions

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
    while not self.is_game_ended() and type(action)!=Act_EndTurn:
      actions = player.list_actions()
      # filter actions
      actions = self.filter_actions(actions)
      actions = [a for a in actions if a.cost<=player.mana]
      action = player.choose_actions(actions)  # action can be Msg_EndTurn
      self.send_message(action)

    self.turn += 1

  def is_game_ended(self):
    return self.board.is_game_ended()




