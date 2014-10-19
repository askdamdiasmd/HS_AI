"""
IA or manual players
"""
import pdb, random
from numpy import searchsorted
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
    self.minions_pos = [0.0, 1000.0]
    self.mana = self.max_mana = 0
    self.weapon = None
    self.secrets = []
    self.owner = self # for conveniency

    # init ownership
    self.hero.set_owner(self)
    self.deck.set_owner(self)

  @classmethod
  def set_engine(cls, engine):
    cls.engine = engine

  def __str__(self):
      return str(self.hero)

  def add_thing(self, thing, pos=None):
      if issubclass(type(thing), Hero):
        assert 0, "todo"
      elif issubclass(type(thing), Weapon):
        self.weapon = thing
        self.engine.send_message( Msg_WeaponPopup(thing), immediate=True)
      elif issubclass(type(thing), Secret):
        self.secrets.append(thing)
        self.engine.send_message( Msg_SecretPopup(thing), immediate=True)
      elif issubclass(type(thing), Minion) and len(self.minions)<7:
        mp = self.minions_pos
        if pos.fpos=="right": # helper
          pos.fpos = (mp[-2]+mp[-1])/2
        i = searchsorted(mp,pos.fpos,side='right')
        if mp[i-1]==pos.fpos: # already exist, so create new number
          pos.fpos = (pos.fpos+mp[i])/2
        mp.insert(i,pos.fpos)
        self.minions.insert(i-1, thing)
        self.engine.send_message(Msg_MinionPopup(thing,i-1), immediate=True)
      else:
        return False
      return True

  def remove_thing(self, thing=None):
      if thing is self.weapon:
        self.weapon = None
      elif issubclass(type(thing), Secret):
        self.secrets.remove(thing)
      elif issubclass(type(thing), Minion):
        i = self.minions.index(thing)
        self.minions_pos.pop(i+1)
        self.minions.pop(i)
      elif issubclass(type(thing), Hero):
        pass #print "Hero %s died !"

  def add_mana_crystal(self, nb, useit=False):
    self.mana = min(10,self.mana+nb)
    self.max_mana = min(10,self.max_mana+nb)
    if useit: self.use_mana(nb)

  def use_mana(self, nb):
    self.mana -= nb
    assert self.mana>=0

  def gain_mana(self, nb):
    self.mana = min(10,self.mana + nb)

  def start_turn(self):    # activated by Msg_StartTurn(player)
    self.hero.start_turn()
    for m in self.minions:
      m.start_turn()
    if self.weapon:
      self.weapon.start_turn()
    self.mana = self.max_mana
    self.add_mana_crystal(1)
    self.engine.send_message(Msg_DrawCard(self))

  def end_turn(self):
    self.hero.end_turn()
    for m in self.minions:
      m.end_turn()

  def list_actions(self):
    res = [Act_EndTurn(self)]
    # first, hero power
    res += self.hero.list_actions()
    # then, all card's actions
    for card in self.cards:
      actions = card.list_actions()
      res += actions if type(actions)==list else [actions]
    # then, weapon/hero's attack (if any)
    if self.weapon:
      res += self.weapon.list_actions()
    # then, all minions actions
    for m in self.minions:
      res += m.list_actions()
    return res

  def draw_card(self):
    card = self.deck.draw_one_card()
    if card:  
      if len(self.cards)<10:
        self.cards.append(card)
        self.engine.send_message(Msg_CardDrawn(self,card),immediate=True)
      else:
        self.engine.send_message(Msg_DrawBurnCard(self,card),immediate=True)
  
  def give_card(self, card, origin):
      self.cards.append(card)
      self.engine.send_message(Msg_CardDrawn(self,card,origin=origin),immediate=True)

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
        self.engine.send_message(Msg_CardDrawn(self,c),immediate=True)

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
      choices = []
      for ch in action.choices:
        if ch:
          choices.append(ch[random.randint(0,len(ch)-1)])
        else:
          choices.append(None)
      return action.select(choices)


### ------ attempter simple IA based on heuristics without forecasting ------------

class SimpleIA (Player):
  ''' todo
  attempter simple IA based on heuristics without forecasting'''
  pass







