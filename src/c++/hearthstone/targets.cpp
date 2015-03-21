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
    secrets =     0x0008
    targetable =  0x0010, // means that player can select target manually
    friendly =    0x0020,
    enemy =       0x0040,
    attackable =  0x0080, // things that can be attacked from the player viewpoint
*/

ListPInstance Target::resolve(Player* by_who, Instance* me) const {
  assert(!(tags & (friendly | enemy)));  // cannot be both
  Player* other = engine->get_other_player(by_who);
  ListPInstance res;
  const auto if_targetable = [by_who](const PThing& i){ return i->is_targetable(by_who); };

  if (tags & minions) {
    if (tags & targetable) {
      assert(!(tags & weapon)); // cannot be both
      if (!(tags & enemy))  // not enemy
        append_if(res, by_who->state.minions, if_targetable);
      if (!(tags & friendly)) // not friendly
        append_if(res, other->state.minions, if_targetable);
    }
    else {
      if (!(tags & enemy))  // not enemy
        append(res, by_who->state.minions);
      if (!(tags & friendly)) // not friendly
        append(res, other->state.minions);
    }
  }

  if (tags & heroes) {
    if (!(tags & enemy))  // not enemy
      res.push_back(by_who->state.hero);
    if (!(tags & friendly)) // not friendly
      res.push_back(other->state.hero);
  }

  if (tags & attackable) {
    // remove everything behind a taunt, if any
    assert(!(tags & weapon)); // cannot be both
    assert(tags & enemy); // useless otherwise
    ListPInstance res2;
    for (auto& p : res)
      if (CAST(p.get(),Thing)->is_taunt())
        res2.push_back(p);
    if (res2.size()) 
      swap(res,res2);  // optimized by STL
  }

  if (tags & weapon) {
    assert(!(tags & (attackable | targetable)));
    if (!(tags & enemy))  // not enemy
      if (by_who->state.weapon) res.push_back(by_who->state.weapon);
    if (!(tags & friendly)) // not friendy
      if (other->state.weapon) res.push_back(other->state.hero);
  }

  // remove dead stuff
  for (int i = 0; i < len(res); ++i)
    if (CAST(res[i].get(), Thing)->is_dead())
      fast_remove(res, i);

  assert(!(tags & secret)); // NI

  return res;
}
