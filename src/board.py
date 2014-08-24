
### ------- Board -------------

class Board:
  ''' Contains everything on the board: the 2 heroes, and their minions
  '''
  def __init__(self, hero1, deck1, hero2, deck2 ):
    self.heroes = [hero1, hero2]
    self.decks = [deck1, deck2]
    self.minions = []

  def __hash__(self):
    assert 0
    return 0

  def save_sate(self):
    # return current board + decks + heroes
    return

  def load_sate(self):
    return

  def list_friendly_minions(self, foe):
    return [m for m in self.minions if m.owner==foe]

  def list_enemy_minions(self, foe):
    return [m for m in self.minions if m.owner!=foe]

  def get_enemy_hero(self, foe):
    return self.heroes[0] if foe is not self.heroes[0] else self.heroes[1]

  def list_attackable_characters(self, foe):
    enemies = self.list_enemy_minions(foe)
    taunts = [e for e in enemies if e.is_taunt()]
    if taunts:
      return taunts
    else:
      return enemies + [get_enemy_hero(foe)]

  def list_targetables(self):
    return self.heroes + [m for m in self.minions if m.action_filter!=Eff_Untargetable()]
