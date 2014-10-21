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

  def __str__(self):
      return "[%s (%s) %dHP]" % (self.owner.name,self.card.name,self.hp)

  def start_turn(self):
    Creature.start_turn(self)
    self.n_remaining_power = 1

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

  def hurt(self, damage):
      if self.has_effect('insensible'):
        return  # cannot die this turn
      assert damage>0, pdb.set_trace()
      absorbed = min(damage, self.armor)
      self.armor -= absorbed
      damage -= absorbed
      self.hp -= damage
      self.engine.send_message(Msg_Status(self,'hp armor'),immediate=True)
      self.check_dead()

  def add_armor(self, n):
      self.armor += n
      self.engine.send_message(Msg_Status(self,"armor"), immediate=True)



### --------  Heroes' abilities and cards  --------------------

from cards import Card, Card_Minion

class Card_HeroAbility (Card):
    def __init__(self, cost, name, actions, targets="none", **kwargs):
        Card.__init__(self, cost, name, collectible=False, **kwargs)
        self.targets = targets
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




































