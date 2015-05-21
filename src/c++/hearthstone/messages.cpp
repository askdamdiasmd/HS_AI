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

string Msg_Arrow::tostr() const {
  return string_format("[%s] sends an arrow [%c, %s] to [%s]", cstr(caster), ch, color, cstr(target));
}

// hero power message

string Msg_HeroPower::tostr() const {
  return string_format("[%s] uses its hero power", cstr(caster));
}
string Msg_Arrow_HeroPower::tostr() const {
  return string_format("[%s]'s hero power sends an arrow [%c, %s] to [%s]", cstr(caster), ch, color, cstr(target));
}

  // Minion messages
string Msg_AddMinion::tostr() const {
  return string_format("New %s on the board for %s at %s", cstr(caster), caster->player->name.c_str(), cstr(&pos));
}

string Msg_AddWeapon::tostr() const {
  return string_format("[%s] equipped a [%s]", caster->player->name.c_str(), cstr(caster));
}

string Msg_RemoveInstance::tostr() const {
  return string_format("[%s] dies for player [%s]", cstr(caster), caster->player->name.c_str());
}

string Msg_Attack::tostr() const {
  return string_format("[%s] attacks [%s]", cstr(caster()), cstr(target()));
}

string Msg_Damage::tostr() const {
  return string_format("[%s] inflicts %d damages to [%s]", 
    caster ? cstr(caster) : "Fatigue", amount, cstr(target));
}

string Msg_Heal::tostr() const {
  return string_format("[%s] restores %d hps to [%s]", cstr(caster), amount, cstr(target));
}

string Msg_ZoneDamage::tostr() const {
  return string_format("[%s] deals %d damages to zone %s", cstr(caster), amount, zone.tostr().c_str());
}

string Msg_ZoneHeal::tostr() const {
  return string_format("[%s] restores %d hp to zone %s", cstr(caster), amount, zone.tostr().c_str());
}
