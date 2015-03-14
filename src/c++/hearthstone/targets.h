#ifndef __TARGETS_H__
#define __TARGETS_H__
#include "common.h"


struct Target {
  SET_ENGINE();
  enum {
    None = 0,
    minions =     0x0001,
    heroes =      0x0002,
    characters =  0x0003,
    weapon =      0x0004,
    targetable =  0x0008, // means that player must select target manually
    friendly =    0x0010,
    enemy =       0x0020,
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

  //struct ResolvedTargets {
  //  const char* special_target;
  //  ListInstance objs;

  //  ResolvedTargets(const char* special) :
  //    special_target(special) {}
  //  //ResolvedTargets(ListMinion l, PHero h0 = nullptr, PHero h1 = nullptr, bool(*cond)(Minion*) = nullptr);
  //};

  //ResolvedTargets resolve_targets(Player* owner, Minion* me = nullptr);
};

//inline Target operator | (unsigned int tag, Target target) {
//  return Target(target.tags | tag);
//}



#endif