#include "actions.h"
#include "board.h"
#include "cards.h"
#include "collection.h"
#include "creatures.h"
#include "decks.h"
#include "effects.h"
#include "engine.h"
#include "events.h"
#include "heroes.h"
#include "messages.h"
#include "players.h"

Engine* Card::engine = nullptr;

// helper function to resovle a target at runtime
ListCreature Card::list_targets(const string& Target) {
  assert(0);
  return{};// engine->board.list_targets(owner, Target);
}

void Card_Minion::list_actions(ListAction& list) const {
  NI;// Act_PlayMinionCard(this);
}

string Card_Minion::tostr() const {
  const auto& s = dynamic_pointer_cast<Minion>(instance)->state;
  return string_format("%s (%d): %d/%d %s", name_fr.c_str(), cost, s.atq, s.hp, desc.c_str());
}