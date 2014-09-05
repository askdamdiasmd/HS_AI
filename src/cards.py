'''
all HS cards
'''
from minions import *
from actions import *


### ------- Cards -------------

class Card:
    def __init__(self, name, cost, cls=None, desc='', name_fr='', img='' ):
        self.name = name
        self.cost = cost  # mana cost
        self.cls = cls  # card class = 'priest', ...
        self.desc = desc
        self.name_fr = name_fr
        self.img = img
        self.owner = None # specified after assignging deck to hero

    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine

    def list_actions(self):
        assert 0, "must be overloaded"



### --------------- Minion cards ----------------------

class Card_Minion (Card):
    def __init__(self, name, cost, hp, att, cat=None, cls=None, desc='', name_fr='', img='' ):
        Card.__init__(self, name, cost, cls, desc, name_fr, img )
        self.hp = hp    # health point = life
        self.att = att  # attack
        self.cat = cat  # category of minion = 'beast', ...

    def list_actions(self):
        hero = self.owner
        return Act_PlayMinionCard(self, self.board.get_free_slots(hero), self)



### ----------------- Spell cards -------------------------

class Card_Spell (Card):
    pass


class Card_Wrath (Card_Spell):
    """ Druid : Wrath (2 choices) """
    def __init__(self):
        Card_Spell.__init__(self, "Wrath",2,cls="Druid",name_fr="Colere")
    def list_actions(self):
        targets = self.board.list_characters()
        hero = self.owner
        first = Act_SingleSpellDamageCard(self,targets,self,damage=3)
        actions = lambda self: [Msg_SpellDamage(self.caster,self.choices[0],self.damage), 
                                Msg_DrawCard(hero)]
        second = Act_PlayCardSpell(self,targets,self,damage=1,actions=actions)
        return [first,second]


class Card_FakeSpell (Card_Spell):
    def __init__(self, damage):
        Card_Spell.__init__(self, "Fake Damage Spell %d"%damage,damage-1,
                            name_fr="Faux Sort de dommage %d"%damage,
                            desc="Deal %d points of damage"%damage)
    def list_actions(self):
        targets = self.board.list_characters()
        return Act_SingleSpellDamageCard(self,targets,self,damage=self.damage)



### -------- Heathstone cardbook collection --------------

def get_cardbook():
  cardbook = []
  cardbook.append( Card_Minion('Wisp',0,1,1,name_fr='Feu follet') )
  cardbook.append( Card_Minion('River Crocolisk',2,3,2,name_fr='Crocilisque des rivieres') )
  cardbook.append( Card_Minion('Chillwind Yeti',4,5,4,name_fr='Yeti Noroit') )
  
  # add fake creatures
  for i in range(1,11):
    cardbook.append( Card_Minion('Fake Creature %d'%i,i,i,i+1) )
    
  # add fake spells
  for i in range(1,10):
    cardbook.append( Card_FakeSpell(i) )
  
  # Druid cards
  cardbook.append( Card_Wrath() )
  
  # transform into a Dictionary
  cardbook = {card.name:card for card in cardbook}
  return cardbook



def fake_deck():
    cardbook = get_cardbook()
    deck = []
    deck += [cardbook["Wisp"]]*4
    deck += [cardbook["River Crocolisk"]]*4
    deck += [cardbook["Chillwind Yeti"]]*4
    for i in range(1,11):
      deck += [cardbook["Fake Creature %d"%i]]
    for i in range(1,9):
      deck += [cardbook["Fake Damage Spell %d"%i]]
    return deck




















