#include "actions.h"
#include "board.h"
#include "cards.h"
#include "collection.h"
#include "creatures.h"
#include "decks.h"
#include "effects.h"
#include "engine.h"
#include "events.h"
#include "messages.h"
#include "players.h"
#include "effects.h"


void Engine::set_default()  {
  // init global variables: everyone  can  access  board  or  send  messages
  Board::set_engine(this);
  Target::set_engine(this);
  Instance::set_engine(this);
  Action::set_engine(this);
  Effect::set_engine(this);
  Card::set_engine(this);
  Deck::set_engine(this);
  //Message::set_engine(this);
  Player::set_engine(this);
}

PInstance Engine::random(ListPInstance& instances) {
  // default implementation
  if (len(instances))
    return instances[randint(0, len(instances) - 1)];
  else
    return PInstance();
}

ListAction Engine::list_player_actions(Player* player) {
  ListAction actions = player->list_actions();
  //actions = filter_actions(actions);  //  filter  actions
  ListAction res; 
  for (auto a : actions)
    if (a->is_valid(player) && a->get_cost() <= player->state.mana)
      res.push_back(a);
  return res;
}

void Engine::play_turn() {
  Player* player = start_turn();
  board.clean_deads();
  wait_for_display();

  bool exit = false;
  while (!is_game_ended() && !exit) {
    ListAction actions = list_player_actions(player);
    Instance* choice = nullptr;
    Slot slot;
    const Action* action = player->choose_actions(actions, choice, slot); // can be Act_EndTurn
    exit = issubclass(action, const Act_EndTurn) != nullptr; // do it before action is destroyed
    action->execute(player->state.hero.get(), choice, slot);
    board.clean_deads();  // just in case
    wait_for_display();
  }
  board.state.turn += 1;
}

bool Engine::is_game_ended() const {
  return board.is_game_ended();
}

const Player* Engine::get_winner() const {
  return board.get_winner();
}


// Game actions ----------------------------

Player* Engine::start_turn() {
  Player* player = board.get_current_player();
  board.start_turn(player); // exec first to remove insensible
  signal(player->state.hero.get(), Event::StartTurn);
  player->start_turn();
  return player;
}
bool Engine::end_turn() {
  Player* player = board.get_current_player();
  board.end_turn(player);
  signal(player->state.hero.get(), Event::EndTurn);
  player->end_turn();
  return true;
}

bool Engine::draw_card(Instance* caster, Player* player, int nb ) {
  while (nb-->0)
    player->draw_card(caster);
  return true;
}

bool Engine::play_card(Instance* caster, const Card* _card, int cost) {
  Player* player = caster->player;
  PCard card = findP(player->state.cards, _card);  
  player->throw_card(card);
  return true;
}

bool Engine::add_thing(Instance* caster, PThing thing, const Slot& slot) {
  bool res = board.add_thing(thing, slot);
  return res;
}
bool Engine::add_secret(Instance* caster, PSecret secret) {
  NI;
  return true;
}

inline static bool is_power_of_2(const long v) {
  return  v && !(v & (v - 1));
}
void Engine::register_trigger(Effect* eff, int ev) {
  if (is_power_of_2(ev))
    board.state.triggers[Event(ev)].push_back(eff);
  else {  // multiple bits are active
    for (int i = 1; ev; i <<= 1, ev >>= 1)
      if (ev & 1) // bit i is active
        board.state.triggers[Event(i)].push_back(eff);
  }
}
void Engine::unregister_trigger(Effect* eff, int ev) {
  if (is_power_of_2(ev))
    remove(board.state.triggers[Event(ev)], eff);
  else {  // multiple bits are active
    for (int i = 1; ev; i <<= 1, ev >>= 1)
      if (ev & 1) // bit i is active
        remove(board.state.triggers[Event(i)], eff);
  }
}
void Engine::signal(Instance* caster, Event event) {
  assert(is_power_of_2(event));
  for (auto& e : board.state.triggers[event])
    if (e->is_triggered(e, event, caster)) {
      // check that trigger is not dead
      assert(!issubclass(e->owner, Thing) || !issubclass(e->owner, Thing)->is_dead());
      e->trigger(event, caster);
    }
}

bool Engine::heal(Instance* from, int hp, Instance* _to) {
  if (from && from->player->state.auchenai)
    return damage(from, hp, _to);
  Thing* to = CAST(_to, Thing);
  int n = to->heal(hp, from);
  return n>0;
}
bool Engine::damage(Instance* from, int hp, Instance* _to) {
  Thing* to = CAST(_to, Thing);
  int n = to->hurt(hp, from);
  return n>0;
}

bool Engine::HeroHeal(Instance* from, int hp, Instance* to) {
  if (from && from->player->state.auchenai) 
    return HeroDamage(from, hp, to);
  if (from) // velen mutliplier
    hp *= (1 << from->player->state.velen);
  return heal(from, hp, to);
}
bool Engine::HeroDamage(Instance* from, int hp, Instance* to) {
  if (from) // velen mutliplier
    hp *= 1 << from->player->state.velen;
  return damage(from, hp, to);
}

bool Engine::SpellHeal(Instance* from, int hp, Instance* to) {
  if (from && from->player->state.auchenai)
    return SpellDamage(from, hp, to);
  if (from) // velen mutliplier
    hp *= 1 << from->player->state.velen;
  return heal(from, hp, to);
}
bool Engine::SpellDamage(Instance* from, int hp, Instance* to) {
  if (from) {
    hp += from->player->state.spell_power; // spell power
    hp *= 1 << from->player->state.velen; // velen mutliplier
  }
  return damage(from, hp, to);
}

bool Engine::heal_zone(Instance* from, int hp, Target zone) {
  ListPInstance creatures = zone.resolve(from->player, from);
  Engine* engine = this;
  SEND_DISPLAY_MSG(Msg_ZoneHeal, GETP(from), zone, hp);
  for (auto& i : creatures) {
    Creature* creature = CAST(i.get(), Creature);
    if (!creature->is_dead())
      creature->heal(hp, from);
  }
  return true;
}
bool Engine::damage_zone(Instance* from, int hp, Target zone) {
  ListPInstance creatures = zone.resolve(from->player, from);
  Engine* engine = this; 
  SEND_DISPLAY_MSG(Msg_ZoneDamage, GETP(from), zone, hp);
  for (auto& i : creatures) {
    Creature* creature = CAST(i.get(), Creature);
    if (!creature->is_dead())
      creature->hurt(hp, from);
  }
  return true;
}
bool Engine::SpellHeal_zone(Instance* from, int hp, Target zone) {
  if (from && from->player->state.auchenai)
    return SpellDamage_zone(from, hp, zone);
  if (from) // velen mutliplier
    hp *= 1 << from->player->state.velen;
  return heal_zone(from, hp, zone);
}
bool Engine::SpellDamage_zone(Instance* from, int hp, Target zone) {
  if (from) {
    hp += from->player->state.spell_power; // spell power
    hp *= 1 << from->player->state.velen; // velen mutliplier
  }
  return damage_zone(from, hp, zone);
}


bool Engine::attack(Creature* from, Creature* target) {
  assert(from && target);
  signal(from, Event::StartAttack);
  if(from->attack(target)) {
    signal(from, Event::EndAttack);
    return true;
  }
  else
    return false;
}

//void Engine::send_display_message(PMessage msg) {
//  //assert(!issubclassP(msg, Msg_ReceiveCard));
//  if (!is_simulation) // useless if it's a simulation
//    display.push_back(msg);
//}


//void SimulationEngine::save_state(int num = 0) {
//  /*to  run  a  simulation  based  on  current  state,
//  and  see  what  happens  if  we  take  some  action*/
//  saved_turn[num] = state.turn;
//  board.save_state(num);
//}

//def  restore_state(, num = 0) :
//__dict__.update(saved[num])
//board.restore_state(num)

//def  hash_state() :
//return  board.hash_state()

//def  end_simulation(, num = 0) :
//restore_state(num)
//del  saved
//board.end_simulation()
//true_engine.set_default()


