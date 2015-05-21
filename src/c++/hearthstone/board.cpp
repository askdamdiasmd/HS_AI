//#include "actions.h"
#include "board.h"
//#include "cards.h"
//#include "collection.h"
#include "creatures.h"
//#include "decks.h"
//#include "effects.h"
#include "engine.h"
//#include "events.h"
//#include "heroes.h"
//#include "messages.h"
#include "players.h"

Engine* Board::engine = nullptr;

Board::Board(Player* player1, Player* player2) :
  player1(player1), player2(player2) {
  state.turn = 0;
  state.n_dead = 0;
  state.everybody.push_back(player1->state.hero);
  state.everybody.push_back(player2->state.hero);
}

bool Board::is_game_ended() const {
  return players[0]->state.hero->is_dead() || players[1]->state.hero->is_dead();
}
const Player* Board::get_winner() const {
  bool dead0 = players[0]->state.hero->is_dead();
  bool dead1 = players[1]->state.hero->is_dead();
  if (dead0 && !dead1)  return players[1];
  if (!dead0 && dead1)  return players[0];
  return nullptr; // match nul
}

void Board::deal_init_cards() {
  assert(state.turn == 0);
  state.turn = -2; 
  players[0]->draw_init_cards(3, false);
  state.turn = -1;
  players[1]->draw_init_cards(4, true);
  state.turn = 0;
}

void Board::start_turn(Player* current) {
  for (auto& i : state.everybody)
    i->start_turn(current);
}
void Board::end_turn(Player* current) {
  for (auto& i : state.everybody)
    i->end_turn(current);
}

void Board::create_thing(PThing thing) {
  state.everybody.push_back(thing); // do this FIRST
}
bool Board::add_thing(PThing thing, const Slot& pos) {
  assert(in(thing, state.everybody)); // create_thing MUST be called before
  if (thing->player->add_thing(thing, pos)) { // will send signal
    thing->popup(); // will send signal
    return true;
  }
  else {
    state.everybody.pop_back(); // oops cancel
    return false;
  }
}

void Board::clean_deads() {
  ListPThing all_deads;

  while (state.n_dead) {
    if (engine->is_game_ended())  
      return; // don't care about the rest
    ListPThing deads;
    
    // search deads
    for (auto& i : state.everybody)
      if (i->is_dead()) {
        deads.push_back(i);
        state.n_dead--;
      }
    assert(state.n_dead == 0);

    // remove deads from board
    assert(state.dead_pos.empty());
    for (auto& i : deads) 
      // signal Event::RemoveThing
      state.dead_pos[i.get()] = i->player->remove_thing(i);

    // trigger death-rattles once cleaning is done
    for (auto& i : deads) {
      all_deads.push_back(i);
      i->signal_death();  // signal Event::ThingDead, then trigger death_rattles
      remove(state.everybody, i);  // remove from everybody
    }
    state.dead_pos.clear(); // was temporary
  }

  // finally 
  for (auto& i : all_deads) 
    i->silence(true); // remove auras
}

ListSlot Board::get_free_slots(Player* player) const {
  ListSlot res;
  if (player->state.minions.size() < 7) {
    const auto& mp = player->state.minions_pos;
    for (int i = 0; i < len(mp) - 1; ++i)
      res.emplace_back(player, i, (mp[i] + mp[i + 1]) / 2);
  }
  return res;
}

int Board::get_nb_free_slots(const Player* player) const {
  return 7 - len(player->state.minions);
}


Slot Board::get_minion_pos(Instance* i) {
  Minion* m = issubclass(i, Minion);
  if (m) {
    Player* pl = m->player;
    int pos = indexP(pl->state.minions, m);
    if (pos < 0) {
      Slot slot = state.dead_pos.at(m);
      slot.pos = -1;  // signal that it's not here anymore
      return slot;
    } else {
      float rel_index = pl->state.minions_pos[pos + 1];
      return Slot(pl, pos, rel_index);
    }
  }
  else
    return Slot(m->player);
}

float Board::score_situation(Player* player) {
  Player* adv = engine->board.get_other_player(player);
  if (player->state.hero->is_dead())
    return -INF;
  else if (adv->state.hero->is_dead())
    return INF;
  else
    return player->score_situation() - adv->score_situation();
}

//def save_state(num = 0)) {
//self.saved[num] = dict(minions = list(self.minions), state.everybody = list(self.state.everybody))
//for pl in self.players) {
//  pl.save_state(num)
//  for obj in self.state.everybody) {
//    obj.save_state(num)
//    def restore_state(num = 0)) {
//    self.__dict__.update(self.saved[num])
//    self.state.everybody = list(self.state.everybody)
//    self.minions = list(self.minions)
//    for pl in self.players) {
//      pl.restore_state(num)
//      for obj in self.state.everybody) {
//        obj.restore_state(num)
//        def hash_state()) {
//        return 0 # todo
//        def end_simulation()) {
//        self.saved = dict()
//        for pl in self.players) {
//          pl.end_simulation()
//          for obj in self.state.everybody) {
//            obj.end_simulation()
