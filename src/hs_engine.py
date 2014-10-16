import pdb
from creatures import Thing
from messages import Message, Msg_StartTurn
from actions import Action, Act_EndTurn, Act_PlayMinionCard
from effects import Effect
from cards import Card
from decks import Deck
from players import Player

def tree_go_up( level, stack ):
    while not level: # empty level
        if not stack: return []
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
    from board import Board, Slot
    self.players = [player1, player2]
    self.board = Board(player1, player2)
    self.turn = 0
    self.messages = []
    self.immediate = []
    self.executing = False

    # init global variables : everyone can access board or send messages
    Board.set_engine(self)
    Slot.set_engine(self)
    Thing.set_engine(self)
    Action.set_engine(self)
    Effect.set_engine(self)
    Card.set_engine(self)
    Deck.set_engine(self)
    Message.set_engine(self)
    Player.set_engine(self)

  def send_message(self, messages, immediate=False ):
    if type(messages)!=list:
      messages = [messages]
    
    # let minions modify first
    for i,msg in enumerate(messages):
      for reader in self.board.everybody:
        messages[i] = reader.modify_msg( msg )

    if immediate:
      self.immediate += messages
    else:
      deep_level = tree_go_down( self.messages )
      deep_level += messages

    self.exec_messages()

  def display_msg(self, msg):
    """ default implementation """
    print "[%s] %s" %(type(msg).__name__,msg)

  def wait_for_display(self):
    pass

  def exec_messages(self):
    if self.executing:  return # already doing it !
    self.executing = True
    
    level, stack = self.messages, []
    while self.messages:
      # go up the tree if empty level
      level = tree_go_up(level, stack)
      # go down the tree if new level
      level = tree_go_down( level, stack )

      assert level, pdb.set_trace()
      msg = level.pop(0)

      # let minions react
      for reader in self.board.everybody:
        res = reader.react_msg( msg )
        assert not res, pdb.set_trace()

      # then execute the message
      self.display_msg(msg)
      res = msg.execute()
      assert not res, pdb.set_trace()
      
      while self.immediate: # add immediate-effect messages
        level.insert(0,self.immediate.pop())

    self.wait_for_display()
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
    self.players[1].draw_init_cards(4, coin=True)

  def play_turn(self):
    player = self.get_current_player()
    self.send_message( Msg_StartTurn(player) )
    
    action = None
    while not self.is_game_ended() and type(action)!=Act_EndTurn:
      actions = player.list_actions()
      # filter actions
      actions = self.filter_actions(actions)
      actions = [a for a in actions if a.is_valid() and a.cost<=player.mana]
      action = player.choose_actions(actions)  # action can be Msg_EndTurn
      self.send_message(action)

    self.turn += 1

  def is_game_ended(self):
    return self.board.is_game_ended()

  def get_winner(self):
    p1, p2 = self.players
    if p1.hero.hp<=0 and p2.hero.hp>0:
      return p2
    if p1.hero.hp>0 and p2.hero.hp<=0:
      return p1
    return None
    


