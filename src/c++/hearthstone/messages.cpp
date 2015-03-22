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

string Msg_ThingUpdate::tostr() const {
  return string_format("Change of %s for [%s]", what, caster->tostr().c_str());
}

string Msg_PlayerUpdate::tostr() const {
  return string_format("Change of %s for player %s", what, caster->player->name.c_str());
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

string Msg_ThrowCard::tostr() const {
  return string_format("Player [%s] plays card [%s]", caster->player->name.c_str(), cstr(card));
}

string Msg_AddMinion::tostr() const {
  return string_format("New %s on the board for %s at %s", cstr(caster), caster->player->name.c_str(), cstr(&pos));
}

string Msg_Damage::tostr() const {
  return string_format("[%s] inflicts %d damages to [%s] ", cstr(caster), amount, cstr(target));
}

string Msg_Heal::tostr() const {
  return string_format("[%s] restores %d hps to [%s] ", cstr(caster), cstr(target), amount);
}
