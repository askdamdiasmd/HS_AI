#ifndef __BOARD_H__
#define __BOARD_H__
#include "common.h"


struct Slot {
  /* a position on the board (insertion index in players.minions[]) */
  const Player* player;
  int pos;  // in [0..6]
  float fpos; // float position for smart insertion

  Slot() : player(nullptr), pos(-1), fpos(-1) {}

  Slot(Player* owner, int index = -1, float fpos = -1) :
    player(owner), pos(index), fpos(fpos) {}

  string tostr() const {
    return string_format("Position %d", pos);
  }
};


struct Board {
  SET_ENGINE();
  ListInstance everybody;

  PVizBoard viz;

public:
  Board();

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

  void add_thing(PInstance thing, Slot pos = Slot());

  void remove_thing(PInstance m);

  bool is_game_ended() const;
  
  Player* get_enemy_player(Player* me);

  PHero get_enemy_hero(Player* me);

  ListMinion get_friendly_minions(Player* player);

  ListMinion get_enemy_minions(Player* player, bool targetable = false);

  ListMinion get_minions(Player* player, bool targetable = false);

  static ListCreature ListMinion_to_ListCreature(ListMinion minions);

  ListCreature get_characters(Player* player, bool targetable = false);

  ListCreature get_attackable_characters(Player* player);
    
  ListSlot get_free_slots(Player* player);

  Slot get_minion_pos(Instance* m);

  float score_situation(Player* player);
};


#endif