#ifndef __PLAYERS_H__
#define __PLAYERS_H__
#include "common.h"

typedef vector<float> ArrFloat;


struct Player {
  SET_ENGINE();
  string name;
  Deck* const deck;
  struct State {
    int mana, max_mana;
    ListCard cards;
    PHero hero;
    PWeapon weapon;
    ListMinion minions;
    ArrFloat minions_pos;
    ListSecret secrets;
  } state;

  Player(PHero hero, string name, Deck* deck);

  // shortcuts
  PHero hero() { return state.hero; }
  PWeapon weapon() { return state.weapon; }
  ListMinion minions() { return state.minions; }
  ListSecret secrets() { return state.secrets; }

  /*def save_state(num = 0) :
  state.deck.save_state(num)
  state.saved[num] = dict(cards = list(state.cards), minions = list(state.minions), minions_pos = list(state.minions_pos),
  mana = state.mana, max_mana = state.max_mana, weapon = state.weapon, secrets = list(state.secrets))
  def restore_state(num = 0) :
  state.deck.restore_state(num)
  state.__dict__.update(state.saved[num])
  state.cards = list(state.cards)
  state.minions = list(state.minions)
  state.minions_pos = list(state.minions_pos)
  state.secrets = list(state.secrets)
  def end_simulation() :
  state.saved = dict()*/

  string tostr() const;

  bool add_thing(PInstance thing, Slot pos);

  void remove_thing(PInstance thing);

  void add_mana_crystal(int nb, bool useit = false);

  void use_mana(int nb);

  void gain_mana(int nb);

  void start_turn();

  void end_turn();

  ListAction list_actions();

  void draw_card();

  void give_card(PCard card, Instance* origin);

  void throw_card(PCard card) {
    remove(state.cards,card);
  }

  virtual void mulligan(ListCard cards) = 0;

  void draw_init_cards(int nb, bool coin = false);

  virtual Action* choose_actions(ListAction actions) = 0;

  float score_situation();
};


// ----------- Manual (human) player -----------

struct HumanPlayer : public Player {
  // human player : ask the player what to do

  HumanPlayer(PHero hero, string name, Deck* deck) :
    Player(hero, name, deck) {} 
};



// ------ stupidest player ever ------------

struct RandomPlayer : public Player {
  //// random player : just do random things

  //virtual ListPCard mulligan(ListPCard cards) {
  //  // keep everything without changing
  //  return{};
  //}

  //virtual PAction choose_actions(ListPAction actions) {
  //  // select one action in the list
  //  int r = randint(0, len(actions) - 1);
  //  PAction action = actions[r];
  //  // select one target for this action
  //  for (ch : action.choices) {
  //    if ch :
  //    action.select(i, random.randint(0, len(ch) - 1));
  //  }
  //  return action;
  //}
};



#endif