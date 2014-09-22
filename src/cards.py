'''
all HS cards
'''
import pdb
from minions import *
from actions import *


### ------- Cards -------------

class Card (object):
    def __init__(self, name, cost, cls=None, desc='', name_fr='', desc_fr='', img='' ):
      self.name = name
      self.name_fr = name_fr
      self.cost = cost  # mana cost
      self.cls = cls  # card class = 'priest', ...
      self.desc = desc
      self.desc_fr = desc_fr
      self.img = img
      self.owner = None # specified after assigning deck to hero
      self.action_filters = [] # reacting to actions before selection: [(Act_class, handler),...]
      self.modifiers = [] # reacting to event at emission time, list: [(Msg_Class, event),...]
      self.triggers = [] # reacting to events at execution time, list: [(Msg_Class, event),...]
      self.effects = [] # list of effects without triggers: {'taunt','stealth', or buffs that can be undone...}

    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine

    def list_actions(self):
        assert 0, "must be overloaded"


### --------------- Minion cards ----------------------

class Card_Minion (Card):
    def __init__(self, name, cost, hp, atq, cat=None, cls=None, desc='', name_fr='', img='' ):
        Card.__init__(self, name, cost, cls, desc, name_fr, img )
        self.hp = hp    # health point = life
        self.atq = atq  # attack
        self.cat = cat  # category of minion = 'beast', ...

    def __str__(self):
        return "%s (%d): %d/%d %s" % (self.name_fr, self.cost, self.atq, self.hp, self.desc)

    def list_actions(self):
        return Act_PlayMinionCard(self, self.engine.board.get_free_slots(self.owner))



### ----------------- Spell cards -------------------------

class Card_Spell (Card):
    def __str__(self):
        return "%s (%d): %s" % (self.name_fr, self.cost, self.desc)


class Card_Wrath (Card_Spell):
    """ Druid : Wrath (2 choices) """
    def __init__(self):
        Card_Spell.__init__(self, "Wrath",2,cls="Druid",name_fr="Colere")
    def list_actions(self):
        targets = self.engine.board.list_characters()
        hero = self.owner
        first = Act_SingleSpellDamageCard(self,targets,damage=3)
        actions = lambda self: [Msg_SpellDamage(self.caster,self.choices[0],self.damage),
                                Msg_DrawCard(hero)]
        second = Act_PlayCardSpell(self,targets,damage=1,actions=actions)
        return [first,second]


class Card_FakeSpell (Card_Spell):
    def __init__(self, damage):
        Card_Spell.__init__(self, "Fake Damage Spell %d"%damage,damage-1,
                            name_fr="Faux Sort de dommage %d"%damage,
                            desc="Deal %d points of damage"%damage)
        self.damage = damage
    def list_actions(self):
        targets = self.engine.board.list_characters()
        return Act_SingleSpellDamageCard(self,targets,damage=self.damage)



### -------- Heathstone cardbook collection --------------

def get_cardbook():
  cardbook = []
  cardbook.append( Card_Minion('Wisp',0,1,1,name_fr='Feu follet') )
  cardbook.append( Card_Minion('River Crocolisk',2,3,2,name_fr='Crocilisque des rivieres') )
  cardbook.append( Card_Minion('Chillwind Yeti',4,5,4,name_fr='Yeti Noroit') )

  # add fake creatures
  for i in range(1,11):
    cardbook.append( Card_Minion('Fake Creature %d'%i,i,i,i+1,name_fr="Fausse creature %d"%i) )

  # add fake spells
  for i in range(1,10):
    cardbook.append( Card_FakeSpell(i) )

  # Druid cards
  cardbook.append( Card_Wrath() )

  # transform into a Dictionary
  cardbook = {card.name:card for card in cardbook}
  return cardbook



def fake_deck():
    from copy import copy
    cardbook = get_cardbook()
    deck = []
#    deck += [copy(cardbook["Wisp"]) for i in range(15)]
#    deck += [copy(cardbook["Fake Creature 1"]) for i in range(15)]
    deck += [copy(cardbook["Wisp"]) for i in range(4)]
    deck += [copy(cardbook["River Crocolisk"]) for i in range(4)]
    deck += [copy(cardbook["Chillwind Yeti"]) for i in range(4)]
    for i in range(1,11):
      deck += [copy(cardbook["Fake Creature %d"%i])]
    for i in range(1,9):
      deck += [copy(cardbook["Fake Damage Spell %d"%i])]

    from decks import Deck
    return Deck(deck)







































