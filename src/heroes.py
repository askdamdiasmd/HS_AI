import pdb
from creatures import Creature
from messages import Msg_DeadHero, Msg_Status

### ------------ Hero ----------

class Hero (Creature):
  def __init__(self, card, owner=None ):
      Creature.__init__(self, card, owner=owner )
      self.n_remaining_power = 0
      self.popup()

  def set_owner(self, owner):
      self.owner = owner
      self.card.owner = owner
      self.card.ability.owner = owner

  def save_state(self, num=0):
      Creature.save_state(self, num)
      self.saved[num]['n_remaining_power'] = self.n_remaining_power

  def __str__(self):
      return "[%s (%s) %dHP]" % (self.owner.name,self.card.name,self.hp)

  def is_targetable(self, player):
      return True
  
  def start_turn(self):
      Creature.start_turn(self)
      self.n_remaining_power = 1
      if self.has_effect('insensible'):
        self.remove_effect('insensible')

  def use_hero_power(self):
      self.n_remaining_power -= 1
      assert self.n_remaining_power>=0

  def list_actions(self):
      res = []
      if self.n_remaining_power:
        res += self.card.ability.list_actions()
      if self.n_atq<self.n_max_atq and self.atq>0 and not self.has_effect('frozen'):
        from actions import Act_HeroAttack
        res.append(Act_HeroAttack(self, self.engine.board.get_attackable_characters(self.owner)))
      return res

  def ask_for_death(self):
      self.engine.send_message( Msg_DeadHero(self), immediate=True)

  def hurt(self, damage, caster=None):
      if not self.has_effect('insensible'):
        Creature.hurt(self, damage, caster)

  def add_armor(self, n):
      self.armor += n
      self.engine.send_message(Msg_Status(self,"armor"), immediate=True)

  def score_situation(self):
      # healthpoint: 1 at 0, 0.3 at 30
      # sum_i=1..hp max(0, 1-0.0233*i)
      f = lambda n: ((19767-233*n)*n)/20000. 
      return f(min(42, self.armor+self.hp))


### --------  Heroes' abilities and cards  --------------------

from cards import Card, Card_Minion

class Card_HeroAbility (Card):
    def __init__(self, cost, name, actions, targets="none", **kwargs):
        Card.__init__(self, cost, name, collectible=False, **kwargs)
        self.targets = "targetable "+targets
        self.actions = actions # lambda self: [messages...]
    def list_actions(self):
        from actions import Act_HeroPower
        return [Act_HeroPower(self.owner, self.cost, self.list_targets(self.targets), self.actions)]


class Card_Hero (Card_Minion):
    def __init__(self, *args, **kwargs):
        ability = kwargs.pop('ability') 
        Card_Minion.__init__(self,*args,collectible=False,**kwargs)
        self.effects = ['charge'] # if weapon can attack immediately
        assert type(ability)==Card_HeroAbility
        self.ability = ability




































