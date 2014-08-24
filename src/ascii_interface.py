def weapon_string(weapon):
    return weapon.name + " %d/%d"%(weapon.attack,weapon.durability)

def creature_string(c):
    return c.name+"(%d/%d)\t"%(c.att,c.hp)

def hand_card_string(c):
    return c.name + "(%d)"%c.cost

#Draws board and cards 
def draw_board(board, cards1, cards2, foe=None):
    if foe is None:
        foe = board.hero2

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

def fake_settings():
    cardbook = get_cardbook()
    wisp = cardbook[0]
    croco = cardbook[1]
    yeti = cardbook[2]

    board = Board(Mage(), Mage())
    board.minions1.append(wisp)
    board.minions1.append(croco)
    board.minions1.append(yeti)
    board.minions1.append(croco)
    board.minions2.append(croco)
    board.minions2.append(yeti)
    
    hand1 = []
    hand2 = []

    hand1.append(yeti)
    hand1.append(croco)
    hand2.append(wisp)
    hand2.append(croco)
    hand2.append(yeti)

    return board, hand1, hand2

if __name__=="__main__":
    from board import Board
    from heroes import Mage
    from cards import MinionCard, get_cardbook
    
    board, hand1, hand2 = fake_settings()
    draw_board(board, hand1, hand2)
