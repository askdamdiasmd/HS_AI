#include "actions.h"
#include "board.h"
#include "cards.h"
#include "collection.h"
#include "creatures.h"
#include "decks.h"
#include "effects.h"
#include "engine.h"
#include "events.h"
#include "messages.h"
#include "players.h"

Engine* Card::engine = nullptr;

Card_Thing* Card_Thing::add_battlecry(PAct_Battlecry bc) {
  assert(!battlecry);
  battlecry = bc;
  desc = bc->tostr() + " " + desc;
  desc_fr = bc->tostr() + " " + desc_fr;
  return this;
}

Card_Thing* Card_Thing::add_effect(PEffect eff) {
  eff->bind_to(eff, const_cast<Instance*>(instance.get()));
  return this;
}

Card_Thing* Card_Thing::add_spell_powert(int power) {
  CAST(const_cast<Instance*>(instance.get()), Thing)->state.spell_power += power;
  return this;
}


void Card_Minion::list_actions(ListAction& list) const {
  list.push_back(&act_play);
}

string Card_Minion::tostr() const {
  const auto& s = minion()->state;
  return string_format("%s (%d): %d/%d", name_fr.c_str(), cost, s.atq, s.hp);
}

PMinion Card_Minion::instanciate(Player* owner) const {
  PMinion res = NEWP(Minion, *minion(), owner);
  engine->board.create_thing(res);
  return res;
}

void Card_Weapon::list_actions(ListAction& list) const {
  list.push_back(&act_play);
}

string Card_Weapon::tostr() const {
  const auto& s = weapon()->state;
  return string_format("Weapon %s (%d): %d/%d %s", name_fr.c_str(), cost, s.atq, s.hp, desc.c_str());
}

PWeapon Card_Weapon::instanciate(Player* owner) const {
  PWeapon res = NEWP(Weapon, *weapon(), owner);
  engine->board.create_thing(res);
  return res;
}

Card_Coin::Card_Coin() :
  Card_AreaSpell(0, "The coin", FUNCACTION {from->player->gain_mana(1); return true; }) {
  set_collectible(false);
  set_name_fr("La piece");
  set_desc("Gain one mana crystal this turn only");
}

PCard Card_Coin::copy() const {
  return NEWP(Card_Coin);
}
