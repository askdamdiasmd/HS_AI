#include "collection.h"
#include "engine.h"
#include "Cards.h"
#include "creatures.h"
#include "actions.h"
#include "effects.h"
#include "messages.h"

Collection Collection::only_instance = Collection();

Collection::Collection() {
  // All heroes -------------------------------------------------------

  PCard Lesser_Heal = add(NEWP(Card_HeroAbility, 2, "Lesser heal", 
    FUNCACTION{ Engine* engine = a->engine;
                SEND_DISPLAY_MSG(Msg_Arrow_HeroPower, GETP((Hero*)from), GETP((Thing*)target), '+', "GREEN_on_BLACK");
                return a->engine->HeroHeal(from, 2, target); }, TGT::characters));
  Lesser_Heal->set_desc("Restore 2 Health")
             ->set_collectible(false);

  PCard Fireblast = add(NEWP(Card_HeroAbility, 2, "Fireblast",
    FUNCACTION{ Engine* engine = a->engine;
                SEND_DISPLAY_MSG(Msg_Arrow_HeroPower, GETP((Hero*)from), GETP((Thing*)target), '*', "RED_on_BLACK");
                return a->engine->HeroDamage(from, 1, target); }, TGT::characters));
  Fireblast->set_desc("Deal 1 damage")
           ->set_collectible(false);

  add(NEWP(Card_Hero, "Anduin Wrynn", Card::HeroClass::Priest, NEWP(Hero, 30),
    dynamic_pointer_cast<Card_HeroAbility>(Lesser_Heal)))
          ->set_collectible(false);

  add(NEWP(Card_Hero, "Jaina Proudmoore", Card::HeroClass::Mage, NEWP(Hero, 30),
    dynamic_pointer_cast<Card_HeroAbility>(Fireblast)))
          ->set_collectible(false);

  // All minions -------------------------------------------------------
  #define MINION(atq, hp, ...) NEWP(Minion, atq, hp, ##__VA_ARGS__)
  #define ADDM(cost, name, atq, hp, ...) add(NEWP(Card_Minion, cost, name, MINION(atq,hp,##__VA_ARGS__)))
  #define minion_by_name(name)  CASTP(by_name.at("Spectral Spider"), const Card_Minion)

  ADDM(1, "Spectral Spider", 1, 1)->set_collectible(false)
    ->set_name_fr("Araignee spectrale");

  ADDM(1, "Leper Gnome", 2, 1)
    ->add_effect(NEWP(Eff_DR_Damage, TGT::enemy | TGT::heroes, 2))
    ->set_name_fr("Gnome lepreux")
    ->set_desc("Deathrattle: deal 2 damage to the enemy Hero.")
    ->set_desc_fr("Rale d'agonie : inflige 2 points de degats au heros adverse.");

  ADDM(2, "Haunted Creeper", 1, 2, 0, Creature::Beast)
    ->add_effect(NEWP(Eff_DR_Invoke_Minion,minion_by_name("Spectral Spider")))
    ->add_effect(NEWP(Eff_DR_Invoke_Minion, minion_by_name("Spectral Spider")))
    ->set_name_fr("Rampante Hantee")
    ->set_desc_fr("Rale d'agonie: Invoque 2 Araignees spectrales 1/1");

    //add(Card_Minion(3, 4, 4, "Nerubian", name_fr = "Nerubien", collectible = False))
    //add(Card_Minion(2, 0, 2, "Nerubian Egg", effects = [Eff_DR_Invoke_Minion(cardbook["Nerubian"])],
    //name_fr = "Oeuf de Nerubien", desc_fr = "Rale d'agonie: Invoque un Nerubien 4/4", ))

  ADDM(1, "Argent Squire", 1, 1, Thing::divine_shield)
    ->set_name_fr("Ecuyere d'argent");

  ADDM(1, "Elven Archer", 1, 1)
    ->add_battlecry(NEWP(Act_BC_Damage, Target(TGT::targetable | TGT::characters), 1))
    ->set_name_fr("Archere elfe");

  ADDM(1, "Zombie chow", 2, 3)
    ->add_effect(NEWP(Eff_DR_Heal, TGT::enemy | TGT::heroes, 5))
    ->set_name_fr("Croq'zombie")
    ->set_desc("Deathrattle: restore 5 Health to the enemy Hero.")
    ->set_desc_fr("Rale d'agonie : rend 5 PV au heros adverse.");

  ADDM(2, "Dire Wolf Alpha", 2, 2, 0, Creature::Beast)
    ->add_effect(NEWP(Eff_Aura_Ngh, 1, 0))
    ->set_name_fr("Loup Alpha redoutable")
    ->set_desc("Adjacent minions have + 1 Attack.")
    ->set_desc_fr("Les serviteurs adjacents ont + 1 ATQ");

  ADDM(2, "Unstable Ghoul", 1, 3, EFF::taunt)
    ->add_effect(NEWP(Eff_DR_ZoneDamage, TGT::minions, 1))
    ->set_name_fr("Goule instable")
    ->set_desc("Taunt. Deathrattle: deal 1 damage to all minions.")
    ->set_desc_fr("Provocation. Rale d'agonie : inflige 1 point de degats a tous les serviteurs.");

  ADDM(3, "Shattered Sun Cleric", 3, 2)
    ->add_battlecry(NEWP(Act_BC_Buff, "Give a friendly minion +1/+1.",
      TGT::targetable | TGT::friendly | TGT::minions, 1, 1))
    ->set_name_fr("Clerc du Soleil brise")
    ->set_desc_fr("Cri de guerre: confere +1/+1 a un serviteur allie");

  ADDM(4, "Auchenai Soulpriest", 3, 5)
    ->add_effect(NEWP(Eff_Presence, 
      FUNCEFFECT{ caster->player->state.auchenai++; return true; },
      FUNCEFFECT{ caster->player->state.auchenai--; return true; }))
    ->set_cls(Card::Priest)
    ->set_name_fr("Pretresse auchenai")
    ->set_desc("Your cards and powers that restore Health now deal damage instead.")
    ->set_desc_fr("Vos cartes et pouvoirs rendant de la Vie infligent desormais des degats a la place.");

  ADDM(4, "Defender of Argus", 2, 3)
    ->add_battlecry(NEWP(Act_BC_Ngh,"Give adjacent minions +1/+1 and Taunt.", 1, 1, EFF::taunt))
    ->set_name_fr("Defenseur d'Argus");

  ADDM(4, "Gnomish Inventor", 2, 4)
    ->add_battlecry(NEWP(Act_BC_DrawCard, TGT::friendly | TGT::heroes, 1))
    ->set_name_fr("Inventrice gnome");

  ADDM(4, "Windchill Yeti", 4, 5)
    ->set_name_fr("Yeti Noroit");

  //add(NEWP(Card_Minion, 4, "Windchill Yeti", NEWP(Minion, 4, 5)));

  #undef MINION
  #undef ADDM

  // All weapons -------------------------------------------------------
  #define WEAPON(atq, hp, ...) NEWP(Weapon, atq, hp, ##__VA_ARGS__)
  #define ADDW(cost, name, atq, hp, ...) add(NEWP(Card_Weapon, cost, name, WEAPON(atq,hp,##__VA_ARGS__)))

  ADDW(1, "Heavy Axe", 1, 3)->set_cls(Card::Warrior)
    ->set_name_fr("Hache lourde");

  ADDW(2, "Fiery War Axe", 3, 2)->set_cls(Card::Warrior)
    ->set_name_fr("Hache de guerre embrasee");

  #undef WEAPON
  #undef ADDW

  // create collectible card list
  for (auto c : by_id)
    if(c->collectible)
      collectibles.push_back(c);
}

template <typename TCard>
TCard Collection::add(TCard card) {
  const string & name = card->name;
  //name = "%s-%d-%d-%d" % (name, card.cost, card.atq, card.hp)

  // remember pointer to card
  PConstCardInstance ci = issubclassP(card, Card_Instance);
  if (ci) {
    CONSTCAST(ci->instance.get(), Instance)->init(ci, nullptr);
    PConstCardThing ct = issubclassP(card, Card_Thing);
    if (ct) {
      int st = ct->thing()->state.static_effects;
      if (ct->desc.empty()) {
        string res;
        if (st & EFF::untargetable) res += "Can't be targeted by spells or Hero Powers. ";
        if (st & EFF::freezer) res += "Freeze any character damaged by this minion. ";
        if (st & EFF::windfury) res += "Windfury. ";
        if (st & EFF::charge) res += "Charge. ";
        if (st & EFF::divine_shield) res += "Divine Shield. ";
        if (st & EFF::stealth) res += "Stealth. ";
        if (st & EFF::taunt) res += "Taunt. ";
        CONSTCAST(ci.get(), Card_Instance)->set_desc(res);
      }
      if (ct->desc_fr.empty()) {
        string res;
        if (st & EFF::untargetable) res += "Ne peut etre la cible de sorts ou de pouvoirs heroiques. ";
        if (st & EFF::freezer) res += "Gele tout personnage blesse par ce serviteur. ";
        if (st & EFF::windfury) res += "Furie des vens. ";
        if (st & EFF::charge) res += "Charge. ";
        if (st & EFF::divine_shield) res += "Bouclier divin. ";
        if (st & EFF::stealth) res += "Camouflage. ";
        if (st & EFF::taunt) res += "Provocation. ";
        CONSTCAST(ci.get(), Card_Instance)->set_desc(res);
      }
    }
  }

  const int card_count = by_id.size();
  card->id = card_count;
  by_id.push_back(card);

  assert(!in(name, by_name));
  by_name[name] = by_name[lower(name)] = card;

  const string& name_fr = card->name_fr;
  if (!name_fr.empty() && name_fr != name) {
    assert(!in(name_fr, by_name));
    by_name[name_fr] = by_name[lower(name_fr)] = card;
  }

  return card;
}

PConstCardHero Collection::get_hero(const string& name) const {
  return CASTP(get_by_name(name), const Card_Hero);
}
PConstCardMinion Collection::get_minion(const string& name) const {
  return CASTP(get_by_name(name), const Card_Minion);
}
PConstCardWeapon Collection::get_weapon(const string& name) const {
  return CASTP(get_by_name(name), const Card_Weapon);
}
PConstCardSecret Collection::get_secret(const string& name) const {
  NI; 
  return nullptr;//return CASTP(get_by_name(name), const Card_Secret);
}