#include "targets.h"
#include "creatures.h"
#include "engine.h"
#include "Board.h"
#include "players.h"

Engine* Target::engine = nullptr;

/*  None = 0,
    minions =     0x0001,
    heroes =      0x0002,
    characters =  0x0003, // minions + heroes
    weapon =      0x0004,
    targetable =  0x0008, // means that player can select target manually
    friendly =    0x0010,
    enemy =       0x0020,
    attackable =  0x0040, // things that can be attacked from the player viewpoint
*/

ListInstance Target::resolve(Player* owner, Instance* me) const {
  assert(!(tags & (friendly | enemy)));  // cannot be both
  Player* other = engine->get_other_player(owner);
  ListInstance res;
  const auto if_targetable = [](const PThing& i){ return !i->is_untargetable(); };

  if (tags & minions) {
    if (tags & targetable) {
      assert(!(tags & weapon)); // cannot be both
      if (!(tags & enemy))  // not enemy
        append_if(res, owner->state.minions, if_targetable);
      if (!(tags & friendly)) // not friendly
        append_if(res, other->state.minions, if_targetable);
    }
    else {
      if (!(tags & enemy))  // not enemy
        append(res, owner->state.minions);
      if (!(tags & friendly)) // not friendly
        append(res, other->state.minions);
    }
  }

  if (tags & heroes) {
    if (!(tags & enemy))  // not enemy
      res.push_back(owner->state.hero);
    if (!(tags & friendly)) // not friendly
      res.push_back(other->state.hero);
  }

  if (tags & attackable) {
    // remove everything behind a taunt, if any
    assert(!(tags & weapon)); // cannot be both
    assert(tags & enemy); // useless otherwise
    ListInstance res2;
    for (auto& p : res)
      if (((Thing*)p.get())->is_taunt())
        res2.push_back(p);
    if (res2.size()) 
      swap(res,res2);  // optimized by STL
  }

  if (tags & weapon) {
    assert(!(tags & (attackable | targetable)));
    if (!(tags & enemy))  // not enemy
      if(owner->state.weapon) res.push_back(owner->state.weapon);
    if (!(tags & friendly)) // not friendy
      if (other->state.weapon) res.push_back(other->state.hero);
  }

  return res;
}
