#ifndef __TARGETS_H__
#define __TARGETS_H__
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
    return string_format("[Slot %d (%f)]", pos, fpos);
  }

  bool empty() const {
    return pos < 0 && fpos < 0;
  }

  int insert_after_pos(const vector<float>& minion_pos) const {
    assert(minion_pos[0] < fpos && fpos < minion_pos.back());
    // return insertion pos in minion_pos such that order is preserved
    int i = 0, j = minion_pos.size() - 1;
    while (i + 1 < j) {
      int mid = (i + j) / 2;
      if (minion_pos[mid] <= fpos)
        i = mid;
      else
        j = mid;
    }
    assert(minion_pos[j-1] <= fpos && fpos < minion_pos[j]);
    return j;
  }
};



struct Target {
  SET_ENGINE();
  enum {
    None = 0,
    minions =         0x0001,
    heroes =          0x0002,
    characters =      0x0003,
    weapon =          0x0004,
    secret =          0x0008,
    friendly =        0x0010,
    enemy =           0x0020,
    targetable =      0x0040, // can be targeted (= not stealth)
    attackable =      0x00C0, // can be targeted and attacked (= not stealth or behind taunt)
    spell_targetable= 0x0140, // can be targeted by spell (=not stealth and not spell_untargetable)
    neighbors =       0x0200, // neighbors with respect to a slot or a minion
    other =           0x0400, // not the caster
    random =          0x0800, // select a random result in the list
  }; 
  const unsigned int tags;
  const int breed;

  Target() : Target(0) {}
  Target(unsigned int tags, int breed = 0) : 
    tags(tags), breed(breed) {}

  Target operator | (unsigned int tag) {
    return Target(tags | tag);
  }

  bool is_targetable() const {
    return (tags & targetable)!=0;
  }

  ListPInstance resolve(Player* owner, Instance* me = nullptr, Slot slot = Slot()) const;

  string tostr(bool simple=false) const;
};




#endif