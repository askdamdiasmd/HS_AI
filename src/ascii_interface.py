import pdb
from hs_engine import HSEngine
from heroes import Mage
from cards import fake_deck


#Draws board and cards 
def draw_board(board):
    print "-"*50
    print "="*50
    print " "*15 + "PLAYER 2 HAND" + " "*15
    for j,c in enumerate(cards2):
        print hand_card_string(c)+"\t", 
        
    print
    print 
    print "="*50
    hero2 = board.get_enemy_hero(foe)
    print "\t Hero2: %s (%s)\t"%(hero2.name, "no weapon" if hero2.weapon is None else weapon_string(hero2.weapon))
    print 

    print "="*13+"  PLAYER 2 CREATURES    "+"="*13
    for j,c in enumerate(board.list_enemy_minions(foe)):
        print creature_string(c), 
    print
    print
    
    print 
    print "="*13+"  PLAYER 1 CREATURES    "+"="*13
    for j,c in enumerate(board.list_friendly_minions(foe)):
        print creature_string(c), 
    print
    print
    print "="*50
    hero1 = board.get_friendly_hero(foe)
    print "\t Hero2: %s (%s)\t"%(hero1.name, "no weapon" if hero1.weapon is None else weapon_string(hero1.weapon))
    print 

                         
    print "="*50
    print "          PLAYER 1 HAND             "
    for j,c in enumerate(cards1):
        print hand_card_string(c)+"\t", 
    print
    print 
    print "="*50
    print "-"*50



if __name__=="__main__":
    deck1 = fake_deck()
    hero1 = Mage()
    player1 = HumanPlayer('jerome', hero1)
    player1.set_deck(deck1)
    
    deck2 = fake_deck()
    hero2 = Mage('matttis')
    player2 = RandomPlayer('IA')
    player2.set_deck(deck2)
    
    engine = HSEngine( player1, player2 )
    
    # initialize global variables
    while not engine.is_game_ended():
      draw_board(engine.board, engine.get_current_hero())
      engine.play_hero()
    
    t = engine.turn
    print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)
  

















