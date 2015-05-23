#include "Cards.h"
#include "collection.h"
#include "decks.h"
#include "players.h"
#include "creatures.h"
#include "curses_interface.h"
#include "validation.h"



int main(int argc, char** argv) {
  bool anim = true, dbg = true, setup = false;
  int mana = 10;
  for (int i = 0; i < argc; i++) {
    const char * a = argv[i];
    if (a == string("anim"))  anim = true;
    if (a == string("mana"))  mana = true;
    if (a == string("debug"))  dbg = true;
    if (a == string("setup")) setup = true;
  }
  
  init_screen();

  // generate collection = all possible cards
  const Collection& cardbook = Collection::CardBook();  

  /*ScriptedEngine testbed(cardbook);
  assert(testbed.validate_script_file("test1.txt"));*/
  
  //ArrayString cards { "Heavy Axe", "Fiery War Axe" };
  //ArrayString cards{ "Haunted Creeper", "Dire Wolf Alpha", "Argent Squire" };
  ArrayString cards{  "Unstable Ghoul", "Zombie chow", "Auchenai Soulpriest" };

  const int nb_cards = 30;
  PDeck deck1 = fake_deck(cardbook, dbg, cards, nb_cards);
  PHero hero1 = NEWP(Hero, issubclassP(cardbook.get_by_name("Anduin Wrynn"), const Card_Hero));
  PPlayer player1;
  if (true)
    player1 = NEWP(HumanPlayer, hero1, "jerome", deck1.get());
  else
    player1 = NEWP(RandomPlayer, hero1, "jerome", deck1.get());

  PDeck deck2 = fake_deck(cardbook, dbg, cards, nb_cards);
  PHero hero2 = NEWP(Hero, issubclassP(cardbook.get_by_name("Jaina Proudmoore"), const Card_Hero));
  PPlayer player2;
  if (false)
    player2 = NEWP(HumanPlayer, hero2, "mattis", deck2.get());
  else
    player2 = NEWP(RandomPlayer, hero2, "IA", deck2.get());
  
  //deck1->print();
  //deck2->print();

  //show_ACS()
  //show_unicode();

  VizBoard::accel = 1;
  VizBoard::animated = anim;
  VizBoard::switch_heroes = bool(issubclassP(player2, HumanPlayer));

  CursesEngine engine;
  engine.init_players(player1.get(), player2.get());
  if (mana) {
    player1->add_mana_crystal(mana);
    player2->add_mana_crystal(mana);
  }
  
  //start playing
  engine.play_game();
    
  // end of game
  congratulate_winner(engine.get_winner(), engine.board.state.turn);

  end_screen();
  return 0;
}
