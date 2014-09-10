
from effects import *

### ------- Board -------------

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
      m.owner.add_minion(m, pos)
      self.everybody.append(m)

  def remove_thing(self, m):
      m.owner.remove_thing(m)
      self.everybody.remove(m)

  def is_game_ended(self):
      return any([h for h in heroes if h.hp<=0])

  def list_friendly_minions(self, hero):
      return [m for m in self.minions if m.owner==hero]

  def list_enemy_minions(self, hero):
      return [m for m in self.minions if m.owner!=hero]

  def get_friendly_hero(self, hero):
      heroes = self.everybody[:2]
      return heroes[0] if hero is heroes[0] else heroes[1]

  def get_enemy_hero(self, hero):
      heroes = self.everybody[:2]
      return heroes[0] if hero is not heroes[0] else heroes[1]

  def list_characters(self):
      return [v for v in self.everybody if issubclass(type(v),Creature)]

  def list_attackable_characters(self, hero):
      enemies = self.list_enemy_minions(hero)
      taunts = [e for e in enemies if e.is_taunt()]
      if taunts:
        return taunts
      else:
        return enemies + [self.get_enemy_hero(hero)]
    










