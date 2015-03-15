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

  Player* get_current_player() { return players[turn % 2]; }
  Player* get_enemy_player(){ return players[(turn+1) % 2]; }
  Player* get_other_player(Player* p) { return p==players[0] ? players[1] : players[0]; }

  // general game functions
  void start_game();
  ListAction list_player_actions(Player* player);
  void play_turn();

  bool is_game_ended() { return board.is_game_ended(); }
  Player* get_winner();

  // game actions
  bool start_turn();
  bool end_turn();

  bool draw_card(PInstance caster, Player* player, int nb = 1);
  bool play_card(const Card* card, PInstance caster);
  void signal(Event event, PInstance from);

  bool heal(PInstance from, int hp, PInstance to);
  bool damage(PInstance from, int hp, PInstance to);

  bool attack(PInstance from, PInstance target);

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

  // display section

  list<PMessage> display;
  void send_display_message(PMessage msg);
  virtual void wait_for_display() = 0;

  void send_status(PMsgStatus msg) {
    if (!is_simulation)
      display_status(msg);
  }
  virtual void display_status(PMsgStatus msg);
};










#endif