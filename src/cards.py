'''
all HS cards
'''


### ------- Cards -------------

class Card:
  def __init__(self, name, cost, cls=None, desc='', name_fr='', img='' ):
    self.name = name
    self.cost = cost  #mana cost
    self.cls = cls  # card class
    self.desc = desc
    self.name_fr = name_fr
    self.img = img

  def list_actions():
    assert 0, "must be overloaded"



class MinionCard (Card):
  def __init__(self, name, cost, hp, att, cls=None, desc='', name_fr='', img='' ):
    Card.__init__(self, name, cost, cls, desc, name_fr, img )
    self.hp = hp    # health point = life
    self.att = att  # attack

  def create_minion(self):
    return Minion(self)

  def list_actions(self, available_mana):
    if available_mana >= self.cost:
      return [Act_Minion(self.create_minion, pos) for pos in self.board.get_free_slots()]
    else:
      return []



class SpellCard:
  def __init__(self, name, mana, cls=None, desc='', name_fr='', img='' ):
    

class Spell_Damage:




### -------- Heathstone cardbook collection --------------


cardbook = []

carbook.append( MinionCard('River Crocolisk',2,2,3,name_fr='Crocilisque des rivieres') )
carbook.append( MinionCard('Chillwind Yeti',4,4,5,name_fr='Yeti Noroit') )

# add fake creatures
for i in range(1,10):
  carbook.append( MinionCard('Fake Creature %d'%i,i,i,i+1) )

# add fake spells
for i in range(1,10):
  carbook.append( DamageSpell('Fake Wrath %d'%i,i,i) )



def fake_deck():
  
