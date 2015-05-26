#ifndef __BOARD_H__
#define __BOARD_H__
#include "common.h"


struct VizBoard;
typedef shared_ptr<VizBoard> PVizBoard;

struct Board {
  SET_ENGINE();
  union {
    struct {
      Player* const player1;
      Player* const player2;
    };
    Player* const players[2];
  };

  struct State {
    int turn;
    ListPThing everybody;
    ListPSecret secrets;
    int n_dead;
    unordered_map<Thing*, Slot> dead_pos;
    unordered_map<Event, ListEffect, std::hash<int> > triggers;
  } state;
  PVizBoard viz;

public:
  Board() :
   player1(nullptr), player2(nullptr) {
    state.n_dead = 0;
    // empty initializer
  }

  Board(Player* player1, Player* player2);

  Board& operator = (const Board& copy) {
    memcpy((void*)players, copy.players, sizeof(players));
    state = copy.state;
    return *this;
  }

  bool is_game_ended() const;
  const Player* get_winner() const;
  
  void deal_init_cards();

  Player* get_current_player() { return players[(state.turn + 2) % 2]; }
  Player* get_enemy_player(){ return players[(state.turn + 3) % 2]; }
  Player* get_other_player(Player* p) { return p == players[0] ? players[1] : players[0]; }

  // retrieve shared_ptr from ptr
  template <typename T>
  shared_ptr<T> getP(Instance* i) {
    NI; // call should always ends up in a template specialization
    return nullptr;
  }

  void create_thing(PThing thing); // add to everybody
  bool add_thing(Instance* caster, PThing thing, const Slot& slot);
  bool add_secret(Instance* caster, PSecret secret);
  void clean_deads();

  ListSlot get_free_slots(Player* player) const;
  int get_nb_free_slots(const Player* player) const;
  Slot get_minion_pos(Instance* m);

  ListAction list_player_actions(Player* player);
  bool play_turn(); // return true when can't continue

  // game actions

  Player* start_turn();
  bool end_turn();

  // Signals are send at the deepest stack level, 
  // i.e. right when action is accomplished, but are not necessarily
  // processed immediately (wait for current phase to end).
  void register_trigger(const Effect* eff, int events);
  void unregister_trigger(const Effect* eff, int events);
  QueueSignal waiting_signals;
  void signal(Instance* from, Event event, Creature** target=nullptr, int nb=0);
  void signal_now(Instance* caster, Event event, Creature** target=nullptr, int nb=0);
  void process_signals();

  bool draw_card(Instance* caster, Player* player, int nb = 1);
  bool play_card(Instance* caster, const Card* card, const int cost);

  bool heal(Instance* from, int hp, Instance* to);
  bool damage(Instance* from, int hp, Instance* to);
  bool SpellHeal(Instance* from, int hp, Instance* to);
  bool SpellDamage(Instance* from, int hp, Instance* to);
  bool HeroHeal(Instance* from, int hp, Instance* to);
  bool HeroDamage(Instance* from, int hp, Instance* to);

  bool heal_zone(Instance* from, int hp, Target zone);
  bool damage_zone(Instance* from, int hp, Target zone);
  bool SpellHeal_zone(Instance* from, int hp, Target zone);
  bool SpellDamage_zone(Instance* from, int hp, Target zone);

  bool attack(Creature* from, Creature* target);


  float score_situation(Player* player);
};

template <>
shared_ptr<Instance> Board::getP<Instance>(Instance* i);
template <>
shared_ptr<Thing> Board::getP<Thing>(Instance* i);
template <>
shared_ptr<Minion> Board::getP<Minion>(Instance* i);
template <>
shared_ptr<Weapon> Board::getP<Weapon>(Instance* i);
template <>
shared_ptr<Creature> Board::getP<Creature>(Instance* i);
template <>
shared_ptr<Hero> Board::getP<Hero>(Instance* i);
template <>
shared_ptr<Secret> Board::getP<Secret>(Instance* i);

#endif
