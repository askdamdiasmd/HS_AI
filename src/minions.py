'''
list of all minions

Minions are instanciated by cards (hp, att).
The rest of their abilities is coded here.
'''



class Minion:
  def __init__(self, card ):
    self.name = card.name
    self.hp = card.hp
    self.att = card.att
    
    # what is below is what can be silenced
    self.effects = []
    self.action_filter = lambda x: x

  @classmethod
  def set_engine(cls, engine):
    cls.engine = engine

  def list_actions(self):
    if self.att==0:
      return None
    else:
      res = Act_Attack(self, self.board.list_attackable_characters())
      return self.action_filter(res)

  def attacked(self, hit):
    self.hp -= hit
    if self.hp <= 0:
      self.engine.send_message( Msg_Dying(self) )



class AncientWatcher (Minion):
  def __init__(self, card):
    Minion.__init__(self, card)
    self.effects.append( Eff_NoAttack() )


