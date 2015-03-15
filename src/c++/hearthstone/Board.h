#ifndef __BOARD_H__
#define __BOARD_H__
#include "common.h"


struct Slot {
  /* a position on the board (insertion index in players.minions[]) */
  Player* player;
  int pos;  // in [0..6]
  float fpos; // float position for smart insertion

  Slot() : player(nullptr), pos(-1), fpos(-1) {}

  Slot(Player* owner, int index = -1, float fpos = -1) :
    player(owner), pos(index), fpos(fpos) {}

  string tostr() const {
    return string_format("Position %d", pos);
  }

  int insert_after_pos(const vector<float>& minion_pos) const {
    // return insertion pos in minion_pos such that order is preserved
    int i = 0, j = minion_pos.size() - 1;
    while (i + 1 < j) {
      int mid = (i + j) / 2;
      if (minion_pos[mid] <= fpos)
        i = mid;
      else
        j = mid;
    }
    //assert(minion_pos[j-1] <= fpos && fpos < minion_pos[j]);
    return j;
  }
};

struct VizBoard;
typedef shared_ptr<VizBoard> PVizBoard;

struct Board {
  SET_ENGINE();
  ListInstance everybody;

  PVizBoard viz;

public:
  Board() {}

  void add_thing(PInstance thing, Slot pos = Slot());

  void remove_thing(PInstance m);

  bool is_game_ended() const;
  
  ListSlot get_free_slots(Player* player);

  Slot get_minion_pos(PInstance m);

  float score_situation(Player* player);
};


#endif