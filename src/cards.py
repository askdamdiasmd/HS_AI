'''
all HS cards
'''
from minions import *
from actions import *

### ------- Cards -------------

class Card:
    def __init__(self, name, cost, cls=None, desc='', name_fr='', img='' ):
      self.name = name
      self.cost = cost  #mana cost
      self.cls = cls  # card class = 'priest', ...
      self.desc = desc
      self.name_fr = name_fr
      self.img = img

    def get_cost(self):
      return self.cost + self.board.filter_card_cost(self)
    
    def list_actions(self, owner, available_mana):
      if available_mana >= self.get_cost():
        return self._list_actions_ok() 
      else:
        return None
    
    def _list_actions(self, owner):
      assert 0, "must be overloaded"



class MinionCard (Card):
    def __init__(self, name, cost, hp, att, cat=None, cls=None, desc='', name_fr='', img='' ):
      Card.__init__(self, name, cost, cls, desc, name_fr, img )
      self.hp = hp    # health point = life
      self.att = att  # attack
      self.cat = cat  # category of minion = 'beast', ...

    def instanciate_minion(self):
      return Minion(self)

    def _list_actions(self, owner):
      return Act_Minion(owner, self.instanciate_minion, self.board.get_free_slots(owner))


class SpellCard (Card):
    def __init__(self, name, mana, cls=None, desc='', name_fr='', img='' ):
      Card.__init__(self, name, cost, cls, desc, name_fr, img )


class Spell_Damage (SpellCard):
    def __init__(self, name, cost, cls=None, desc='', name_fr='', img='' ):
      SpellCard.__init__(self, name, cost, cls, desc, name_fr, img )

    def _list_actions(self, owner):
      return Act_Damage( self.board.list_characters() )


class SpellCard:
  def __init__(self, name, mana, cls=None, desc='', name_fr='', img='' ):
    pass

class Spell_Damage:
  pass



### -------- Heathstone cardbook collection --------------

def get_cardbook():
  cardbook = []
  cardbook.append( MinionCard('Wisp',0,1,1,name_fr='Feu follet') )
  cardbook.append( MinionCard('River Crocolisk',2,3,2,name_fr='Crocilisque des rivieres') )
  cardbook.append( MinionCard('Chillwind Yeti',4,5,4,name_fr='Yeti Noroit') )

# add fake creatures
  for i in range(1,10):
    cardbook.append( MinionCard('Fake Creature %d'%i,i,i,i+1) )
  
# add fake spells
  """ for i in range(1,10):
      cardbook.append( DamageSpell('Fake Wrath %d'%i,i,i) )"""

  return cardbook

def fake_deck():
  pass





















