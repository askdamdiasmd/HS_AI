'''
list of possible messages 
(contrary to actions, there are just here to to inform of an event)
'''



class Message:
  def __init__(self, sender):
    self.sender = sender


class Msg_Dying (Message):
  pass


