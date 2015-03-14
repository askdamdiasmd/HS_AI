#include "collection.h"
#include "engine.h"
#include "Cards.h"
#include "creatures.h"
#include "actions.h"

Collection Collection::only_instance = Collection();

Collection::Collection() {
  PMinion minion = NEWP(Minion, 4, 5);
  add(NEWP(Card_Minion, 4, "Windchill Yeti", minion));

  // add all heroes
  PCard Lesser_Heal = add(NEWP(Card_HeroAbility, 2, "Lesser heal", 
    [](const Action* a, PInstance me, PInstance i){return a->engine->heal(me, 2, i);}, Target::characters));
  Lesser_Heal->set_desc("Restore 2 Health")
             ->set_collectible(false);

  PCard Fireblast = add(NEWP(Card_HeroAbility, 2, "Fireblast",
    [](const Action* a, PInstance me, PInstance i){return a->engine->damage(me, 1, i);}, Target::characters));
  Fireblast->set_desc("Deal 1 damage")
           ->set_collectible(false);

  add(NEWP(Card_Hero, "Anduin Wrynn", Card::HeroClass::Priest, NEWP(Hero, 30),
    dynamic_pointer_cast<Card_HeroAbility>(Lesser_Heal)))
          ->set_collectible(false);

  add(NEWP(Card_Hero, "Jaina Proudmoore", Card::HeroClass::Mage, NEWP(Hero, 30),
    dynamic_pointer_cast<Card_HeroAbility>(Fireblast)))
          ->set_collectible(false);

  // create collectible card list
  for (auto c : by_id)
    if(c->collectible)
      collectibles.push_back(c);
}

static string lower(const string& str) {
  string res;
  for (char c : str)
    res += char(tolower(c));
  return res;
}

PCard Collection::add(PCard card) {
  const string & name = card->name;

  //name = "%s-%d-%d-%d" % (name, card.cost, card.atq, card.hp)

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