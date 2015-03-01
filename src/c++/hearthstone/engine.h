#ifndef __ENGINE_H__
#define __ENGINE_H__
#include "common.h"
#include "Board.h"


struct Engine {
public:
  Board board;
  Player* players[2];
  int turn;
  bool is_simulation;

public:
  Engine(Player* player1, Player* player2);

  void set_default();

  Player* get_current_player() {
    return players[turn % 2];
  }

  Player* get_other_player() {
    return  players[(turn + 1) % 2];
  }

  void start_game();

  ListAction list_player_actions(Player* player);

  void play_turn();

  void send_status(PMsgStatus msg) {
    if (!is_simulation)
      display_status(msg);
  }
  
  void display_status(PMsgStatus msg);

  bool is_game_ended();

  Player* get_winner();

  // game functions
  void signal(Event event, PInstance from);

  void heal(PInstance from, int hp, PInstance to);
  void damage(PInstance from, int hp, PInstance to);

private:
  //unordered_map<htype, int> saved_turn;

public:
  /*Engine* launch_simulation(htype num = 0) {
    is_simulation = true;
    save_state(num);
    return this;
  }*/

  //void save_state(int num = 0);

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
};










#endif