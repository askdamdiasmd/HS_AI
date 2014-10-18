'''
list of possible messages

  messages = atomic actions

    (actions are decomposed into messages,
    which are then executed one after one)
'''
import pdb
from copy import copy


class Message (object):
    def __init__(self, caster):
        assert caster!=None, pdb.set_trace()
        self.caster = caster
    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine
    def execute(self):
        pass
    def __str__(self):
        assert 0

class TargetedMessage (Message):
    def __init__(self, caster, target):
        Message.__init__(self, caster)
        assert target!=None, pdb.set_trace()
        self.target = target

class CardMessage (Message):
    def __init__(self, caster, card):
        Message.__init__(self, caster)
        assert card!=None, pdb.set_trace()
        self.card = card

class Msg_Debug (Message):
    def __init__(self, caster, msg):
        Message.__init__(self, caster)
        self.msg = msg
    def __str__(self):
        return "from %s: %s" %(self.caster, self.msg)


# game messages

class Msg_StartTurn (Message):
    def execute(self):
        return self.caster.start_turn()
    def __str__(self):
        return "Start of turn for %s " % self.caster

class Msg_EndTurn (Message):
    def execute(self):
        self.caster.end_turn()
    def __str__(self):
        return "End of turn for %s" % self.caster

class Msg_Nothing (Message):
    pass


class Msg_Status (Message):
    ''' just to tell the interface that something happened '''
    def __init__(self, caster, attrs):
      Message.__init__(self,caster)
      self.attrs = attrs.split() if type(attrs)==str else attrs
      for attr in self.attrs:
        setattr(self,attr,copy(getattr(caster,attr)))
    def __str__(self):
        return "Change of status for %s: {%s}" % (self.caster,
                ', '.join(['%s=%s'%(a,getattr(self,a)) for a in self.attrs]))


# play card messages

class Msg_DrawCard (Message):
    '''just to inform that caster drew a card'''
    def __str__(self):
        return '%s is going to draw a card' % (self.caster)
    def execute(self):
        self.caster.draw_card()

class Msg_CardDrawn (CardMessage):
    '''just to inform that caster drew a card'''
    def __str__(self):
        return '%s draw %s from the deck' % (self.caster, self.card)

class Msg_DrawBurnCard (CardMessage):
    '''a card was burned because hand is too full'''
    def __str__(self):
        return '%s draw %s from the deck' % (self.caster, self.card)

class Msg_ThrowCard (CardMessage):
    def __str__(self):
        return "Player %s loses %s" % (self.caster,self.card)
    def execute(self):
        self.caster.throw_card(self.card)

class Msg_UseMana (Message):
    def __init__(self, caster, cost):
        Message.__init__(self, caster)
        self.cost = cost
    def execute(self):
        self.caster.use_mana(self.cost)
    def __str__(self):
        return "Player %s loses %d mana crystal" % (self.caster,self.cost)

class Msg_GainMana (Message):
    def __init__(self, caster, gain):
        Message.__init__(self, caster)
        self.gain = gain
    def execute(self):
        self.caster.gain_mana(self.gain)
    def __str__(self):
        return "Player %s gain %d mana crystal" % (self.caster,self.gain)

class Msg_PlayCard (CardMessage):
    def __init__(self, caster, card, cost):
        CardMessage.__init__(self, caster, card)
        self.cost = cost
    def __str__(self):
        return "%s plays %s" % (self.caster, self.card)
    def execute(self):
        self.engine.send_message(
            [Msg_UseMana(self.caster,self.cost), 
             Msg_ThrowCard(self.caster,self.card)], immediate=True)


class Msg_Fatigue (Message):
    """ fatigue damage points (no more cards) """
    def __init__(self, caster, damage):
        Message.__init__(self, caster)
        self.damage = damage
    def __str__(self):
        return "%s takes %d points of fatigue" % (self.caster, self.damage)
    def execute(self):
        self.caster.hero.hurt(self.damage)


# start/end messages

class Msg_StartAttack (TargetedMessage):
    def __str__(self):
        return "Minion %s attacks enemy %s" %(self.caster, self.target)
class Msg_EndAttack (Message):
    def __str__(self):
        return "End of attack."

class Msg_StartCard (CardMessage):
    def __str__(self):
        return "Player %s plays [%s]" % (self.caster, self.card)
class Msg_EndCard (CardMessage):
    def __str__(self):
        return "[%s] finishes." % self.card

class Msg_StartSpell (Msg_StartCard):
    def __str__(self):
        return "Player %s plays spell [%s]" % (self.caster, self.card)
class Msg_EndSpell (Msg_EndCard):
    def __str__(self):
        return "End of spell."

class Msg_StartHeroPower (Message):
    def execute(self):
        self.caster.hero.use_hero_power()
    def __str__(self):
        return "[%s] uses its hero power" % self.caster
class Msg_EndHeroPower (Message):
    def __str__(self):
        return "End of hero power effect"


# minion messages

class Msg_AddThing (Message):
    def __init__(self, caster, thing, pos=None):
        Message.__init__(self, caster)
        assert thing!=None, pdb.set_trace()
        self.thing = thing
        self.pos = pos
    def execute(self):
        self.engine.board.add_thing(self.thing, self.pos)
    def __str__(self):
        return "New %s on the board for %s" %(self.thing, self.caster)

class Msg_AddMinion (Msg_AddThing):
    pass
class Msg_AddWeapon (Msg_AddThing):
    def __str__(self):
        return "%s equipped a %s" %(self.caster, self.thing)
class Msg_AddSecret (Msg_AddThing):
    def __str__(self):
        return "%s sets %s" %(self.caster, self.thing)

class Msg_Popup (Message):
    def execute(self):
        self.caster.popup()
    def __str__(self):
        return "%s pops up" %(self.caster)

class Msg_MinionPopup (Msg_Popup):
  def __init__(self, caster, pos):
    Msg_Popup.__init__(self,caster)
    self.pos = pos
class Msg_SecretPopup (Msg_Popup):
  pass
class Msg_WeaponPopup (Msg_Popup):
  pass
class Msg_HeroPopup (Msg_Popup):
  pass


class Msg_CheckDead (Message):
    """ as soon as a minion dies, it asks for its cleaning """
    def execute(self):
        for i in self.engine.board.everybody:
          if i.dead:
            i.ask_for_death()
    def __str__(self):
        return "%s asks for dead cleaning." % self.caster


class Msg_Dead (Message):
    """ this thing just died, just for information """
    def __str__(self):
        return "%s dies." % self.caster
    def execute(self):
        self.caster.death()
class Msg_DeadMinion (Msg_Dead):
    pass
class Msg_DeadWeapon (Msg_Dead):
    pass
class Msg_DeadSecret (Msg_Dead):
    pass
class Msg_DeadHero (Msg_Dead):
    pass



class Msg_DeathRattle (Message):
    """ death rattle: execute a pre-specified instruction """
    def __init__(self, caster, msg, immediate=False):
      Message.__init__(self, caster)
      self.msg = msg
      self.immediate = immediate
    def __str__(self):
      return "Death rattle casted by %s: %s%s" % (self.caster, self.msg, 
                                                  self.immediate and ' (immediate)' or '')
    def execute(self):
      self.engine.send_message(self.msg,immediate=self.immediate)


# attack / heal

class Msg_Damage (TargetedMessage):
    def __init__(self, caster, target, damage):
        TargetedMessage.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        self.target.hurt(self.damage)
    def __str__(self):
        return "%s takes %d damage from %s." % (self.target, self.damage, self.caster)

class Msg_HeroDamage (Msg_Damage):
    """ damage from hero power """
    pass

class Msg_SpellDamage (Msg_Damage):
    """ damage from a spell """
    pass

class Msg_RandomDamage (Message):
    ''' deal damage to a random target'''
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        while True:
          r = random.randint(len(self.target))
          target = self.target[r]
        self.engine.send_message( Msg_Damage(self.caster,target,self.damage), immediate=True)
    def __str__(self):
        return "%s takes %d damage." % (self.target, self.damage)


class Msg_MultiRandomDamage (Message):
    ''' deal damage to a random target seveal times'''
    def __init__(self, caster, target, damage, each=1):
        Msg_Damage.__init__(caster, target, damage)
        self.each = each  # amount of damage of each hit
    def execute(self):
        # breaks down into new messages as time passes by
        self.engine.send_message( 
               [Msg_RandomDamage(self.caster, self.target,self.each),
                Msg_MultiRandomDamage(self.caster,self.target,self.damage-1,self.each)],
                immediate=True)
    def __str__(self):
        return "%s throws %dx%d hit damages randomly." % (self.caster, self.damage, self.each)


# Heal messages

class Msg_Heal (TargetedMessage):
    def __init__(self, caster, target, heal):
        TargetedMessage.__init__(self, caster, target)
        self.heal = heal
    def execute(self):
        self.target.heal(self.heal)
    def __str__(self):
        return "%s heals %s by %dHP." % (self.caster, self.target, self.heal)

class Msg_HeroHeal (Msg_Heal):
    pass




# Effect messages

class Msg_Silence (TargetedMessage):
    def __init__(self, caster, target):
        TargetedMessage.__init__(self,caster,target)
    def __str__(self):
        return "%s gets silenced by %s." % (self.target, self.caster)
    def execute(self):
        self.target.silence()


class Msg_BindEffect (TargetedMessage):
    def __init__(self, caster, target, effect):
        TargetedMessage.__init__(self,caster,target)
        self.effect = effect
    def __str__(self):
        return "%s binds effect [%s] to %s." % (self.caster, self.effect, self.target)
    def execute(self):
        self.effect.bind_to(self.target, caster=self.caster)


























