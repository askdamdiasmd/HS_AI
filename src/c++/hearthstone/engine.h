#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "common.h"

#include "board.h"
#include "players.h"
#include "messages.h"


struct Engine {
public:
  Board board;
  Player* players[2];
  int turn = 0;
  bool is_simulation;

public:
  Engine(Player* player1, Player* player2) :
    players{ player1, player2 },
    board(player1, player2),
    turn(0), is_simulation(false) {
    set_default();
  }

  static void set_default();

  Player* get_current_player() {
    return players[turn % 2];
  }

  Player* get_other_player() {
    return  players[(turn + 1) % 2];
  }

  void start_game();

  ListAction list_player_actions(PPlayer & player);

  void play_turn();

  virtual void display_UPDATE_STATUS(PMsgUPDATE_STATUS & message) = 0;

  bool is_game_ended() {
    return  board.is_game_ended();
  }

  PPlayer get_winner();

private:
  unordered_map<htype, int> saved_turn;

public:
  Engine* launch_simulation(htype num = 0) {
    is_simulation = true;
    save_state(num);
    return this;
  }

  void save_state(int num = 0);

  //def  restore_state(, num = 0) :
  //__dict__.update(saved[num])
  //board.restore_state(num)

  //def  hash_state() :
  //return  board.hash_state()

  //def  end_simulation(, num = 0) :
  //restore_state(num)
  //del  saved
  //board.end_simulation()
  //true_engine.set_default()

  //def  send_UPDATE_STATUS(, message) :
  //pass  //  we  don't  care

  virtual void display_status(Msg_Status & msg) {
    assert(!is_simulation);
    // default behavior : just print on screen
    msg->print();
  }
};










#endif