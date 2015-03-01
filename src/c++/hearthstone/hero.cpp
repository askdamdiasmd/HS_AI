#include "creatures.h"
#include "Cards.h"
#include "players.h"
#include "heroes.h"
#include "actions.h"

Hero::Hero(int hp):
  Creature(0, hp) {
  NI; //popup();
}

//Hero::Hero(PCardHero hero, Player* controller) :
//  Hero(*dynamic_pointer_cast<Hero>(hero->instance)) {
//  init(hero, controller);
//  assert(0);//popup();
//}

void Hero::set_controller(Player* pl) {
  player = pl;
  card->player = pl;
  assert(0);//dynamic_cast<Card_Hero*>(card.get())->ability.controller = pl;
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
  Card_Instance(0, name, dynamic_pointer_cast<Instance>(hero)), ability(ability) {
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
