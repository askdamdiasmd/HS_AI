'''
all HS cards
'''
import pdb
from minions import *
from actions import *

eff_trad_fr = dict(taunt='Provocation', 
                   charge='Charge',
                   divine_shield='Bouclier Divin',
                   stealth='Camouflage',
                   windfury='Furie des vents',
                   effect='')
def tolist(l):
  if type(l)==str:  
    return l.split()
  else:
    return list(l)


### ------- Cards -------------

class Card (object):
    def __init__(self, cost, name, cls=None, desc='', name_fr='', desc_fr='', effects=(), 
                 img='', collectible=True ):
      self.owner = None # specified after assigning deck to hero
      self.name = name
      self.name_fr = name_fr
      self.cost = cost  # mana cost
      self.cls = cls  # card class = 'priest', ...
      self.desc = desc
      self.desc_fr = desc_fr
      self.img = img
      self.collectible = collectible # if False, cannot be in a deck
      self.effects = tolist(effects) # list of effects: {'taunt','stealth', or buffs that can be silenced}
      if desc=='':  
        #assert all([type(e)==str for e in self.effects]), "error: description is missing"
        self.desc = '. '.join(['%s%s'%(e[0].upper(),e[1:]) for e in self.effects if type(e)==str])
      if desc_fr=='':  
        fr = [eff_trad_fr[e] for e in self.effects if type(e)==str]
        self.desc_fr = '. '.join([e for e in fr if e])

    @classmethod
    def set_engine(cls, engine):
      cls.engine = engine

    def list_actions(self):
      assert 0, "must be overloaded"

    def list_targets(self, targets ): # helper function
      if targets=='none':
        return []
      elif targets=="characters":
        return self.engine.board.get_characters(self.owner)
      elif targets=='friendly minions':
        return self.engine.board.get_friendly_minions(self.owner)
      elif targets=='friendly beasts':
        return [b for b in self.engine.board.get_friendly_minions(self.owner) if b.card.cat=="beast"]
      elif targets=='minions':
        return self.engine.board.get_minions(self.owner)
      elif targets=='neighbors':
        return 'neighbors'
      elif targets=='enemy weapon':
        return self.engine.board.get_enemy_player(self.owner).weapon
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
    def __init__(self, cost, atq, hp, name, battlecry, targets='none', **kwargs):
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
    def __init__(self, cost, name, actions, targets='none', **kwargs ):
        Card.__init__(self, cost, name, **kwargs )
        self.actions = actions # lambda self: [Msg_* list]
        self.targets = targets # see list_targets()
    def __str__(self):
        return "%s (%d): %s" % (self.name_fr, self.cost, self.desc)
    def list_actions(self):
        targets = self.list_targets(self.targets)
        return Act_PlaySpellCard(self,targets,self.actions)


class Card_Coin (Card_Spell):
    def __init__(self, owner):
        Card_Spell.__init__(self, 0, "The coin", lambda self: [Msg_GainMana(self.caster,1)],
                                   desc="Gain one mana crystal this turn only")
        self.owner = owner

'''
class Card_Wrath (Card_Spell):
    """ Druid : Wrath (2 choices) """
    def __init__(self):
        Card_Spell.__init__(self, 2, "Wrath", cls="Druid",name_fr="Colere")
    def list_actions(self):
        targets = self.engine.board.get_characters()
        first = Act_SingleSpellDamageCard(self,targets,damage=3)
        actions = lambda self: [Msg_SpellDamage(self.caster,self.choices[0],self.damage),
                                Msg_DrawCard(self.owner)]
        second = Act_SingleSpellDamageCard(self,targets,damage=1,actions=actions)
        return [first,second]
'''

class Card_FakeDamageSpell (Card_Spell):
    def __init__(self, damage, targets="characters"):
        Card_Spell.__init__(self, damage-1, "Fake Damage Spell %d"%damage, None, targets,
                            name_fr="Faux Sort de dommage %d"%damage,
                            desc="Deal %d points of damage"%damage)
        self.damage = damage
    def list_actions(self):
        targets = self.list_targets(self.targets)
        return Act_SingleSpellDamageCard(self,targets,self.damage)






































