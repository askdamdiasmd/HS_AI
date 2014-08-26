from effects import *

### ------- Board -------------

class Board:
  ''' Contains everything on the board: the 2 heroes, and their minions
  '''
  def __init__(self, hero1, hero2 ):
    self.heroes = [hero1, hero2]
    self.minions = []
    self.listeners = [] # heroes + weapons + minion

  def __hash__(self):
    assert 0, "hash(minions + heroes + weapons (everything but cards))"
    return 0

  def save_sate(self):
    # return current board + decks + heroes
    return

  def load_sate(self):
    return

  def is_game_ended(self):
    return any([h for h in self.heroes if h.hp<=0])

  def list_friendly_minions(self, hero):
    return [m for m in self.minions if m.owner==hero]

  def list_enemy_minions(self, hero):
    return [m for m in self.minions if m.owner!=hero]

  def get_friendly_hero(self, hero):
    return self.heroes[0] if hero is self.heroes[0] else self.heroes[1]

  def get_enemy_hero(self, hero):
    return self.heroes[0] if hero is not self.heroes[0] else self.heroes[1]

  def list_characters(self):
    return self.heroes + self.minions

  def list_attackable_characters(self, hero):
    enemies = self.list_enemy_minions(hero)
    taunts = [e for e in enemies if e.is_taunt()]
    if taunts:
      return taunts
    else:
      return enemies + [self.get_enemy_hero(hero)]




