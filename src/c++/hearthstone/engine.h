#include "common.h"

#include "board.h"
#include "players.h"
#include "messages.h"


class Engine {
public:
  Board board;
  Player* players[2];
  int turn = 0;
  const bool is_simulation;

public:
  Engine(Player* player1, Player* player2, bool simulation = false) :
    players{ player1, player2 },
    board(player1, player2),
    turn(0), executing(false), 
    is_simulation(simulation) {
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

  virtual void display_status(PMsgStatus & message) = 0;

  bool is_game_ended() {
    return  board.is_game_ended();
  }

  PPlayer get_winner();
};


class  SimulationEngine : public Engine {
  Engine & true_engine;
  Board & board;
  int turn;
  unordered_map<htype, int> saved_turn;

public:
  SimulationEngine(Engine & true_engine, htype num) :
    Engine(true_engine.players[0], true_engine.players[1], true),
    true_engine(true_engine),
    board(true_engine.board),
    turn(true_engine.turn)
  {
    save_state(num);
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

  //def  send_status(, message) :
  //pass  //  we  don't  care

  virtual void display_status(PStatus & msg);
};



class  PlayEngine : public Engine {
  SimulationEngine launch_simulation( htype num = 0) {
    return SimulationEngine(this, num);
  }

  virtual void display_status(PStatus & msg) {
    // default behavior : just print on screen
    msg->print();
  }
};








