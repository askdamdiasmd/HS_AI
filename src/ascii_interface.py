def weapon_string(weapon):
    return weapon.name + " %d/%d"%(weapon.attack,weapon.durability)

def creature_string(c):
    return c.name+"(%d/%d)\t"%(c.att,c.hp)

def hand_card_string(c):
    return c.name + "(%d)"%c.card.cost

#Draws board and cards 
def draw_board(board, cards1, cards2, foe=None):
    if foe is None:
        foe = board.heroes[1]

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
    import copy
    from minions import Minion

    cardbook = get_cardbook()
    wisp1 = Minion(copy.copy(cardbook[0]))
    croco1 = Minion(copy.copy(cardbook[1]))
    yeti1 = Minion(copy.copy(cardbook[2]))

    wisp2 = Minion(copy.copy(cardbook[0]))
    croco2 = Minion(copy.copy(cardbook[1]))
    yeti2 = Minion(copy.copy(cardbook[2]))


    hero1 = Mage()
    hero2 = Mage()

    wisp1.owner=hero1
    croco1.owner=hero1
    yeti1.owner=hero1

    wisp2.owner=hero2
    croco2.owner=hero2
    yeti2.owner=hero2


    board = Board(hero1, hero2)
    board.minions.append(wisp1)
    board.minions.append(croco1)
    board.minions.append(yeti1)
    board.minions.append(croco2)
    board.minions.append(croco2)
    board.minions.append(yeti2)
    
    hand1 = []
    hand2 = []

    hand1.append(yeti1)
    hand1.append(croco1)
    hand2.append(wisp2)
    hand2.append(croco2)
    hand2.append(yeti2)

    return board, hand1, hand2

if __name__=="__main__":
    from board import Board
    from heroes import Mage
    from cards import MinionCard, get_cardbook
    
    board, hand1, hand2 = fake_settings()
    draw_board(board, hand1, hand2)
