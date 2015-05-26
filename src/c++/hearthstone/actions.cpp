#include "common.h"
#include "engine.h"
#include "actions.h"
#include "Cards.h"
#include "creatures.h"
#include "players.h"
#include "messages.h"

Engine* Action::engine = nullptr;

bool Action::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  caster->player->use_mana(get_cost());
  return true;
}

bool Act_EndTurn::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  return engine->board.end_turn();
}

Act_PlayCard::Act_PlayCard(const Card* card, const bool need_slot, FuncAction actions, Target targets ) :
  Action(card->cost, need_slot, targets), card(card), actions(actions) {}

bool Act_PlayCard::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  return Action::execute(caster, choice, slot) &&
    engine->board.play_card(caster, card, get_cost());
}

const Card_Minion* Act_PlayMinionCard::card_minion() const { 
  return CAST(card, const Card_Minion); 
}

const FuncAction RunBattlecry = FUNCACTION{
  const Card_Thing* card_thing = CAST(CAST(a, const Act_PlayMinionCard)->card, const Card_Thing);
  card_thing->battlecry->execute(from, target, slot);
  return true;
};

Act_PlayMinionCard::Act_PlayMinionCard(const Card_Minion* card) :
  Act_PlayCard(card, true, card->battlecry ? RunBattlecry : nullptr, 
               card->battlecry ? card->battlecry->target : 0) {}

bool Act_PlayMinionCard::is_valid(const Player* pl) const { 
  return engine->board.get_nb_free_slots(pl)>0; 
}

string Act_PlayMinionCard::tostr() const {
  return string_format("Action Play-minion-card: [%s]", cstr(card)); 
}

bool Act_PlayMinionCard::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  if (Act_PlayCard::execute(caster, choice, slot)) {
    PMinion m = card_minion()->instanciate(slot.player);
    if (actions)  actions(this, m.get(), choice, slot);
    return engine->board.add_thing(caster, m, slot);
  }
  return false;
}

// Battlecries -------------

static ListInstance resolve_targets(const Action* a, Instance* from, Instance* target, const Slot& slot) {
  ListInstance targets;
  if (a->need_target()) {
    if (target)
      targets.push_back(target);
  }
  else
    for (auto& i : a->target.resolve(from->player, from, slot))
      targets.push_back(i.get());
  return targets;
}

Act_BC_Damage::Act_BC_Damage(Target target, int damage) :
  Act_Battlecry(FUNCACTION{ 
  Engine* engine = a->engine;
  SEND_DISPLAY_MSG(Msg_Arrow, GETPT(from,Thing), GETPT(target,Thing), '|', "WHITE_on_BLACK");
  return a->engine->board.damage(from, CAST(a, const Act_BC_Damage)->damage, target); 
  }, target), damage(damage) {}

string Act_BC_Damage::tostr() const  {
  return string_format("Battlecry: deal %d damage to a%s", damage, target.tostr(true).c_str());
}

Act_BC_Buff::Act_BC_Buff(string desc, Target target, int atq, int hp, int static_eff) :
Act_Battlecry(FUNCACTION{
  const Act_BC_Buff* me = CAST(a, const Act_BC_Buff);
  for (auto& i : resolve_targets(a, from, target, slot)) {
    Thing* thing = CAST(i, Thing);
    if (me->hp) thing->change_hp(me->hp);
    if (me->atq) thing->change_atq(me->atq);
    if (me->static_effects) thing->add_static_effect(Thing::StaticEffect(me->static_effects));
  }
  return true;
}, target), desc(desc), atq(atq), hp(hp), static_effects(static_eff) {}

string Act_BC_Buff::tostr() const  {
  return "Battlecry: "+desc;
}

Act_BC_DrawCard::Act_BC_DrawCard(Target target, int nb) 
  :Act_Battlecry(FUNCACTION{
  const Act_BC_DrawCard* me = CAST(a, const Act_BC_DrawCard);
  for (auto& i : resolve_targets(a, from, target, slot))
    a->engine->board.draw_card(from, i->player, me->nb_card);
  return true;
}, target), nb_card(nb) {}

string Act_BC_DrawCard::tostr() const {
  return string_format("Battlecry: %s draw %d card.", target.tostr(true).c_str(), nb_card);
}



// Attack actions ----------

string Act_Attack::tostr() const { 
  return string_format("Action Attack by [%s]", cstr(creature));
}

bool Act_Attack::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  assert(choice);
  return engine->board.attack(creature, CAST(choice, Creature));
}


/// Weapons --------------

Act_PlayWeaponCard::Act_PlayWeaponCard(const Card_Weapon* card) :
  Act_PlayCard(card, false, nullptr) {}

string Act_PlayWeaponCard::tostr() const {
  return string_format("Action Play-weapon-card: [%s]", cstr(card));
}

const Card_Weapon* Act_PlayWeaponCard::card_weapon() const {
  return CAST(card, const Card_Weapon);
}

bool Act_PlayWeaponCard::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  if (Act_PlayCard::execute(caster, choice, slot)) {
    if (actions)  actions(this, caster, choice, slot);
    PWeapon m = card_weapon()->instanciate(caster->player);
    return engine->board.add_thing(caster, m, slot);
  }
  return false;
}


/// Spells --------------

Act_SpellCard::Act_SpellCard(const Card_Spell* card, FuncAction actions, Target targets ) :
  Act_PlayCard(card, false, actions, targets) {}

bool Act_SpellCard::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  if (Act_PlayCard::execute(caster, choice, slot)) {
    engine->board.signal(caster, Event::StartSpell);
    bool res = actions(this, caster, choice, slot);
    engine->board.signal(caster, Event::EndSpell);
    return res;
  }
  return false;
}

Act_TargetedSpellCard::Act_TargetedSpellCard(const Card_TargetedSpell* card, FuncAction actions, Target targets) :
Act_SpellCard(card, actions, targets) {
  assert(targets.is_targetable());
}

string Act_TargetedSpellCard::tostr() const {
  return string_format("Action Play-targeted-spell-Card: [%s]", card->tostr().c_str());
}

Act_AreaSpellCard::Act_AreaSpellCard(const Card_AreaSpell* card, FuncAction actions, Target targets) :
Act_SpellCard(card, actions, targets) {
  assert(!targets.is_targetable());
}

string Act_AreaSpellCard::tostr() const {
  return string_format("Action Play-Area-Spell-Card: [%s]", card->tostr().c_str());
}


/// Hero power --------------

string Act_HeroPower::tostr() const {
  return string_format("Action Hero-Power (%d): [%s]", cost, card->name);
}

bool Act_HeroPower::execute(Instance* caster, Instance* choice, const Slot& slot) const {
  Action::execute(caster, choice, slot);
  engine->board.signal(caster, Event::StartHeroPower);
  CAST(caster, Hero)->use_hero_power();
  bool res = action(this, caster, choice, slot);
  engine->board.signal(caster, Event::EndHeroPower);
  return res;
}
