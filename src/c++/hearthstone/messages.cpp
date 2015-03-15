#include "common.h"
#include "players.h"
#include "messages.h"
#include "creatures.h"
#include "Cards.h"

string Msg_StartTurn::tostr() const {
  return string_format("Start of turn for %s ", caster->player->name.c_str());
}

string Msg_EndTurn::tostr() const {
  return string_format("End of turn for %s ", caster->player->name.c_str());
}

string Msg_NewCard::tostr() const {
  string from = caster ? caster->tostr() : "the deck";
  return string_format("card %X [%s] is created by %s.", card.get(), card->tostr().c_str(), from.c_str());
}

string Msg_ReceiveCard::tostr() const {
  string from = caster ? caster->tostr() : "the deck";
  return string_format("%s receives card [%s] obtained from %s", player->name.c_str(), card->tostr().c_str(), from.c_str());
}

string Msg_BurnCard::tostr() const {
  string from = caster ? caster->tostr() : "the deck";
  return string_format("%s burn card [%s] obtained from %s", player->name.c_str(), card->tostr().c_str(), from.c_str());
}

string Msg_AddMinion::tostr() const {
  return string_format("New %s on the board for %s", caster->tostr().c_str(), caster->player->name.c_str());
}