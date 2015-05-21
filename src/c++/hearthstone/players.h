#ifndef __PLAYERS_H__
#define __PLAYERS_H__
#include "common.h"
#include "actions.h"

struct VizPlayer;
typedef shared_ptr<VizPlayer> PVizPlayer;

struct Player {
  SET_ENGINE();
  string name;
  Deck* const deck;
  struct State {
    int mana, max_mana;
    ListPCard cards;
    PHero hero;
    PWeapon weapon;
    ListPMinion minions;
    vector<float> minions_pos;
    ListPSecret secrets;
    int spell_power;
    char auchenai, velen; // how many are present on my side ?
  } state;
  PVizPlayer viz;

  const Act_EndTurn act_end_turn;

  Player(PHero hero, string name, Deck* deck);

  // shortcuts
  /*PHero hero() { return state.hero; }
  PWeapon weapon() { return state.weapon; }
  ListMinion minions() { return state.minions; }
  ListSecret secrets() { return state.secrets; }*/

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
  ListAction list_actions();

  bool add_thing(PInstance thing, Slot pos);
  Slot remove_thing(PInstance thing);

  void add_mana_crystal(int nb, bool useit = false);
  void use_mana(int nb);
  void gain_mana(int nb);

  void start_turn();
  void end_turn();

  void draw_card(Instance* origin);
  void give_card(PCard card, Instance* origin);
  void throw_card(PCard card);

  virtual ListPCard mulligan(ListPCard & cards) const = 0;
  void draw_init_cards(int nb, bool coin = false);
  virtual const Action* choose_actions(ListAction actions, Instance*& choice, Slot& slot) const = 0;

  float score_situation();
};


// ----------- Manual (human) player -----------

/* to be defined in your interface 
struct HumanPlayer : public Player {
  // human player : ask the player what to do

  HumanPlayer(PHero hero, string name, Deck* deck) :
    Player(hero, name, deck) {} 
};
*/


// ------ stupidest player ever ------------

struct RandomPlayer : public Player {
  //// random player : just do random things

  RandomPlayer(PHero hero, string name, Deck* deck) :
    Player(hero, name, deck) {}

  virtual ListPCard mulligan(ListPCard & cards) const {
    // keep everything without changing
    return{};
  }

  virtual const Action* choose_actions(ListAction actions, Instance*& choice, Slot& slot) const;
};





#endif