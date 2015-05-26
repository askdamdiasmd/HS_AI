#include "targets.h"
#include "creatures.h"
#include "engine.h"
#include "Board.h"
#include "players.h"

Engine* Target::engine = nullptr;

/*  None = 0,
    minions =         0x0001, 1
    heroes =          0x0002, 2
    characters =      0x0003, 3
    weapon =          0x0004, 4
    secret =          0x0008, 8
    friendly =        0x0010, 16
    enemy =           0x0020, 32
    targetable =      0x0040, 64 // can be targeted (= not stealth)
    attackable =      0x00C0, 192 // can be targeted and attacked (= not stealth or behind taunt)
    spell_targetable= 0x0140, 320 // can be targeted by spell and Hero Power (=not stealth and not spell_untargetable)
*/

ListPInstance Target::resolve(Player* by_who, Instance* me, Slot slot) const {
  #define DEFINED(prop)  ((tags&prop)==prop)
#define BREED(th)  ((breed<0? th->breed | breed : th->breed & breed)==breed)
  assert(!(DEFINED(friendly) && DEFINED(enemy)));  // cannot be both
  Player* other_pl = engine->board.get_other_player(by_who);
  ListPInstance res;

  if (DEFINED(neighbors)) {
    assert(tags == neighbors && breed==0);  // nothing else can be defined
    if (slot.empty())
      slot = engine->board.get_minion_pos(me);
    assert(slot.fpos > 0);

    const int i = slot.insert_after_pos(by_who->state.minions_pos) - 1;
    // if me in minions: me is at minions[i-1] 
    const ListPMinion& minions = by_who->state.minions;
    const int N = len(minions);
    assert(len(by_who->state.minions_pos) == N + 2);

    if (0 <= i - 1 && i - 1 < N) {
      if (minions[i - 1].get() != me)  // not myself!
        res.push_back(minions[i - 1]);
      else if (0 <= i - 2)
        res.push_back(minions[i - 2]);
    }
    if (i < N)
      res.push_back(minions[i]);
  }
  else {
    if (DEFINED(minions)) {
      unsigned int breed = this->breed;
      if (DEFINED(spell_targetable)) {
        const auto if_spell_targetable = [breed, by_who](const PMinion& i){ 
          return BREED(i) && !i->is_dead() && i->is_spell_targetable(by_who);
        };
        assert(!DEFINED(weapon)); // cannot be both
        if (!DEFINED(enemy))  // not enemy
          append_if(res, by_who->state.minions, if_spell_targetable);
        if (!DEFINED(friendly)) // not friendly
          append_if(res, other_pl->state.minions, if_spell_targetable);
      }
      else
        if (DEFINED(targetable)) {
          const auto if_targetable = [breed, by_who](const PMinion& i){
            return BREED(i) && !i->is_dead() && i->is_targetable(by_who);
          };
          assert(!DEFINED(weapon)); // cannot be both
          if (!DEFINED(enemy))  // not enemy
            append_if(res, by_who->state.minions, if_targetable);
          if (!DEFINED(friendly)) // not friendly
            append_if(res, other_pl->state.minions, if_targetable);
        }
        else {
          const auto if_breed = [breed, by_who](const PMinion& i){
            return BREED(i) && !i->is_dead();
          };
          if (!DEFINED(enemy))  // not enemy
            append_if(res, by_who->state.minions, if_breed);
          if (!DEFINED(friendly)) // not friendly
            append_if(res, other_pl->state.minions, if_breed);
        }
    }

    if (DEFINED(heroes)) {
      if (!DEFINED(enemy) && BREED(by_who->state.hero))  // not enemy
        res.push_back(by_who->state.hero);
      if (!DEFINED(friendly) && BREED(other_pl->state.hero)) // not friendly
        res.push_back(other_pl->state.hero);
    }

    if (DEFINED(attackable)) {
      // remove everything behind a taunt, if any
      assert(!DEFINED(weapon)); // cannot be both
      assert(DEFINED(enemy)); // useless otherwise
      ListPInstance res2;
      for (auto& p : res)
        if (CAST(p.get(), Thing)->is_taunt())
          res2.push_back(p);
      if (res2.size())
        swap(res, res2);  // optimized by STL
    }

    if (DEFINED(weapon)) {
      assert(!(DEFINED((attackable | targetable))));
      if (!DEFINED(enemy))  // not enemy
        if (by_who->state.weapon) res.push_back(by_who->state.weapon);
      if (!DEFINED(friendly)) // not friendy
        if (other_pl->state.weapon) res.push_back(other_pl->state.hero);
    }

    // remove dead instances : already done above
    //for (int i = 0; i < len(res); ++i)
    //  if (CAST(res[i].get(), Thing)->is_dead())
    //    fast_remove(res, i);

    assert(!DEFINED(secret)); // NI
  }

  if (DEFINED(other)) {
    int i = indexP(res, me);
    if (i >= 0) res.erase(res.begin() + i);
  }

  if (DEFINED(random)) {
    PInstance i = engine->random(res);
    res = { i };
  }
  return res;
}

string Target::tostr(bool simple) const {
  string res = simple ? "" : "`";

  if (!simple) {
    if (DEFINED(attackable))
      res += " attackable";
      elif(DEFINED(spell_targetable))
        res += " spell_targetable";
      elif(DEFINED(targetable))
        res += " targetable";
  }

  if (DEFINED(friendly))
    res += " friendly";
  if (DEFINED(enemy))
    res += " enemy";

  if (DEFINED(characters))
    res += " character";
  elif(DEFINED(minions))
    res += " minion";
  elif(DEFINED(heroes))
    res += " hero";
  if (DEFINED(weapon))
    res += " weapon";
  if (DEFINED(secret))
    res += simple ? " random secret": " secret";
  
  if (!simple)  res += "`";
  return res;
}