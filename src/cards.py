'''
all HS cards
'''


### ------- Cards -------------

class Card:
    def __init__(self, name, cost, cls=None, desc='', name_fr='', img='' ):
      self.name = name
      self.cost = cost  #mana cost
      self.cls = cls  # card class = 'priest', ...
      self.desc = desc
      self.name_fr = name_fr
      self.img = img

    def list_actions(self, owner, available_mana):
      if available_mana >= self.get_cost():
        return self._list_actions_ok() 
      else:
        return None

    def _list_actions(self, owner):
      assert 0, "must be overloaded"

    def get_cost(self):
      return self.cost + self.board.filter_card_cost(self)



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
      return Act_Damage( self.board.list_targetables() )





### -------- Heathstone cardbook collection --------------


cardbook = []

carbook.append( MinionCard('River Crocolisk',2,2,3,'beast',name_fr='Crocilisque des rivieres') )
carbook.append( MinionCard('Chillwind Yeti',4,4,5,name_fr='Yeti Noroit') )

# add fake creatures
for i in range(1,10):
  carbook.append( MinionCard('Fake Creature %d'%i,i,i,i+1) )

# add fake spells
for i in range(1,10):
  carbook.append( DamageSpell('Fake Wrath %d'%i,i,i) )



def fake_deck():
  





















