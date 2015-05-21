#ifndef __BOARD_H__
#define __BOARD_H__
#include "common.h"


struct VizBoard;
typedef shared_ptr<VizBoard> PVizBoard;

struct Board {
  SET_ENGINE();
  union {
    struct {
      Player* player1;
      Player*  player2;
    };
    Player* players[2];
  };

  struct State {
    int turn;
    ListPThing everybody;
    ListPSecret secrets;
    int n_dead;
    unordered_map<Thing*, Slot> dead_pos;
    unordered_map<Event, ListEffect> triggers;
  } state;
  PVizBoard viz;

public:
  Board() :
   player1(nullptr), player2(nullptr) {
    state.n_dead = 0;
    // empty initializer
  }

  Board(Player* player1, Player* player2);

  Board& operator = (const Board&) = default;

  bool is_game_ended() const;
  const Player* get_winner() const;
  
  void deal_init_cards();

  Player* get_current_player() { return players[(state.turn + 2) % 2]; }
  Player* get_enemy_player(){ return players[(state.turn + 3) % 2]; }
  Player* get_other_player(Player* p) { return p == players[0] ? players[1] : players[0]; }

  // retrieve shared_ptr from ptr
  template <typename T>
  shared_ptr<T> getP(T* i) {
    if (!i) return shared_ptr<T>(); // nullptr
    Thing* th = issubclass(i, Thing);
    return th ? CASTP(getPThing(th), T) : CASTP(getPSecret((Secret*)i), T);
  }
  PThing getPThing(Thing* i) {
    if (!i) return PThing(); // nullptr
    return state.everybody[indexP(state.everybody, i)];
  }
  PSecret getPSecret(Secret* i) {
    if (!i) return PSecret(); // nullptr
    return state.secrets[indexP(state.secrets, i)];
  }

  void start_turn(Player* current);
  void end_turn(Player* current);

  void create_thing(PThing thing); // add to everybody
  bool add_thing(PThing thing, const Slot& pos = Slot());
  void clean_deads();

  ListSlot get_free_slots(Player* player) const;

  int get_nb_free_slots(const Player* player) const;

  Slot get_minion_pos(Instance* m);

  float score_situation(Player* player);
};


#endif