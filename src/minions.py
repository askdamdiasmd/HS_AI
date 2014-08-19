'''
list of all minions

Minions are instanciated by cards (hp, att).
The rest of their abilities is coded here.
'''



class Minion:
  def __init__(self, hp, att ):
    self.hp = hp
    self.att = att

  def list_actions(self):
    res = []
    for enemy in self.board.list_attackable_characters():
      res.append( Act_Attack(enemy) )
    return res

  def attacked(self, hit):
    self.hp -= hit
    if self.hp <= 0:
      self.engine.send_message( Msg_Dying(self) )

