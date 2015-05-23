#ifndef __VALIDATION_H__
#define __VALIDATION_H__
#include "common.h"
#include "curses_interface.h"

struct ScriptedEngine : public CursesEngine {
  PPlayer player1, player2;
  const Collection& cardbook;

  ScriptedEngine(const Collection& cardbook) :
    CursesEngine(), cardbook(cardbook) {}

  virtual PInstance random(ListPInstance& instances);

  bool validate_script(const string& script, bool viz = true);
  bool validate_script_file(const string& file_name, bool viz = true);
};

// for validation tests setups
struct ScriptedPlayer : public Player {
  /// human player : ask the player what to do
  ScriptedPlayer(PHero hero, string name, Deck* deck) :
    Player(hero, name, deck) {}

  virtual ListPConstCard mulligan(ListPConstCard & cards);
  virtual const Action* choose_actions(ListAction actions, Instance*& choice, Slot& slot);

  typedef pair<int, string> line_t;
  vector<line_t> script;
  void add_action(int num, const string& action) {
    script.emplace_back(num, action);
  }

  PInstance read_thing(int num, ListString& words);
  PCard read_card(int num, ListString& words);
};


#endif
