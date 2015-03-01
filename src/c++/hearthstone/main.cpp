#include "collection.h"
#include "decks.h"
#include "players.h"
#include "creatures.h"
#include "heroes.h"

#include "curses_interface.h"


int main(int argc, char** argv) {

  bool anim, mana, dbg, setup;
  anim = mana = dbg = setup = false;
  for (int i = 0; i < argc; i++) {
    const char * a = argv[i];
    if (a == string("anim"))  anim = true;
    if (a == string("mana"))  mana = true;
    if (a == string("debug"))  dbg = true;
    if (a == string("setup")) setup = true;
  }
  
  // generate collection = all possible cards
  const Collection cardbook;
  
  
  ArrayString cards { "Windchill Yeti" };

  PDeck deck1 = fake_deck(cardbook, dbg, cards);
  PHero hero1 = NEWP(Hero, dynamic_pointer_cast<Card_Hero>(cardbook.by_name.at("Anduin Wrynn")));

  HumanPlayerAscii player1(hero1, 'jerome', deck1);
/*
  deck2 = fake_deck(cardbook,dbg,cards)
  hero2 = Hero(cardbook["Jaina Proudmoore"])
  if 0:
    player2 = HumanPlayerAscii(hero2, 'mattis', deck2)
  elif 1:
    from ai import SimpleAI
    player2 = SimpleAI(hero2, 'simpleAI', deck2)
  elif 1:
    from ai import VerySimpleAI
    player2 = VerySimpleAI(hero2, 'simpleAI', deck2)
  else:
    player2 = RandomPlayer(hero2, 'IA', deck2)
    */
  init_screen();
    /*
    engine = CursesHSEngine( player1, player2 )
    engine.board.viz = VizBoard(engine.board, switch=type(player2)==HumanPlayerAscii, animated=anim)

    if mana:
      player1.add_mana_crystal(mana)
      player2.add_mana_crystal(mana)  

    if setup:
      #dbg_add_minion(player1, cardbook["injured blademaster"])
      dbg_add_minion(player2, cardbook["war golem"])
    
    # start playing
    #show_ACS()*/

  //show_unicode();
  
  /*
    engine.start_game()
    while not engine.is_game_ended():
      engine.play_turn()
    
    t = engine.turn
    winner = engine.get_winner()
    NC = uc.getmaxyx(stdscr)[1]
    button = Button(10,NC/2-3,'  %s wins after %d turns!  ' % (winner.name, (t+1)/2),ty=5)
    button.draw(highlight=uc.black_on_yellow)
    show_panels()
    uc.getch()    
*/
  endwin();
  return 0;
}