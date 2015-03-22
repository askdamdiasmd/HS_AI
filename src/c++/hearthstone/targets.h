#ifndef __TARGETS_H__
#define __TARGETS_H__
#include "common.h"


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
    targetable =      0x0040, // can be targeted by player (= not stealth)
    attackable =      0x00C0, // can be attacked (= not stealth or behind taunt)
    spell_targetable= 0x0140, // can be targeted by spell (=not stealth and not spell_untargetable)
  }; 
  unsigned int tags;

  Target() : Target(0) {}
  Target(unsigned int tags) : tags(tags) {}

  Target operator | (unsigned int tag) {
    return Target(tags | tag);
  }

  bool is_targetable() const {
    return (tags & targetable)!=0;
  }

  ListPInstance resolve(Player* owner, Instance* me = nullptr) const;
};




#endif