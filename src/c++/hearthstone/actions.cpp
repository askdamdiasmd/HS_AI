#include "common.h"
#include "engine.h"
#include "actions.h"
#include "Cards.h"
#include "creatures.h"

Engine* Action::engine = nullptr;

Act_PlayCard::Act_PlayCard(const Card* card, const bool need_slot, FuncAction actions, Target targets ) :
  Action(card->cost, need_slot, targets), card(card), actions(actions) {}

bool Act_PlayCard::execute(PInstance caster, PInstance choice, const Slot& slot) {
  return engine->play_card(card, caster);
}

Act_SpellCard::Act_SpellCard(const Card_Spell* card, FuncAction actions, Target targets ) :
  Act_PlayCard(card, false, actions, targets) {}

bool Act_SpellCard::execute(PInstance caster, PInstance choice, const Slot& slot) {
  if (Act_PlayCard::execute(caster, choice, slot)) {
    engine->signal(Event::StartSpell, caster);
    bool res = actions(this, caster, choice);
    engine->signal(Event::EndSpell, caster);
    return res;
  }
  return false;
}

Act_TargetedSpellCard::Act_TargetedSpellCard(const Card_TargetedSpell* card, FuncAction actions, Target targets) :
Act_SpellCard(card, actions, targets) {
  assert(targets.is_targetable());
}

string Act_TargetedSpellCard::tostr() const {
  return string_format("Play Targeted Spell Card %s", card->tostr().c_str());
}

Act_AreaSpellCard::Act_AreaSpellCard(const Card_AreaSpell* card, FuncAction actions, Target targets) :
Act_SpellCard(card, actions, targets) {
  assert(!targets.is_targetable());
}

string Act_AreaSpellCard::tostr() const {
  return string_format("Play Area Spell Card %s", card->tostr().c_str());
}


string Act_HeroPower::tostr() const {
  return string_format("Hero Power (%d): %s", cost, card->name);
}

bool Act_HeroPower::execute(PInstance caster, PInstance choice, const Slot& slot) {
  engine->signal(Event::StartHeroPower, caster);
  bool res = action(this, caster, choice);
  engine->signal(Event::EndHeroPower, caster);
  return res;
}