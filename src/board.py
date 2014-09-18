'''
Class for the game board = container with everything (players, heroes, minions...)
 + convenient functions
'''
import pdb
from creatures import Creature


### ------- Board -------------

class Slot (object):
  ''' position on the board (insertion index in players.minions[]) '''
  def __init__(self, owner, pos):
      self.owner = owner
      self.pos = pos
  def __str__(self):
      return "Position %d" % self.pos


class Board:
  ''' Contains everything on the board: the 2 heroes, and their minions
  '''
  def __init__(self, player1, player2 ):
      self.players = [player1, player2]
      self.minions = []
      self.everybody = [player1.hero,player2.hero] # heroes + weapons + minion, in order of creation

  @classmethod
  def set_engine(cls, engine):
      cls.engine = engine

  def add_thing(self, m, pos):
      m.owner.add_thing(m, pos)
      self.everybody.append(m)

  def remove_thing(self, m):
      m.owner.remove_thing(m)
      self.everybody.remove(m)

  def is_game_ended(self):
      for p in self.players:
          if p.hero.dead:
              return True
      return False

  def get_enemy_player(self, player):
      p = self.players
      return p[0] if player is not p[0] else p[1]

  def list_friendly_minions(self, player):
      return player.minions

  def list_enemy_minions(self, player):
      return self.get_enemy_player(player).minions

  def list_characters(self):
      return [v for v in self.everybody if issubclass(type(v),Creature)]

  def list_attackable_characters(self, player):
      enemies = self.list_enemy_minions(player)
      taunts = [e for e in enemies if e.is_taunt()]
      if taunts:
        return taunts
      else:
        return enemies + [self.get_enemy_player(player)]

  def get_free_slots(self, player):
      return [Slot(player,i) for i in range(len(player.minions)+1)]










