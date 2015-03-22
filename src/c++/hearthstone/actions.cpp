#include "common.h"
#include "engine.h"
#include "actions.h"
#include "Cards.h"
#include "creatures.h"

Engine* Action::engine = nullptr;

bool Act_EndTurn::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  return engine->end_turn();
}

Act_PlayCard::Act_PlayCard(const Card* card, const bool need_slot, FuncAction actions, Target targets ) :
  Action(card->cost, need_slot, targets), card(card), actions(actions) {}

bool Act_PlayCard::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  return engine->play_card(caster, card, get_cost());
}

const Card_Minion* Act_PlayMinionCard::card_minion() const { 
  return CAST(card, const Card_Minion); 
}

Act_PlayMinionCard::Act_PlayMinionCard(const Card_Minion* card) :
  Act_PlayCard(card, true, nullptr) {}

bool Act_PlayMinionCard::is_valid(const Player* pl) const { 
  return engine->board.get_nb_free_slots(pl)>0; 
}

string Act_PlayMinionCard::tostr() const {
  return string_format("Play minion card action: [%s]", cstr(card)); 
}

bool Act_PlayMinionCard::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  if (Act_PlayCard::execute(caster, choice, slot)) {
    if (actions)  actions(this, caster, choice, slot);
    PMinion m = NEWP(Minion, *card_minion()->minion(), slot.player);
    return engine->add_thing(caster, m, slot);
  }
  return false;
}

string Act_Attack::tostr() const { 
  return string_format( "Attack action by [%s]", cstr(thing)); 
}

bool Act_Attack::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  assert(choice);
  return engine->attack(thing, CAST(choice, Thing));
}


Act_SpellCard::Act_SpellCard(const Card_Spell* card, FuncAction actions, Target targets ) :
  Act_PlayCard(card, false, actions, targets) {}

bool Act_SpellCard::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  if (Act_PlayCard::execute(caster, choice, slot)) {
    engine->signal(caster, Event::StartSpell);
    bool res = actions(this, caster, choice, slot);
    engine->signal(caster, Event::EndSpell);
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

bool Act_HeroPower::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  engine->signal(caster, Event::StartHeroPower);
  bool res = action(this, caster, choice, slot);
  engine->signal(caster, Event::EndHeroPower);
  return res;
}
