
### ------- Board -------------

class Board:
  ''' Contains everything on the board: the 2 heroes, and their minions
  '''
  def __init__(self, hero1, hero2 ):
    self.hero1 = hero1
    self.hero2 = hero2
    self.minions1 = []
    self.minions2 = [] 

  def __hash__(self):
    assert 0
    return 0

  def list_friendly_minions(self, foe):
    return self.minions1 if foe is self.hero1 else self.minions2

  def list_enemy_minions(self, foe):
    return self.minions1 if foe is not self.hero1 else self.minions2

  def get_friendly_hero(self, foe):
    return self.hero1 if foe is self.hero1 else self.hero2

  def get_enemy_hero(self, foe):
    return self.hero1 if foe is not self.hero1 else self.hero2

  def list_attackable_characters(self, foe):
    enemies = list_enemy_minions(foe)
    taunts = [e for e in enemies if e.is_taunt()]
    if taunts:
      return taunts
    else:
      return enemies + [get_enemy_hero(foe)]
