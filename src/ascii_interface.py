import pdb
from board import Board

#Draws board and cards 
def draw_Board(self):
    player = self.engine.get_current_player()
    adv = self.engine.get_other_player()
    
    print ('='*36) + " Board " + ('='*37)
    print " "*20 + "Enemy: %s" % str(adv.hero)
    
    text = 'Enemy minions: '
    for i,c in enumerate(adv.hero.minions,1):
        print (i==0 and text or ' '*len(text))+( ' %d] %s' %(i,str(c)))
    print ' -'*40
    
    text = 'Your minions: '
    for i,c in enumerate(adv.hero.minions,1):
        print (i==0 and text or ' '*len(text))+(' %d] %s' %(i,str(c)))
    
    print (" "*20) + ("You: %s" % str(adv.hero))
    
    print ('='*36) + " Board " + ('='*37)
    text = 'Your cards: '
    for i,c in enumerate(player.cards,1):
        print (i==0 and text or ' '*len(text))+(' %d] %s' %(i,str(c)))


# attach each show function to a message
all_globs = globals().keys()
draw_funcs = [key for key in all_globs if key.startswith("draw_")]
for key in draw_funcs:
    if key[5:] in all_globs:
      setattr(globals()[key[5:]], "draw", globals()[key])




if __name__=="__main__":
    from players import HumanPlayer, RandomPlayer
    from cards import fake_deck
    from heroes import *
    from hs_engine import HSEngine

    deck1 = fake_deck()
    hero1 = Hero(Card_Mage())
    player1 = HumanPlayer('jerome', hero1, deck1)
    
    deck2 = fake_deck()
    hero2 = Hero(Card_Priest())
    player2 = RandomPlayer('IA', hero2, deck2)
    
    engine = HSEngine( player1, player2 )
    # initialize global variables
    
    # start playing
    engine.start_game()
    while not engine.is_game_ended():
      engine.board.draw()
      engine.play_hero()
    
    t = engine.turn
    print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)
  

















