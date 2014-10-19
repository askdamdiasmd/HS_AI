'''
Class for the game board = container with everything (players, heroes, minions...)
 + convenient functions
'''
import pdb
from creatures import Creature, Minion


### ------- Board -------------

class Slot (object):
  ''' position on the board (insertion index in players.minions[]) '''
  def __init__(self, owner, index=None, fpos=None):
      self.owner = owner
      self.index = index  # just index in [0,..6]
      self.fpos = fpos   # floatting-point index
  @classmethod
  def set_engine(cls, engine):
    cls.engine = engine  
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

  def add_thing(self, m, pos=None):
      if m.owner.add_thing(m, pos): 
        self.everybody.append(m)

  def remove_thing(self, m=None):
      try:
        self.everybody.remove(m)
        return m.owner.remove_thing(m)
      except ValueError:
        pass  # sometimes, it has been already removed

  def is_game_ended(self):
      for p in self.players:
          if p.hero.dead:
              return True
      return False

  def get_enemy_player(self, player):
      p = self.players
      return p[0] if player is not p[0] else p[1]

  def get_enemy_hero(self, player):
      return self.get_enemy_player(player).hero

  def get_friendly_minions(self, player):
      return player.minions

  def get_enemy_minions(self, player):
      # get targetables enemy minions
      return [e for e in self.get_enemy_player(player).minions if not e.has_stealth()]

  def get_minions(self, player):
      # get targetables characters
      return player.minions + self.get_enemy_player(player).minions
  
  def get_characters(self, player):
      # get targetables characters
      return self.everybody[:2] + self.get_minions(player)

  def get_attackable_characters(self, player):
      enemies = self.get_enemy_minions(player)
      taunts = [e for e in enemies if e.has_taunt()]
      if taunts:
        return taunts
      else:
        return [self.get_enemy_player(player).hero] + enemies

  def get_free_slots(self, player):
      if len(player.minions)<7:
        mp = player.minions_pos
        return [Slot(player,i,(a+b)/2) for i,(a,b) in enumerate(zip(mp[:-1],mp[1:]))]
      else:
        return []

  def get_minion_pos(self, m):
      if issubclass(type(m),Minion):
        index = m.owner.minions.index(m)
        rel_index = m.owner.minions_pos[index+1]
        return Slot(m.owner, index, rel_index)
      else:
        return None







