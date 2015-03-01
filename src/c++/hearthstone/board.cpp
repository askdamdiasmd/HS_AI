//#include "actions.h"
#include "board.h"
//#include "cards.h"
//#include "collection.h"
#include "creatures.h"
//#include "decks.h"
//#include "effects.h"
#include "engine.h"
//#include "events.h"
//#include "heroes.h"
//#include "messages.h"
#include "players.h"

Engine* Board::engine = nullptr;

Board::Board() {
  assert(0);//everybody = { player1->hero, player2->hero };
}

void Board::add_thing(PInstance thing, Slot pos) {
  if (thing->player->add_thing(thing, pos))
    everybody.push_back(thing);
}

void Board::remove_thing(PInstance m) {
  remove(everybody,m);
  m->player->remove_thing(m);
}

bool Board::is_game_ended() const {
  Player** players = engine->players;
  return players[0]->state.hero->is_dead() || players[1]->state.hero->is_dead();
}

Player* Board::get_enemy_player(Player* me) {
  Player** players = engine->players;
  return players[(me == players[0]) ? 1 : 0];
}


ListMinion Board::get_friendly_minions(Player* player) {
  return ListMinion(player->state.minions); // copy it !!
}

ListMinion Board::get_enemy_minions(Player* player, bool targetable) {
  //// get targetables enemy minions
  //ListMinion res = get_enemy_player(player)->minions;  // copy
  //if (targetable)
  //  remove_if(res,[](const PMinion& m) {return !m->is_untargetable(); });
  //return res;
  assert(0); return{};
}

ListMinion Board::get_minions(Player* player, bool targetable) {
  // get targetables characters
  //ListMinion res = players[0]->minions;
  //res += players[1]->minions;
  //if (targetable)
  //  res.remove_if([](const PMinion& m) {return !m->is_untargetable(); });
  //return res;
  assert(0); return{};
}

ListCreature Board::ListMinion_to_ListCreature(ListMinion minions) {
  ListCreature res(minions.size());
  transform(minions.begin(), minions.end(), res.begin(), [](const PMinion& m) { return dynamic_pointer_cast<Creature>(m); });
  return res;
}

ListCreature Board::get_characters(Player* player, bool targetable) {
  // get targetables characters
  //ListCreature res = ListMinion_to_ListCreature(get_minions(player, targetable));
  //res.push_back(PHero(everybody[0]));
  //res.push_back(PHero(everybody[1]));
  //return res;
  assert(0); return{};
}

ListCreature Board::get_attackable_characters(Player* player) {
  //ListCreature enemies = ListMinion_to_ListCreature(get_enemy_minions(player));
  //ListCreature taunts;
  //copy_if(enemies.begin(), enemies.end(), taunts.begin(), [](const PMinion& m){return m->is_taunt(); });
  //if (!taunts.empty())
  //  return taunts;
  //else {
  //  enemies.emplace_back(get_enemy_player(player)->hero);
  //  return enemies;
  //}
  assert(0); return{};
}

//Board::Target::ResolvedTargets::ResolvedTargets(ListMinion l, Hero* h0, Hero* h1, bool(*cond)(Minion*)) {
//  for (auto i : l)
//    if (!cond || cond(i.get()))
//      objs.push_back(i.get());
//  if (h0) objs.push_back(h0);
//  if (h1) objs.push_back(h1);
//}

//void Board::Target::resolve_targets(Player* owner, Minion* me) {
//  string Target = _targets;
//  bool not_self = false;
//  if (startswith(Target, "other ")) {
//    not_self = true;
//    Target = Target.substr(0, 6);
//  }
//  assert(0); /*
//  if (Target == "self")
//  return ResolvedTarget({ me }); // { shared_ptr<Thing>(this) };
//  else
//  res = engine->board.list_targets(controller, Target);
//
//  bool targetable = false;
//  if (!startswith(Target,"targetable ")) {
//  targetable = true;
//  Target = Target.substr(11, -1);
//  }
//  if (Target == "none")
//  return {PCreature()}; // null creature
//  else if (Target == "nobody")
//  return {};
//  else if (Target == "owner") // different from "self", see Creature.list_targets()
//  return owner; //  = is a player
//  else if (!startswith(Target,"character") == 0)
//  return get_characters(owner, targetable);
//  else if (startswith(Target, "minion"))
//  return get_minions(owner, targetable);
//  else if (startswith(Target, "friendly minion"))
//  return get_friendly_minions(owner);
//  else if (startswith(Target, "friendly beast")) {
//  ListMinion res = get_friendly_minions(owner);
//  res.remove_if([](const PMinion& m){return m->card.cat != "beast"; });
//  return ListMinion_to_ListCreature(res);
//  }
//  else if (Target == "neighbors")
//  return "neighbors"; // special target, resolved at popup time
//  else if (startswith(Target, "enemy weapon"))
//  return self.get_enemy_player(owner)->weapon;
//  else
//  assert(!"error: unknown target");*/
//}

//Board::ListSlot Board::get_free_slots(Player* player) {
//  ListSlot res;
//  if (player->minions.size() < 7) {
//    float* mp = player->minions_pos.data();
//    for (int i = 0; i < player->minions.size(); ++i)
//      res.emplace_back(player, i, (mp[i] + mp[i + 1]) / 2);
//  }
//  return res;
//}

//Board::Slot Board::get_minion_pos(Instance* m) {
//  if (issubclass(m, Minion)) {
//    int index = m->controller->minion_index(m);
//    float rel_index = m.owner.minions_pos[index + 1];
//    return Slot(m->controller, index, rel_index);
//  }
//  else
//    return Slot(m->controller);
//}

float Board::score_situation(Player* player) {
  Player* adv = get_enemy_player(player);
  if (player->state.hero->is_dead())
    return -INF;
  else if (adv->state.hero->is_dead())
    return INF;
  else
    return player->score_situation() - adv->score_situation();
}