#include "creatures.h"
#include "Cards.h"
#include "players.h"
#include "actions.h"

PConstCardHero Hero::card_hero() const { 
  return issubclassP(card, const Card_Hero); 
}

Hero::Hero(int hp):
  Creature(0, hp) {
}

Hero::Hero(PConstCardHero hero) :
  Hero(*hero->hero()) {
  init(hero, nullptr);
  popup();
}

string Hero::tostr() const {
  return string_format("[%s (%s) %dHP]", player->name, card->name, state.hp);
}

ListAction Hero::list_actions() {
  //  ListAction res;
  //  if (state.n_remaining_power)
  //    res += card->ability.list_actions();

  //  if (state.n_atq < state.n_max_atq && state.atq>0 && not is_frozen()) {
  //    res.push_back(make_shared<Act_HeroAttack>(this, engine->board.get_attackable_characters(player)));
  //  }
  //  return res;
  //}
  NI;
  return{};
}

float Hero::score_situation() {
  // healthpoint : 1 at 0, 0.3 at 30
  // sum_i = 1..hp max(0, 1 - 0.0233*i)
  int life = min(42, state.armor + state.hp);
  return ((19767 - 233 * life)*life) / 20000.f;
}


Card_HeroAbility::Card_HeroAbility(int cost, string name, FuncAction actions, Target target) :
  Card(cost, name), action(this, cost, actions, target | Target::targetable ) {}

void Card_HeroAbility::list_actions(ListAction& list) const {
  list.emplace_back(&action);
}


Card_Hero::Card_Hero(string name, HeroClass cls, PHero hero, PCardHeroAbility ability) :
  Card_Thing(0, name, hero), ability(ability) { //issubclassP(hero, Thing)
  collectible = false;
  cls = cls;
  hero->add_static_effect(Thing::charge);
}

string Card_Hero::tostr() const {
  return string_format("Card Hero %s", instance->tostr());
}

void Card_Hero::list_actions(ListAction& list) const {
  NI; 
  //from actions import Act_HeroPower
  //  return[Act_HeroPower(self.owner, self.cost, self.list_targets(self.Target), self.actions)]
}
