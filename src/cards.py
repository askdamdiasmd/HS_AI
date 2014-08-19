'''
all HS cards
'''


### ------- Cards -------------

class BasicCard:
  def __init__(self, name, mana, hp, att, hero=None, desc='', name_fr='', img='' ):
    self.name = name
    self.mana = mana
    self.att = att
    self.hp = hp
    self.hero = hero
    self.desc = desc
    self.name_fr = name_fr
    self.img = img



