'''
list of possible messages 
  
  messages = atomic actions 
  
    (actions are decomposed into messages, 
    which are then executed one after one)
'''



class Message:
    def __init__(self, caster):
        self.caster = caster
    @classmethod
    def set_engine(cls, engine):
        cls.engine = engine
    def execute(self):
        pass
    def __str__(self):
        return ""

class TargetedMessage:
    def __init__(self, caster, target):
        Message.__init__(self, caster)
        self.target = target



# game messages

class Msg_StartTurn (Message):
    def execute(self):
        self.caster.start_turn()
    def __str__(self):
        print "Start of turn for hero "+str(self.caster)

class Msg_EndTurn (Message):
    def execute(self):
        self.caster.end_turn()
    def __str__(self):
        print "End of turn for hero "+self.caster.show()

class Msg_Nothing (Message):
    pass



# minion messages

class Msg_AddMinion (Message):
    def __init__(self, caster, minion, pos):
        Message.__init__(self, caster)
        self.minion = minion
        self.pos = pos
    def execute(self):
        # self.target == minion
        self.engine.board.add_thing(self.minion, self.pos)
        # inform everybody that it was created
        self.engine.send_message(Msg_Popup(self.minion))
    def __str__(self):
        print "New minion '%s' on the board" %(self.minion, self.caster)

class Msg_Popup (Message):
    def execute(self):
        self.caster.popup()
    def __str__(self):
        return "Minion %s pops up" %(self.caster)

class Msg_Dying (Message):
    def execute(self):
        self.engine.board.remove_thing(self.caster)
    def __str__(self):
        return "Thing %s dies" % self.caster

class Msg_MinionDying (Msg_Dying):
    def __str__(self):
        return "Minion %s dies" % self.caster



# attack / heal

class Msg_StartAttack (Message):
    def __str__(self):
        return "Minion %s attacks enemy %s" %(self.caster, self.target)

class Msg_EndAttack (Message):
    def __str__(self):
        return "End of attack."

class Msg_Damage (TargetedMessage):
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        self.target.hurt(self.damage)
    def __str__(self):
        return "%s takes %d damage from %s." % (self.target, self.damage, self.caster)


class Msg_RandomDamage (Message):
    ''' deal damage to a random target'''
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        while True:
          r = random.randint(len(self.target))
          target = self.target[r]
        self.engine.send_message(Msg_Damage(self.caster,target,self.damage), immediate=True)
    def __str__(self):
        return "%s takes %d damage." % (self.target, self.damage)


class Msg_MultiRandomDamage (Message):
    ''' deal damage to a random target seveal times'''
    def __init__(self, caster, target, damage, each=1):
        Msg_Damage.__init__(caster, target, damage)
        self.each = each  # amount of damage of each hit
    def execute(self):
        # decomposes into new messages as time pass by
        self.engine.send_message([
          Msg_RandomDamage(self.caster, self.target,self.each),
          Msg_MultiRandomDamage(self.caster,self.target,self.damage-1,self.each)],
            immediate = True)
    def __str__(self):
        return "%s throws %dx%d hit damages randomly." % (self.target, self.damage, self.each)


# card/spell messages

class Msg_StartCard (TargetedMessage):
    def __str__(self):
        return "Player %s plays card %s" % (self.caster, self.target)
class Msg_EndCard (Message):
    def __str__(self):
        return "Card %s finishes." % self.target

class Msg_StartSpell (Msg_StartCard):
    def __str__(self):
        return "Player %s plays spell %s" % (self.caster, self.target)
class Msg_EndSpell (Msg_EndCard):
    def __str__(self):
        return "End of spell %s" % (self.target)






























