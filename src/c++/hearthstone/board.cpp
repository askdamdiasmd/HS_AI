#include "actions.h"
#include "board.h"
//#include "cards.h"
//#include "collection.h"
#include "creatures.h"
//#include "decks.h"
#include "effects.h"
#include "engine.h"
//#include "events.h"
//#include "heroes.h"
#include "messages.h"
#include "players.h"

Engine* Board::engine = nullptr;

Board::Board(Player* player1, Player* player2) :
  player1(player1), player2(player2) {
  state.turn = 0;
  state.n_dead = 0;
  state.everybody.push_back(player1->state.hero);
  state.everybody.push_back(player2->state.hero);
}

template <>
shared_ptr<Instance> Board::getP<Instance>(Instance* i) {
  if (!i) return nullptr;
  Thing* th = issubclass(i, Thing);
  return th ? CASTP(getP<Thing>(th),Instance) : CASTP(getP<Secret>(CAST(i, Secret)),Instance);
}
template <>
shared_ptr<Thing> Board::getP<Thing>(Instance* i) {
  if (!i) return nullptr;
  return state.everybody[indexP(state.everybody, CAST(i, Thing))];
}
template <>
shared_ptr<Minion> Board::getP<Minion>(Instance* i) {
  if (!i) return nullptr;
  return CASTP(getP<Thing>(i), Minion);
}
template <>
shared_ptr<Weapon> Board::getP<Weapon>(Instance* i) {
  if (!i) return nullptr;
  return CASTP(getP<Thing>(i), Weapon);
}
template <>
shared_ptr<Creature> Board::getP<Creature>(Instance* i) {
  if (!i) return nullptr;
  return CASTP(getP<Thing>(i), Creature);
}
template <>
shared_ptr<Hero> Board::getP<Hero>(Instance* i) {
  if (!i) return nullptr;
  return CASTP(getP<Thing>(i), Hero);
}
template <>
shared_ptr<Secret> Board::getP<Secret>(Instance* i) {
  if (!i) return nullptr;
  return state.secrets[indexP(state.secrets, CAST(i, Secret))];
}

bool Board::is_game_ended() const {
  return players[0]->state.hero->is_dead() || players[1]->state.hero->is_dead();
}
const Player* Board::get_winner() const {
  bool dead0 = players[0]->state.hero->is_dead();
  bool dead1 = players[1]->state.hero->is_dead();
  if (dead0 && !dead1)  return players[1];
  if (!dead0 && dead1)  return players[0];
  return nullptr; // match nul
}

void Board::deal_init_cards() {
  assert(state.turn == 0);
  state.turn = -2; 
  players[0]->draw_init_cards(3, false);
  state.turn = -1;
  players[1]->draw_init_cards(4, true);
  state.turn = 0;
}

ListAction Board::list_player_actions(Player* player) {
  ListAction actions = player->list_actions();
  //actions = filter_actions(actions);  //  filter  actions
  ListAction res;
  for (auto a : actions)
    if (a->is_valid(player) && a->get_cost() <= player->state.mana)
      res.push_back(a);
  return res;
}

bool Board::play_turn() {
  Player* player = start_turn();
  clean_deads();
  engine->wait_for_display();

  bool exit = false;
  while (!is_game_ended() && !exit) {
    ListAction actions = list_player_actions(player);
    Instance* choice = nullptr;
    Slot slot;
    const Action* action = player->choose_actions(actions, choice, slot); // can be Act_EndTurn
    if (!action)  return false; // nullptr, means that we want to break here
    exit = issubclass(action, const Act_EndTurn) != nullptr; // do it before action is destroyed
    action->execute(player->state.hero.get(), choice, slot);
    clean_deads();  // just in case
    engine->wait_for_display();
  }
  state.turn += 1;
  return !is_game_ended();
}

void Board::create_thing(PThing thing) {
  state.everybody.push_back(thing); // do this FIRST
}
bool Board::add_thing(Instance* caster, PThing thing, const Slot& pos) {
  assert(in(thing, state.everybody)); // create_thing MUST be called before

  if (thing->player->add_thing(thing, pos)) { // will send signal
    thing->popup(); // will send signal
    return true;
  }
  else {
    state.everybody.pop_back(); // oops cancel
    return false;
  }
}

void Board::clean_deads() {
  ListPThing all_deads;

  while (true) {
    // process all signals
    process_signals();

    if (is_game_ended())  
      return; // don't care about the rest
    if (!state.n_dead)
      break;  // no need continue
    ListPThing deads;

    // search deads
    for (auto& i : state.everybody)
      if (i->is_dead()) {
        deads.push_back(i);
        state.n_dead--;
      }
    assert(state.n_dead == 0);

    // remove deads from board
    assert(state.dead_pos.empty());
    for (auto& i : deads) 
      // signal Event::RemoveThing
      state.dead_pos[i.get()] = i->player->remove_thing(i);

    // trigger death-rattles once cleaning is done
    for (auto& i : deads) {
      all_deads.push_back(i);
      i->signal_death();  // signal Event::ThingDead, then trigger death_rattles
      remove(state.everybody, i);  // remove from everybody
    }
    state.dead_pos.clear(); // was temporary
  }

  // finally 
  for (auto& i : all_deads) 
    i->silence(true); // remove auras, through Eff_Aura::undo(die=true)
}

ListSlot Board::get_free_slots(Player* player) const {
  ListSlot res;
  if (player->state.minions.size() < 7) {
    const auto& mp = player->state.minions_pos;
    for (int i = 0; i < len(mp) - 1; ++i)
      res.emplace_back(player, i, (mp[i] + mp[i + 1]) / 2);
  }
  return res;
}

int Board::get_nb_free_slots(const Player* player) const {
  return 7 - len(player->state.minions);
}


Slot Board::get_minion_pos(Instance* i) {
  Minion* m = issubclass(i, Minion);
  if (m) {
    Player* pl = m->player;
    int pos = indexP(pl->state.minions, m);
    if (pos < 0) {
      Slot slot = state.dead_pos.at(m);
      slot.pos = -1;  // signal that it's not here anymore
      return slot;
    } else {
      float rel_index = pl->state.minions_pos[pos + 1];
      return Slot(pl, pos, rel_index);
    }
  }
  else
    return Slot(m->player);
}

float Board::score_situation(Player* player) {
  Player* adv = get_other_player(player);
  if (player->state.hero->is_dead())
    return -INF;
  else if (adv->state.hero->is_dead())
    return INF;
  else
    return player->score_situation() - adv->score_situation();
}

// Game actions ----------------------------

Player* Board::start_turn() {
  Player* player = get_current_player();
  for (auto& i : state.everybody)
    i->start_turn(player);
  signal(player->state.hero.get(), Event::StartTurn);
  player->start_turn();
  return player;
}
bool Board::end_turn() {
  Player* player = get_current_player();
  for (auto& i : state.everybody)
    i->end_turn(player);
  signal(player->state.hero.get(), Event::EndTurn);
  player->end_turn();
  return true;
}

bool Board::draw_card(Instance* caster, Player* player, int nb) {
  while (nb-->0)
    player->draw_card(caster);
  return true;
}

bool Board::play_card(Instance* caster, const Card* _card, int cost) {
  Player* player = caster->player;
  PCard card = findP(player->state.cards, _card);
  player->throw_card(card);
  return true;
}

void Board::register_trigger(const Effect* eff, int ev) {
  if (is_power_of_2(ev))
    state.triggers[Event(ev)].push_back(eff);
  else {  // multiple bits are active
    for (int i = 1; ev; i <<= 1, ev >>= 1)
      if (ev & 1) // bit i is active
        state.triggers[Event(i)].push_back(eff);
  }
}
void Board::unregister_trigger(const Effect* eff, int ev) {
  if (is_power_of_2(ev))
    remove(state.triggers[Event(ev)], eff);
  else {  // multiple bits are active
    for (int i = 1; ev; i <<= 1, ev >>= 1)
      if (ev & 1) // bit i is active
        remove(state.triggers[Event(i)], eff);
  }
}

void Board::signal(Instance* caster, Event event, Creature** target, int nb) {
  if (event & (Event::EndSpell | Event::EndHeroPower | Event::EndAttack))
    // after completion of a game phase, we clean up everything
    clean_deads();

  waiting_signals.emplace(event, caster, target, nb);
}
void Board::signal_now(Instance* caster, Event event, Creature** target, int nb) {
  signal(caster, event, target, nb);
  process_signals();
}
void Board::process_signals() {
  while(!waiting_signals.empty()) {
    Signal s = waiting_signals.front();
    waiting_signals.pop();

    assert(is_power_of_2(s.event));
    for (auto& e : state.triggers[s.event])
      if (e->is_triggered(e, s)) {
        // check that trigger is not dead
        assert(!issubclass(e->owner, Thing) || !issubclass(e->owner, Thing)->is_dead());
        e->trigger(s);
      }
  }
}

bool Board::heal(Instance* from, int hp, Instance* _to) {
  if (from && from->player->state.auchenai)
    return damage(from, hp, _to);
  Thing* to = CAST(_to, Thing);
  int n = to->heal(hp, from);
  return n>0;
}
bool Board::damage(Instance* from, int hp, Instance* _to) {
  Thing* to = CAST(_to, Thing);
  int n = to->hurt(hp, from);
  return n>0;
}

bool Board::HeroHeal(Instance* from, int hp, Instance* to) {
  if (from && from->player->state.auchenai)
    return HeroDamage(from, hp, to);
  if (from) // velen mutliplier
    hp *= (1 << from->player->state.velen);
  return heal(from, hp, to);
}
bool Board::HeroDamage(Instance* from, int hp, Instance* to) {
  if (from) // velen mutliplier
    hp *= 1 << from->player->state.velen;
  return damage(from, hp, to);
}

bool Board::SpellHeal(Instance* from, int hp, Instance* to) {
  if (from && from->player->state.auchenai)
    return SpellDamage(from, hp, to);
  if (from) // velen mutliplier
    hp *= 1 << from->player->state.velen;
  return heal(from, hp, to);
}
bool Board::SpellDamage(Instance* from, int hp, Instance* to) {
  if (from) {
    hp += from->player->state.spell_power; // spell power
    hp *= 1 << from->player->state.velen; // velen mutliplier
  }
  return damage(from, hp, to);
}

bool Board::heal_zone(Instance* from, int hp, Target zone) {
  ListPInstance creatures = zone.resolve(from->player, from);
  SEND_DISPLAY_MSG(Msg_ZoneHeal, GETP(from), zone, hp);
  for (auto& i : creatures) {
    Creature* creature = CAST(i.get(), Creature);
    if (!creature->is_dead())
      creature->heal(hp, from);
  }
  return true;
}
bool Board::damage_zone(Instance* from, int hp, Target zone) {
  ListPInstance creatures = zone.resolve(from->player, from);
  SEND_DISPLAY_MSG(Msg_ZoneDamage, GETP(from), zone, hp);
  for (auto& i : creatures) {
    Creature* creature = CAST(i.get(), Creature);
    if (!creature->is_dead())
      creature->hurt(hp, from);
  }
  return true;
}
bool Board::SpellHeal_zone(Instance* from, int hp, Target zone) {
  if (from && from->player->state.auchenai)
    return SpellDamage_zone(from, hp, zone);
  if (from) // velen mutliplier
    hp *= 1 << from->player->state.velen;
  return heal_zone(from, hp, zone);
}
bool Board::SpellDamage_zone(Instance* from, int hp, Target zone) {
  if (from) {
    hp += from->player->state.spell_power; // spell power
    hp *= 1 << from->player->state.velen; // velen mutliplier
  }
  return damage_zone(from, hp, zone);
}


bool Board::attack(Creature* from, Creature* target) {
  assert(from && target);
  signal(from, Event::StartAttack, &target);
  if (from->attack(target)) {
    signal(from, Event::EndAttack);
    return true;
  }
  else
    return false;
}





//def save_state(num = 0)) {
//self.saved[num] = dict(minions = list(self.minions), state.everybody = list(self.state.everybody))
//for pl in self.players) {
//  pl.save_state(num)
//  for obj in self.state.everybody) {
//    obj.save_state(num)
//    def restore_state(num = 0)) {
//    self.__dict__.update(self.saved[num])
//    self.state.everybody = list(self.state.everybody)
//    self.minions = list(self.minions)
//    for pl in self.players) {
//      pl.restore_state(num)
//      for obj in self.state.everybody) {
//        obj.restore_state(num)
//        def hash_state()) {
//        return 0 # todo
//        def end_simulation()) {
//        self.saved = dict()
//        for pl in self.players) {
//          pl.end_simulation()
//          for obj in self.state.everybody) {
//            obj.end_simulation()
