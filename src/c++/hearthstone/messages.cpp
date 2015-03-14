#include "common.h"
#include "players.h"
#include "messages.h"
#include "creatures.h"
#include "Cards.h"

string Msg_CardDrawn::tostr() const {
  string from = caster ? caster->tostr() : "the deck";
  return string_format("%s draw %s from %s", player->name.c_str(), card->tostr(), from.c_str());
}