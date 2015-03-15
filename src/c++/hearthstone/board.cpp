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

void Board::add_thing(PInstance thing, Slot pos) {
  if (thing->player->add_thing(thing, pos))
    everybody.push_back(thing);
}

void Board::remove_thing(PInstance m) {
  remove(everybody,m);
  m->player->remove_thing(m);
}

bool Board::is_game_ended() const {
  Player** players = engine->players;
  return players[0]->state.hero->is_dead() || players[1]->state.hero->is_dead();
}

ListSlot Board::get_free_slots(Player* player) {
  ListSlot res;
  if (player->state.minions.size() < 7) {
    float* mp = player->state.minions_pos.data();
    for (int i = 0; i < len(player->state.minions); ++i)
      res.emplace_back(player, i, (mp[i] + mp[i + 1]) / 2);
  }
  return res;
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
//self.saved[num] = dict(minions = list(self.minions), everybody = list(self.everybody))
//for pl in self.players) {
//  pl.save_state(num)
//  for obj in self.everybody) {
//    obj.save_state(num)
//    def restore_state(num = 0)) {
//    self.__dict__.update(self.saved[num])
//    self.everybody = list(self.everybody)
//    self.minions = list(self.minions)
//    for pl in self.players) {
//      pl.restore_state(num)
//      for obj in self.everybody) {
//        obj.restore_state(num)
//        def hash_state()) {
//        return 0 # todo
//        def end_simulation()) {
//        self.saved = dict()
//        for pl in self.players) {
//          pl.end_simulation()
//          for obj in self.everybody) {
//            obj.end_simulation()
