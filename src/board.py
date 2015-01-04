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
  @classmethod
  def set_engine(cls, engine):
      cls.engine = engine

  def __init__(self, player1, player2 ):
      self.players = [player1, player2]
      self.minions = []
      self.everybody = [player1.hero,player2.hero] # heroes + weapons + minion, in order of creation
      self.saved = dict()

  def save_state(self, num=0):
      self.saved[num] = dict(minions=list(self.minions), everybody=list(self.everybody))
      for pl in self.players:
         pl.save_state(num)
      for obj in self.everybody:
        obj.save_state(num)
  def restore_state(self, num=0):
      self.__dict__.update(self.saved[num])
      self.everybody = list(self.everybody)
      self.minions = list(self.minions)
      for pl in self.players:
         pl.restore_state(num)
      for obj in self.everybody:
          obj.restore_state(num)
  def hash_state(self):
      return 0 # todo
  def end_simulation(self):
      self.saved = dict()
      for pl in self.players:
        pl.end_simulation()
      for obj in self.everybody:
         obj.end_simulation()

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
      assert player in self.players, pdb.set_trace()
      p = self.players
      return p[0] if player is not p[0] else p[1]

  def get_enemy_hero(self, player):
      return self.get_enemy_player(player).hero

  def get_friendly_minions(self, player):
      return list(player.minions) # copy it !!

  def get_enemy_minions(self, player):
      # get targetables enemy minions
      return [e for e in self.get_enemy_player(player).minions if not e.has_stealth()]

  def get_minions(self):
      # get targetables characters
      return self.players[0].minions + self.players[1].minions
  
  def get_characters(self):
      # get targetables characters
      return self.everybody[:2] + self.get_minions()

  def get_attackable_characters(self, player):
      enemies = self.get_enemy_minions(player)
      taunts = [e for e in enemies if e.has_taunt()]
      if taunts:
        return taunts
      else:
        return [self.get_enemy_player(player).hero] + enemies

  def list_targets(self, owner, targets):
      if targets==None:
        res = None
      elif targets=='nobody':
        res = []
      elif targets=="owner":
        res = owner
      elif targets.startswith("character"):
        res = self.get_characters()
      elif targets.startswith('friendly minion'):
        res = self.get_friendly_minions(owner)
      elif targets.startswith('friendly beast'):
        res = [b for b in self.get_friendly_minions(owner) if b.card.cat=="beast"]
      elif targets.startswith('minion'):
        res = self.get_minions()
      elif targets=='neighbors':
        res = 'neighbors'
      elif targets.startswith('enemy weapon'):
        res = self.get_enemy_player(owner).weapon
      else:
        assert False, pdb.set_trace() # "error: unknown target '%s'" % targets
      return res

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

  def score_situation(self, player):
    adv = self.get_enemy_player(player)
    if player.hero.dead:
      return -float('inf')
    elif adv.hero.dead:
      return float('inf')
    else:
      return player.score_situation() - adv.score_situation()






