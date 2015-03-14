#include "Cards.h"
#include "collection.h"
#include "decks.h"
#include "players.h"
#include "creatures.h"
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
  const Collection& cardbook = Collection::Instance();  
  
  ArrayString cards { "Windchill Yeti" };

  PDeck deck1 = fake_deck(cardbook, dbg, cards);
  PHero hero1 = NEWP(Hero, issubclassP(cardbook.get_by_name("Anduin Wrynn"), const Card_Hero));
  PPlayer player1 = NEWP(HumanPlayer, hero1, "jerome", deck1.get());

  PDeck deck2 = fake_deck(cardbook, dbg, cards);
  PHero hero2 = NEWP(Hero, issubclassP(cardbook.get_by_name("Jaina Proudmoore"), const Card_Hero));
  PPlayer player2;
  if (false)
    player2 = NEWP(HumanPlayer, hero2, "mattis", deck2.get());
  else
    player2 = NEWP(RandomPlayer, hero2, "IA", deck2.get());
  
  //deck1->print();
  //deck2->print();

  init_screen();
  
  PEngine engine = NEWP(CursesEngine, player1.get(), player2.get());
  engine->board.viz = NEWP(VizBoard, &engine->board, bool(issubclassP(player2, HumanPlayer)), anim); 

  if (mana) {
    player1->add_mana_crystal(mana);
    player2->add_mana_crystal(mana);
  }
  if (setup) {
    //dbg_add_minion(player1, cardbook["injured blademaster"])
    NI; //dbg_add_minion(player2, cardbook["war golem"])
  }
  
  //start playing
  //show_ACS()
  //show_unicode();
  engine->start_game();
  while (!engine->is_game_ended())
    engine->play_turn();
    
  // end of game
  congratulate_winner(engine->get_winner(), engine->turn);

  end_screen();
  return 0;
}