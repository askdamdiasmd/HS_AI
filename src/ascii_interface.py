import os, pdb
from board import Board
from players import *
from actions import *

#Draws board and cards
def draw_Board(self):
    # clear screen
    os.system("cls" if os.name=='nt' else "clear")

    player = self.engine.get_current_player()
    adv = self.engine.get_other_player()

    print ('='*36) + " Board " + ('='*37)
    print " "*20 + "Enemy: [%s]" % str(adv)

    print 'Enemy minions: '
    for i,c in enumerate(adv.minions,1):
        print (' %d] %s' %(i,str(c)))
    print ' -'*40

    print 'Your minions: '
    for i,c in enumerate(player.minions,1):
        print (' %d] %s' %(i,str(c)))

    print (" "*20) + ("You: [%s]" % str(player))

    print ('='*36) + " Board " + ('='*37)
    text = 'Your cards:'
    for i,c in enumerate(player.cards,1):
        print (i==1 and text or ' '*len(text))+(' %d] %s' %(i,str(c)))
    print ' -'*40


# attach each show function to a message
all_globs = globals().keys()
draw_funcs = [key for key in all_globs if key.startswith("draw_")]
for key in draw_funcs:
    if key[5:] in all_globs:
      setattr(globals()[key[5:]], "draw", globals()[key])


class HumanPlayerAscii (HumanPlayer):
  ''' human player : ask the player what to do'''
  def read_int(self, allowed=(), nb=-1):
      while True:
        try:
          inp = raw_input()
          inp = inp.split()
          inp = [int(i) for i in inp]
          if len(set(inp) & set(allowed))==len(inp):
            if nb<0 or len(inp)==nb:
              return inp
        except ValueError:
          pass
        print "erroneous input, please try again:",

  def mulligan(self, cards):
      print "available cards:"
      for i,card in enumerate(cards):
          print "  %d] %s" % (i, card)
      print "which one do you want to remove (ex: 1 3) ?",
      discard = self.read_int(allowed=range(len(cards)))
      return [cards[int(i)] for i in discard]

  def choose_actions(self, actions):
      # split actions
      act_hero = []
      act_card = []
      act_atq = []
      act_others = []
      for a in actions:
        if issubclass(type(a),Act_HeroPower):
          act_hero.append(a)
        elif issubclass(type(a),Act_PlayCard):
          act_card.append(a)
        elif issubclass(type(a),Act_Attack):
          act_atq.append(a)
        else:
          act_others.append(a)

      print "Choose an action:",
      print "          Available mana %d/%d" %(self.mana,self.max_mana)
      n = 0
      mapping = {}
      if act_hero:
        for i,a in enumerate(act_hero,n):
          print " %d] %s" % (i, a)
          mapping[i] = a
        n += len(act_hero)
      else:
        n = 1
      if act_atq:
        print " %d] Attack with a minion" % n
        mapping[n] = act_atq
        n += 1
      if act_card:
        for i,a in enumerate(act_card,n):
          print " %d] %s" %(i, a)
          mapping[i] = a
        n += len(act_card)
      for i,a in enumerate(act_others,n):
        print " %d] %s" %(i,a)
        mapping[i] = a

      choices = []
      while 1:
        print "Your choice ?",
        keys = mapping.keys() if type(mapping)==dict else range(len(mapping))
        c = self.read_int(keys,nb=1)[0]
        act = mapping[c]

        if type(act)==list:
          mapping = act
        else:
          if issubclass(type(act), Action):
            action = act
          else:
            choices.append(act)
          if len(choices)>=len(action.choices):
            return action.select(choices)
          mapping = action.choices[len(choices)]

        # print choices
        for i,a in enumerate(mapping,0):
          whom = lambda m: "Your " if m.owner is self else "His "
          print " %d] %s%s" %(i, issubclass(type(a),Minion) and whom(a) or '', a)




if __name__=="__main__":
    from cards import fake_deck
    from heroes import *
    from hs_engine import HSEngine

    deck1 = fake_deck()
    hero1 = Hero(Card_Mage())
    player1 = HumanPlayerAscii(hero1, 'jerome', deck1)

    deck2 = fake_deck()
    hero2 = Hero(Card_Priest())
    player2 = RandomPlayer(hero2, 'IA', deck2)

    engine = HSEngine( player1, player2 )
    # initialize global variables

    # start playing
    engine.start_game()
    while not engine.is_game_ended():
      engine.board.draw()
      engine.play_turn()
      raw_input()

    t = engine.turn
    print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)


















