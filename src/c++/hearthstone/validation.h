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

  void validate_script(const string& script, bool viz=false);
};

// for validation tests setups
struct ScriptedPlayer : public Player {
  /// human player : ask the player what to do
  ScriptedPlayer(PHero hero, string name, Deck* deck) :
    Player(hero, name, deck) {}

  virtual ListPCard mulligan(ListPCard & cards) const;

  virtual const Action* choose_actions(ListAction actions, Instance*& choice, Slot& slot) const;
};


#endif
