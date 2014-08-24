'''
list of possible messages 
(contrary to actions, there are just here to to inform of an event)
'''



class Message:
    def __init__(self, caster):
      self.caster = caster
    def execute(self):
        pass


class Msg_StartTurn (Message):
    def execute(self):
      self.caster.start_turn()

class Msg_EndTurn (Message):
    pass


class Msg_Damage (Message):
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster)
        self.target = target
        self.damage = damage
    def execute(self):
        self.target.hurt(self.damage)


class Msg_Heal (Message):
    def __init__(self, caster, target, heal):
        Message.__init__(self, caster)
        self.target = target
        self.damage = heal
    def execute(self):
        self.target.heal(self.heal)


class Msg_Dying (Message):
    def execute(self):
        # get position on board
        pos = self.engine.board.remove_minion(self.origin)
        # execute death rattle
        for e in self.origin.effects:
            if e[0]=='death_rattle':              
              e[1].execute(pos)



