"""
IA or manual players
"""
import pdb, random
from creatures import *
from heroes import Hero
#from messages import *
from actions import Act_EndTurn

class Player (object):
  def __init__(self, hero, name, deck):
    self.hero = hero
    self.name = name
    self.deck = deck
    self.cards = []
    self.minions = []
    self.mana = self.max_mana = 0
    self.weapon = None
    self.secrets = []
    self.n_remaining_power = 0

    # init ownership
    self.hero.set_owner(self)
    self.deck.set_owner(self)

  @classmethod
  def set_engine(cls, engine):
    cls.engine = engine

  def __str__(self):
      return str(self.hero)

  def add_thing(self, m, pos=0):
      if issubclass(type(m), Weapon):
        self.weapon = m
      elif issubclass(type(m), Secret):
        self.secrets.append(m)
      else:
        self.minions.insert(pos.pos, m)

  def remove_thing(self, m):
      if m is self.weapon:
        self.weapon = None
      elif issubclass(type(m), Secret):
        self.secrets.remove(m)
      elif issubclass(type(m), Minion):
        self.minions.remove(m)
      elif issubclass(type(m), Hero):
        pass #print "Hero %s died !"

  def add_mana_crystal(self, nb, useit=False):
    self.mana = min(10,self.mana+nb)
    self.max_mana = min(10,self.max_mana+nb)
    if useit: self.use_mana(nb)

  def use_mana(self, nb):
    self.mana -= nb
    assert self.mana>=0

  def gain_mana(self, nb):
    self.mana += nb

  def use_hero_power(self):
    self.n_remaining_power -= 1
    assert self.n_remaining_power>=0

  def start_turn(self):    # activated by Msg_StartTurn(player)
    self.n_remaining_power = 1
    self.hero.start_turn()
    for m in self.minions:
      m.start_turn()
    self.mana = self.max_mana
    self.add_mana_crystal(1)
    self.draw_card()

  def end_turn(self):
    self.hero.end_turn()
    for m in self.minions:
      m.end_turn()

  def list_actions(self):
    res = [Act_EndTurn(self)]
    # first, hero power
    if self.n_remaining_power:
      res.append(self.hero.hero_power())
    # then, all card's actions
    for card in self.cards:
      actions = card.list_actions()
      res += actions if type(actions)==list else [actions]
    # then, weapon's attack (if any)
    if self.weapon:
      res.append(self.weapon.list_actions())
    # then, all minions actions
    for m in self.minions:
      act = m.list_actions()
      if act: res += act if type(act)==list else [act]
    return res

  def draw_card(self):
    card = self.deck.draw_one_card()
    if card:  
      if len(self.cards)<10:
        self.cards.append(card)
        self.engine.display_msg(Msg_DrawCard(self,card))
      else:
        self.engine.display_msg(Msg_DrawBurnCard(self,card))

  def throw_card(self, card):
#      if type(card)==int:
#        for i in range(card):
#          self.cards.pop(random.randint(0,len(self.cards)-1))
#      else:
      self.cards.remove(card)

  def mulligan(self, cards):
      assert 0, "must be overloaded"
      return discarded

  def draw_init_cards(self, nb, coin=False):
      from cards import Card_Coin
      self.cards = self.deck.draw_init_cards(nb,self.mulligan)
      if coin:  
        self.cards.append(Card_Coin(self))
      for c in self.cards:
        self.engine.display_msg(Msg_DrawCard(self,c))

  def choose_actions(self, actions):
      assert 0, "must be overloaded"


# instanciation of players

### ----------- Manual (human) player -----------

def get_choice(range):
  while True:
    try:
      n = int(raw_input())
      if n in range:
        return n
    except ValueError:
      pass

class HumanPlayer (Player):
  ''' human player : ask the player what to do'''
  def mulligan(self, cards):
      assert 0, "to be implemented by user interface"

  def choose_actions(self, actions):
      assert 0, "to be implemented by user interface"


### ------ stupidest player ever ------------

class RandomPlayer (Player):
  ''' random player : just do random things'''
  def mulligan(self, cards):
      # keep everything without changing
      return []

  def choose_actions(self, actions):
      # select one action in the list
      r = random.randint(0,len(actions)-1)
      action = actions[r]
      # select one target for this action
      choices = [ch[random.randint(0,len(ch)-1)] for ch in action.choices]
      return action.select(choices)


### ------ attempter simple IA based on heuristics without forecasting ------------

class SimpleIA (Player):
  ''' todo
  attempter simple IA based on heuristics without forecasting'''
  pass







