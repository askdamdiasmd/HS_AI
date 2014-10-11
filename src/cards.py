'''
all HS cards
'''
import pdb
from minions import *
from actions import *
from effects import *

eff_trad_fr = dict(taunt='Provocation')


### ------- Cards -------------

class Card (object):
    def __init__(self, name, cost, cls=None, desc='', name_fr='', desc_fr='', effects=None, img='' ):
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
      if type(effects)==str:  effects = effects.split()
      self.effects = effects or [] # list of effects without triggers: {'taunt','stealth', or buffs that can be undone...}
      if desc=='':  
        self.desc = '. '.join(['%s%s'%(e[0].upper(),e[1:]) for e in self.effects])
      if desc_fr=='':  
        self.desc_fr = '. '.join([eff_trad_fr[e] for e in self.effects])

    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine

    def list_actions(self):
        assert 0, "must be overloaded"


### --------------- Minion cards ----------------------

class Card_Minion (Card):
    def __init__(self, name, cost, atq, hp, cat=None, **kwargs ):
        Card.__init__(self, name, cost, **kwargs )
        self.hp = hp    # health point = life
        self.atq = atq  # attack
        self.cat = cat  # category of minion = 'beast', ...

    def __str__(self):
        return "%s (%d): %d/%d %s" % (self.name_fr, self.cost, self.atq, self.hp, self.desc)

    def list_actions(self):
        return Act_PlayMinionCard(self)


class Card_HarvestGolem (Card_Minion):
    def __init__(self):
        Card_Minion.__init__(self,"Harvest Golem", 3, 2, 3,
                             name_fr="Golem des moissons",
                             desc_fr="Rale d'agonie: Invoque 2 hyenes 2/2")
        Eff_InvokeCard.create_death_rattle(self, 
                             Card_Minion("Damaged Golem", 1, 2, 1, 
                             name_fr="Golem endommage", cat='beast'))
        

class Card_SavannahHighmane (Card_Minion):
    def __init__(self):
        Card_Minion.__init__(self,"Savannah Highmane", 6, 6, 5, 
                             name_fr="Grande Criniere des Savanes", cat='beast',
                             desc_fr="Rale d'agonie: Invoque 2 hyenes 2/2")
        Eff_InvokeCard.create_death_rattle(self, 
                             Card_Minion("Hyena", 2, 2, 2, 
                             name_fr="Hyene", cat='beast'))
        Eff_InvokeCard.create_death_rattle(self, 
                             Card_Minion("Hyena", 2, 2, 2, 
                             name_fr="Hyene", cat='beast'))


class Card_ShatteredSunCleric (Card_Minion):
    def __init__(self):
        Card_Minion.__init__(self,"Shattered Sun Cleric", 3, 3, 2, 
                             name_fr="Clerc du Soleil brise",
                             desc_fr="Cri de guerre: confere +1/+1 a un serviteur allie")
    def list_actions(self):
        return Act_PlayMinionAndEffect(self, Eff_BuffMinion(1,1,False),
                                       self.engine.board.get_friendly_minions(self.owner))


class Card_IronbeakOwl (Card_Minion):
    def __init__(self):
        Card_Minion.__init__(self,"Ironbeak Owl", 2, 2, 1, 
                             name_fr="Chouette bec-de-fer",
                             desc_fr="Cri de guerre: reduit au silence un autre serviteur")
    def list_actions(self):
        return Act_PlayMinionAndEffect(self, Eff_Silence(), self.engine.board.get_minions())


class Card_AbusiveSergeant (Card_Minion):
    def __init__(self):
        Card_Minion.__init__(self,"Abusive Sergeant", 1, 2, 1, 
                             name_fr="Sergent Grossier",
                             desc_fr="Cri de guerre: confere +2 ATQ a un serviteur pendant ce tour")
    def list_actions(self):
        return Act_PlayMinionAndEffect(self, Eff_BuffMinion(2,0,True), self.engine.board.get_minions())

class Card_DireWolfAlpha (Card_Minion):
    def __init__(self):
        Card_Minion.__init__(self,"Dire Wolf Alpha", 2, 2, 2, 
                             name_fr="Loup alpha redoutable",
                             desc_fr="Les serviteurs adjacents ont +1 ATQ")
        Eff_BuffLeftRight.create(self,1,0)
    def list_actions(self):
        return Act_PlayMinionCard(self)


### --------------- Weapon cards ----------------------

class Card_Weapon (Card):
    def __init__(self, name, cost, atq, hp, cat=None, **kwargs ):
        Card.__init__(self, name, cost, **kwargs )
        self.hp = hp    # health point = weapon durability
        self.atq = atq  # attack
        self.cat = cat  # category of minion = 'beast', ...

    def __str__(self):
        return "Weapon %s (%d): %d/%d %s" % (self.name_fr, self.cost, self.atq, self.hp, self.desc)

    def list_actions(self):
        return Act_PlayWeaponCard(self)




### ----------------- Spell cards -------------------------

class Card_Spell (Card):
    def __str__(self):
        return "%s (%d): %s" % (self.name_fr, self.cost, self.desc)


class Card_Coin (Card_Spell):
    def __init__(self, owner):
        Card_Spell.__init__(self,"The coin",0,desc="Gain one mana crystal this turn only")
        self.owner = owner
    def list_actions(self):
        player = self.owner
        actions = lambda self: [Msg_GainMana(self.caster,1)]
        return Act_PlaySpellCard(self,None,actions)


class Card_Wrath (Card_Spell):
    """ Druid : Wrath (2 choices) """
    def __init__(self):
        Card_Spell.__init__(self, "Wrath",2,cls="Druid",name_fr="Colere")
    def list_actions(self):
        targets = self.engine.board.get_characters()
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
        targets = self.engine.board.get_characters()
        return Act_SingleSpellDamageCard(self,targets,damage=self.damage)



### -------- Heathstone cardbook collection --------------

def get_cardbook():
  cardbook = []
  cardbook.append( Card_Minion('Wisp',0,1,1,name_fr='Feu follet') )
  cardbook.append( Card_Minion('Goldshire Footman',1,1,2,name_fr='Soldat de Comte de l\'Or',effects='taunt') )
  cardbook.append( Card_Minion('River Crocolisk',2,2,3,name_fr='Crocilisque des rivieres') )
  cardbook.append( Card_HarvestGolem() )
  cardbook.append( Card_SavannahHighmane() )
  cardbook.append( Card_ShatteredSunCleric() )
  cardbook.append( Card_IronbeakOwl() )
  cardbook.append( Card_AbusiveSergeant() )
  cardbook.append( Card_DireWolfAlpha() )
  cardbook.append( Card_Minion('Chillwind Yeti',4,4,5,name_fr='Yeti Noroit') )

  # add fake creatures
  for i in range(1,11):
    cardbook.append( Card_Minion('Fake Creature %d'%i,i,i+1,i,name_fr="Fausse creature %d"%i) )
    
  # add fake weapons
  for i in range(1,5):
    cardbook.append( Card_Weapon('Fake Weapon %d'%i,i,i,2) )
  
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
    if 1:
      deck += [copy(cardbook["Abusive Sergeant"]) for i in range(10)]
      deck += [copy(cardbook["Dire Wolf Alpha"]) for i in range(10)]
      deck += [copy(cardbook["Ironbeak Owl"]) for i in range(10)]
#      deck += [copy(cardbook["Savannah Highmane"]) for i in range(6)]
#      deck += [copy(cardbook["Goldshire Footman"]) for i in range(6)]
##      deck += [copy(cardbook["Wisp"]) for i in range(15)]
#      deck += [copy(cardbook["Fake Weapon 1"]) for i in range(6)]
#      deck += [copy(cardbook["Harvest Golem"]) for i in range(6)]
#      deck += [copy(cardbook["Fake Damage Spell 1"]) for i in range(6)]
    else:
      deck += [copy(cardbook["Wisp"]) for i in range(4)]
      deck += [copy(cardbook["River Crocolisk"]) for i in range(4)]
      deck += [copy(cardbook["Chillwind Yeti"]) for i in range(4)]
      for i in range(1,5):
        deck += [copy(cardbook["Fake Weapon %d"%i])]
      for i in range(1,8):
        deck += [copy(cardbook["Fake Creature %d"%i])]
      for i in range(1,8):
        deck += [copy(cardbook["Fake Damage Spell %d"%i])]

    from decks import Deck
    return Deck(deck)







































