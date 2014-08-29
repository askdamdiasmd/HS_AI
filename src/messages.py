'''
list of possible messages 
  
  messages = atomic actions 
  
    (actions are decomposed into messages, 
    which are then executed one after one)
'''



class Message:
    def __init__(self, caster):
      self.caster = caster
    def execute(self):
        pass

class TargetedMessage:
    def __init__(self, caster, target):
      Message.__init__(self, caster)
      self.target = target



# game messages

class Msg_StartTurn (Message):
    def execute(self):
      self.caster.start_turn()

class Msg_EndTurn (Message):
    def execute(self):
      self.caster.end_turn()

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

class Msg_Popup (Message):
    def execute(self):
        self.caster.popup()


class Msg_Dying (Message):
    def execute(self):
        self.engine.board.remove_minion(self.origin)



# attack / heal

class Msg_StartAttack (Message):
    pass
class Msg_EndAttack (Message):
    pass

class Msg_Damage (TargetedMessage):
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        self.target.hurt(self.damage)


class Msg_SpellDamage (Msg_Damage):
    ''' same but launched
     by a spell'''
    pass


class Msg_Heal (TargetedMessage):
    def __init__(self, caster, target, heal):
        Message.__init__(self, caster, target)
        self.heal = heal
    def execute(self):
        self.target.heal(self.heal)


# card/spell messages

class Msg_StartCard (Message):
    pass
class Msg_EndCard (Message):
    pass

class Msg_StartSpell (Msg_StartCard):
    pass
class Msg_EndSpell (Msg_EndCard):
    pass






























