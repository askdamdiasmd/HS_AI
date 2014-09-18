'''
list of possible messages 
  
  messages = atomic actions 
  
    (actions are decomposed into messages, 
    which are then executed one after one)
'''
import pdb


class Message (object):
    def __init__(self, caster):
        self.caster = caster
    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine
    def execute(self):
        pass
    def __str__(self):
        return ""

class TargetedMessage (Message):
    def __init__(self, caster, target):
        Message.__init__(self, caster)
        self.target = target



# game messages

class Msg_StartTurn (Message):
    def execute(self):
        self.caster.start_turn()
    def __str__(self):
        return "Start of turn for %s " % self.caster

class Msg_EndTurn (Message):
    def execute(self):
        self.caster.end_turn()
    def __str__(self):
        return "End of turn for %s" % self.caster

class Msg_Nothing (Message):
    pass

class Msg_PlayCard (Message):
    def __init__(self, caster, card, cost):
        Message.__init__(self, caster)
        self.card = card
        self.cost = cost
    def __str__(self):
        return "Card %s used by %s for %d mana" % (self.card, self.caster, self.cost)
    def execute(self):
        self.caster.burn_card(self.card, self.cost)

class Msg_UseMana (Message):
    def __init__(self, caster, nb):
        Message.__init__(self, caster)
        self.nb = nb
    def execute(self):
        self.caster.mana -= nb
        assert self.caster.mana>=0
    def __str__(self):
        return "Player %s loses %d mana crystal" % (self.caster,self.nb)


# minion messages

class Msg_AddThing (Message):
    def __init__(self, caster, thing, pos):
        Message.__init__(self, caster)
        self.thing = thing
        self.pos = pos
    def execute(self):
        # self.target == thing
        self.engine.board.add_thing(self.thing, self.pos)
        # inform everybody that it was created
        self.engine.send_message(Msg_Popup(self.thing))
    def __str__(self):
        return "New %s on the board for %s" %(self.thing, self.caster)

class Msg_AddMinion (Msg_AddThing):
    pass

class Msg_Popup (Message):
    def execute(self):
        self.caster.popup()
    def __str__(self):
        return "%s pops up" %(self.caster)

class Msg_CheckDead (Message):
    """ as soon as a minion dies, it asks for its cleaning """
    def execute(self):
        res = []  # messages of those who died
        for i in self.engine.board.everybody:
          if i.dead:
            res.append(Msg_dead(i))
        return res
    def __str__(self):
        return "%s asks for dead cleaning." % self.caster


class Msg_Dead (Message):
    """ this thing effectively dies: execute deathrattle if any """
    def execute(self):
      self.caster.death()
    def __str__(self):
        return "%s dies" % self.caster

class Msg_MinionDying (Msg_Dead):
    pass
class Msg_WeaponDying (Msg_Dead):
    pass
class Msg_SecretDying (Msg_Dead):
    pass
class Msg_HeroDying (Msg_Dead):
    pass


# attack / heal

class Msg_StartAttack (TargetedMessage):
    def __str__(self):
        return "Minion %s attacks enemy %s" %(self.caster, self.target)
    def execute(self):
        self.caster.attacks()

class Msg_EndAttack (Message):
    def __str__(self):
        return "End of attack."

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

class Msg_RandomDamage (Message):
    ''' deal damage to a random target'''
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        while True:
          r = random.randint(len(self.target))
          target = self.target[r]
        return Msg_Damage(self.caster,target,self.damage)
    def __str__(self):
        return "%s takes %d damage." % (self.target, self.damage)


class Msg_MultiRandomDamage (Message):
    ''' deal damage to a random target seveal times'''
    def __init__(self, caster, target, damage, each=1):
        Msg_Damage.__init__(caster, target, damage)
        self.each = each  # amount of damage of each hit
    def execute(self):
        # breaks down into new messages as time passes by
        return [Msg_RandomDamage(self.caster, self.target,self.each),
                Msg_MultiRandomDamage(self.caster,self.target,self.damage-1,self.each)]
    def __str__(self):
        return "%s throws %dx%d hit damages randomly." % (self.target, self.damage, self.each)


# card/spell messages

class Msg_StartCard (Message):
    def __init__(self, caster, card):
        Message.__init__(self,caster)
        self.card = card
    def __str__(self):
        return "Player %s plays card %s" % (self.caster, self.card)
class Msg_EndCard (Message):
    def __str__(self):
        return "Card %s finishes." % self.target

class Msg_StartSpell (Msg_StartCard):
    def __str__(self):
        return "Player %s plays spell %s" % (self.caster, self.card)
class Msg_EndSpell (Msg_EndCard):
    def __str__(self):
        return "End of spell %s" % (self.target)

class Msg_StartHeroPower (Message):
    def __str__(self):
        return "Player %s uses its hero power" % self.caster
class Msg_EndHeroPower (Message):
    def __str__(self):
        return "End of hero power effect"




























