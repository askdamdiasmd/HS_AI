#ifndef __ENGINE_H__
#define __ENGINE_H__
#include "common.h"
#include "Board.h"


struct Engine {
public:
  Board board;
  bool is_simulation;

public:
  Engine() { reset(); }
  void reset();

  // general game functions
  virtual void init_players(Player* player1, Player* player2) {
    reset();
    is_simulation = false;
    board = Board(player1, player2);
    // add virtual code to initialize board.viz
  }
  
  void play_game() {
    board.deal_init_cards();
    while (board.play_turn());
  }

  const Player* get_winner() const;

public:
  // display section

  list<PMessage> display;
  void send_display_message(PMessage msg) {
    //assert(!issubclassP(msg, Msg_ReceiveCard));
    if (!is_simulation) // useless if it's a simulation
      display.push_back(msg);
  }
  virtual void wait_for_display() = 0;
  virtual PInstance random(ListPInstance& instances);
};










#endif