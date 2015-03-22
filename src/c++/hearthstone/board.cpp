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

bool Board::add_thing(PThing thing, const Slot& pos) {
  if (thing->player->add_thing(thing, pos)) { // will send signal
    state.everybody.push_back(thing);
    thing->popup(); // will send signal
    return true;
  }
  return false;
}
void Board::remove_thing(PThing m) {
  remove(state.everybody, m);
  m->player->remove_thing(m);
}

void Board::start_turn(Player* current) {
  for (auto& i : state.everybody)
    i->start_turn(current);
}
void Board::end_turn(Player* current) {
  for (auto& i : state.everybody)
    i->end_turn(current);
}

void Board::clean_deads() {
  assert(state.n_dead == 0);
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


Slot Board::get_minion_pos(PInstance i) {
  PMinion m = issubclassP(i, Minion);
  if (m) {
    Player* pl = m->player;
    int pos = index(pl->state.minions, m);
    assert(pos >= 0);
    float rel_index = pl->state.minions_pos[pos + 1];
    return Slot(pl, pos, rel_index);
  }
  else
    return Slot(m->player);
}

float Board::score_situation(Player* player) {
  Player* adv = engine->get_other_player(player);
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
