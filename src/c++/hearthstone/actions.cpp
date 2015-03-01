#include "common.h"
#include "engine.h"
#include "actions.h"
#include "Cards.h"
#include "heroes.h"
#include "creatures.h"

Engine* Action::engine = nullptr;


string Act_HeroPower::tostr() const {
  return string_format("Hero Power (%d): %s", cost, card->name);
}

void Act_HeroPower::execute(PInstance caster, PInstance choice, Slot slot) {
  engine->signal(Event::StartHeroPower, caster);
  action(engine, caster, choice);
  engine->signal(Event::EndHeroPower, caster);
}