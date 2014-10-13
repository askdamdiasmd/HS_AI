'''
all HS cards
'''
import pdb
from minions import *
from actions import *
from effects import *

eff_trad_fr = dict(taunt='Provocation')
def tolist(l):
  if type(l)==str:  
    return l.split()
  else:
    return list(l)


### ------- Cards -------------

class Card (object):
    def __init__(self, cost, name, cls=None, desc='', name_fr='', desc_fr='', effects=(), img='' ):
      self.owner = None # specified after assigning deck to hero
      self.name = name
      self.name_fr = name_fr
      self.cost = cost  # mana cost
      self.cls = cls  # card class = 'priest', ...
      self.desc = desc
      self.desc_fr = desc_fr
      self.img = img
      self.effects = tolist(effects) # list of effects: {'taunt','stealth', or buffs that can be silenced}
      if desc=='':  
        #assert all([type(e)==str for e in self.effects]), "error: description is missing"
        self.desc = '. '.join(['%s%s'%(e[0].upper(),e[1:]) for e in self.effects if type(e)==str])
      if desc_fr=='':  
        self.desc_fr = '. '.join([eff_trad_fr[e] for e in self.effects if type(e)==str])

    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine

    def list_actions(self):
        assert 0, "must be overloaded"

    def list_targets(self, targets ): # helper function
        if targets=='none':
          return []
        elif targets=='friendly minions':
          return self.engine.board.get_friendly_minions(self.owner)
        elif targets=='minions':
          return self.engine.board.get_minions()
        elif targets=='neighbors':
          return 'neighbors'
        else:
          assert False, "error: unknown target '%s'" % targets


### --------------- Minion cards ----------------------

class Card_Minion (Card):
    def __init__(self, cost, atq, hp, name, cat=None, **kwargs ):
        Card.__init__(self, cost, name, **kwargs )
        self.hp = hp    # health point = life
        self.atq = atq  # attack
        self.cat = cat  # category of minion = 'beast', ...

    def __str__(self):
        return "%s (%d): %d/%d %s" % (self.name_fr, self.cost, self.atq, self.hp, self.desc)

    def list_actions(self):
        return Act_PlayMinionCard(self)


class Card_Minion_BC (Card_Minion):
    """ Minion with a battle cry """
    def __init__(self, cost, atq, hp, name, battlecry, targets='minions', **kwargs):
        Card_Minion.__init__(self, cost, atq, hp, name, **kwargs)
        self.battlecry = battlecry
        self.targets = targets
    def list_actions(self):
        targets = self.list_targets(self.targets)
        return Act_PlayMinionCard_BC(self, self.battlecry, targets)


### --------------- Weapon cards ----------------------

class Card_Weapon (Card):
    def __init__(self, cost, atq, hp, name, **kwargs ):
        Card.__init__(self, cost, name, **kwargs )
        self.hp = hp    # health point = weapon durability
        self.atq = atq  # attack

    def __str__(self):
        return "Weapon %s (%d): %d/%d %s" % (self.name_fr, self.cost, self.atq, self.hp, self.desc)

    def list_actions(self):
        return Act_PlayWeaponCard(self)




### ----------------- Spell cards -------------------------

class Card_Spell (Card):
    def __str__(self):
        return "%s (%d): %s" % (self.name_fr, self.cost, self.desc)



class Card_GenericSpell (Card_Spell):
    def __init__(self, cost, name, actions, targets='none', **kwargs ):
        Card_Spell.__init__(self, cost, name, **kwargs )
        self.actions = actions # lambda self: [Msg_* list]
        self.targets = targets # see list_targets()
    def list_actions(self):
        targets = self.list_targets(self.targets)
        return Act_PlaySpellCard(self,targets,self.actions)


class Card_Coin (Card_GenericSpell):
    def __init__(self, owner):
        Card_GenericSpell.__init__(self, 0, "The coin", lambda self: [Msg_GainMana(self.caster,1)],
                                   desc="Gain one mana crystal this turn only")


class Card_Wrath (Card_Spell):
    """ Druid : Wrath (2 choices) """
    def __init__(self):
        Card_Spell.__init__(self, 2, "Wrath", cls="Druid",name_fr="Colere")
    def list_actions(self):
        targets = self.engine.board.get_characters()
        hero = self.owner
        first = Act_SingleSpellDamageCard(self,targets,damage=3)
        actions = lambda self: [Msg_SpellDamage(self.caster,self.choices[0],self.damage),
                                Msg_DrawCard(hero)]
        second = Act_SingleSpellDamageCard(self,targets,damage=1,actions=actions)
        return [first,second]


class Card_DamageSpell (Card_Spell):
    def __init__(self, damage):
        Card_Spell.__init__(self, damage-1, "Fake Damage Spell %d"%damage,
                            name_fr="Faux Sort de dommage %d"%damage,
                            desc="Deal %d points of damage"%damage)
        self.damage = damage
    def list_actions(self):
        targets = self.engine.board.get_characters()
        return Act_SingleSpellDamageCard(self,targets,damage=self.damage)



### -------- Heathstone cardbook collection --------------

def get_cardbook():
  cardbook = {}
  def add( card ):
    assert card.name not in cardbook, "error: %s already in cardbook"%card.name
    cardbook[card.name] = card
    if card.name_fr:
      assert card.name_fr not in cardbook, "error: %s already in cardbook"%card.name
      cardbook[card.name_fr] = card

  ### O Mana ##################################
  
  add( Card_Minion(0, 1, 1, "Wisp", name_fr="Feu follet") )
  add( Card_GenericSpell(1,"Power Overwhleming", name_fr="Puissance accablante",
       actions = lambda self: [Msg_BindEffect(self.caster, self.choices[0], Eff_BuffMinion(4,4)), 
                               Msg_BindEffect(self.caster, self.choices[0], Eff_DieSoon(Msg_EndTurn))], 
       targets='friendly minions',
       desc_fr='Confere +4/+4 a un serviteur allie jusqu\'a la fin du tour. Puis il meurt.') )
  
  ### 1 Mana ##################################

  add( Card_Minion_BC(1, 2, 1, "Abusive Sergeant", Eff_BuffMinion(2,0,True), 
       name_fr="Sergent Grossier", 
       desc_fr="Cri de guerre: confere +2 ATQ a un serviteur pendant ce tour") )
  
  add( Card_Minion(1, 1, 2, 'Goldshire Footman',name_fr='Soldat de Comte de l\'Or',effects='taunt') )

  ### 2 Mana ##################################

  add( Card_Minion(2, 2, 2, "Dire Wolf Alpha", name_fr="Loup alpha redoutable",
       effects=[Eff_BuffLeftRight(1,0)], desc_fr="Les serviteurs adjacents ont +1 ATQ") )

  add( Card_Minion(1, 1, 1, "Spectral Spider", name_fr="Araignee spectrale") )
  add( Card_Minion(2, 1, 2, "Haunted Creeper", 
       effects=[Eff_DR_Invoke_Minion(cardbook["Spectral Spider"])]*2,
       name_fr="Rampante Hantee", cat='beast',
       desc_fr="Rale d'agonie: Invoque 2 Araignees spectrales 1/1") )

  add( Card_Minion_BC(2, 2, 1, "Ironbeak Owl", Eff_Silence(), name_fr="Chouette bec-de-fer",
       desc_fr="Cri de guerre: reduit au silence un autre serviteur") )
  
  add( Card_Minion(3, 4, 4, "Nerubian", name_fr="Nerubien") )
  add( Card_Minion(2, 0, 2, "Nerubian Egg", effects=[Eff_DR_Invoke_Minion(cardbook["Nerubian"])],
       name_fr="Oeuf de Nerubien", desc_fr="Rale d'agonie: Invoque un Nerubien 4/4", ) )

  add( Card_Minion(2, 2, 3, 'River Crocolisk', name_fr='Crocilisque des rivieres',cat='beast') )
  
  add( Card_Minion_BC(2, 2, 3, "Sunfury Protector", Eff_BuffMinion(0,0,others='taunt'), 'neighbors',
       name_fr="Protectrice Solfury",
       desc_fr="Cri de guerre: confere Provocation aux serviteurs adjacents") )
  
  ### 3 Mana ##################################

  add( Card_Minion(1, 2, 1, "Damaged Golem", name_fr="Golem endommage") )
  add( Card_Minion(3, 2, 3, "Harvest Golem", effects=[Eff_DR_Invoke_Minion(cardbook["Damaged Golem"])],
       name_fr="Golem des moissons", desc_fr="Rale d'agonie: Invoque un golem endommage 2/1", ) )

  add( Card_Minion_BC(3, 3, 2, "Shattered Sun Cleric", Eff_BuffMinion(1,1,False), 'friendly minions',
       name_fr="Clerc du Soleil brise",
       desc_fr="Cri de guerre: confere +1/+1 a un serviteur allie") )

  ### 4 Mana ##################################

  add( Card_Minion_BC(4, 4, 4, "Dark Iron Dwarf", Eff_BuffMinion(2,0,True), 
       name_fr="Nain sombrefer", 
       desc_fr="Cri de guerre: confere +2 ATQ a un serviteur pendant ce tour") )

  add( Card_Minion(4, 4, 5, 'Chillwind Yeti', name_fr='Yeti Noroit') )
  
  add( Card_Minion_BC(4, 2, 3, "Defender of Argus", Eff_BuffMinion(1,1,others='taunt'), 'neighbors',
       name_fr="Defenseur d'Argus",
       desc_fr="Cri de guerre: donne aux serviteurs adjacents +1/+1 et Provocation") )
  
  ### 5 Mana ##################################

  add( Card_Minion(1, 1, 2, "Slime", name_fr="Gelee", effects='taunt') )
  add( Card_Minion(5, 3, 5, "Sludge Belcher", effects=['taunt',Eff_DR_Invoke_Minion(cardbook["Slime"])],
       name_fr="Crache-vase", desc_fr="Rale d'agonie: Invoque 1 gelee 1/2 avec provocation") )
  
  ### 6 Mana ##################################

  add( Card_Minion(4, 4, 5, "Baine") )
  add( Card_Minion(6, 4, 5, "Cairne", effects=[Eff_DR_Invoke_Minion(cardbook["Baine"])],
       desc_fr="Rale d'agonie: Invoque un Baine 4/5" ) )

  add( Card_Minion(2, 2, 2, "Hyena", name_fr="Hyene", cat='beast') )
  add( Card_Minion(6, 6, 5, "Savannah Highmane", effects=[Eff_DR_Invoke_Minion(cardbook["Hyena"])]*2,
       name_fr="Grande Criniere des Savanes", cat='beast',
       desc_fr="Rale d'agonie: Invoque 2 hyenes 2/2") )

  ### 7 Mana ##################################
  ### 8 Mana ##################################
  ### 9+ Mana #################################

  
  
  # add fake creatures
  for i in range(1,11):
    add( Card_Minion(i,i+1,i,'Fake Creature %d'%i,name_fr="Fausse creature %d"%i) )
    
  # add fake weapons
  for i in range(1,5):
    add( Card_Weapon(i,i,2,'Fake Weapon %d'%i) )
  
  # add fake spells
  for i in range(1,10):
    add( Card_DamageSpell(i) )

  # Druid cards
  add( Card_Wrath() )

  # transform into a Dictionary
  return cardbook



def fake_deck(debug=False):
    from copy import deepcopy as copy
    cardbook = get_cardbook()
    deck = []
    if debug:
      deck += [copy(cardbook["Goldshire Footman"]) for i in range(15)]
      deck += [copy(cardbook["Power Overwhleming"]) for i in range(15)]
    else:
      deck += [copy(cardbook["Wisp"]) for i in range(2)]
      deck += [copy(cardbook["Power Overwhleming"]) for i in range(2)]
      deck += [copy(cardbook["Abusive Sergeant"]) for i in range(2)]
      deck += [copy(cardbook["Goldshire Footman"]) for i in range(2)]
      deck += [copy(cardbook["Nerubian Egg"]) for i in range(2)]
      deck += [copy(cardbook["Sunfury Protector"]) for i in range(2)]
      deck += [copy(cardbook["Defender of Argus"]) for i in range(2)]
      deck += [copy(cardbook["Harvest Golem"]) for i in range(2)]
      deck += [copy(cardbook["River Crocolisk"]) for i in range(2)]
      deck += [copy(cardbook["Chillwind Yeti"]) for i in range(2)]
      deck += [copy(cardbook["Sludge Belcher"]) for i in range(2)]
      deck += [copy(cardbook["Savannah Highmane"]) for i in range(2)]
      deck += [copy(cardbook["Cairne"]) for i in range(1)]
      for i in range(1,5):
        deck += [copy(cardbook["Fake Weapon %d"%i])]
      for i in range(1,5):
        deck += [copy(cardbook["Fake Damage Spell %d"%i])]
    
    from decks import Deck
    return Deck(deck)







































