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


Engine::Engine(Player* player1, Player* player2) :
  board(),
  turn(0), is_simulation(false) {
  players[0] = player1;
  players[1] = player2;
  set_default();
}

void Engine::set_default()  {
  // init global variables: everyone  can  access  board  or  send  messages
  Board::set_engine(this);
  Target::set_engine(this);
  Instance::set_engine(this);
  Action::set_engine(this);
  //Effect::set_engine(this);
  Card::set_engine(this);
  Deck::set_engine(this);
  //Message::set_engine(this);
  Player::set_engine(this);
}

void Engine::start_game() {
  players[0]->draw_init_cards(3, false);
  turn = 1;
  players[1]->draw_init_cards(4, true);
  turn = 0;
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
  Player* player = get_current_player();
  player->start_turn();
  wait_for_display();

  bool exit = false;
  while (!is_game_ended() && !exit) {
    ListAction actions = list_player_actions(player);
    Instance* choice = nullptr;
    Slot slot;
    const Action* action = player->choose_actions(actions, choice, slot); // can be Act_EndTurn
    action->execute(player->state.hero.get(), choice, slot);
    exit = issubclass(action, const Act_EndTurn); // do it before action is destroyed
    wait_for_display();
  }
  turn += 1;
}

bool Engine::is_game_ended() const {
  return players[0]->state.hero->is_dead() || players[1]->state.hero->is_dead();
}

Player* Engine::get_winner() {
  bool dead0 = players[0]->state.hero->is_dead();
  bool dead1 = players[1]->state.hero->is_dead();
  if (dead0 && !dead1)  return players[1];
  if (!dead0 && dead1)  return players[0];
  return nullptr; // match nul
}


// Game actions ----------------------------

bool Engine::start_turn() {
  Player* player = get_current_player();
  signal(player->state.hero.get(), Event::StartTurn);
  player->start_turn();
  return true;
}
bool Engine::end_turn() {
  Player* player = get_current_player();
  player->end_turn();
  signal(player->state.hero.get(), Event::EndTurn);
  return true;
}

bool Engine::draw_card(Instance* caster, Player* player, int nb ) {
  while (nb-->0)
    player->draw_card();
  return true;
}

bool Engine::play_card(Instance* caster, const Card* _card, int cost) {
  Player* player = caster->player;
  PCard card = findP(player->state.cards, _card);  
  player->use_mana(cost);
  player->throw_card(card);
  return true;
}

bool Engine::add_minion(Instance* caster, PMinion minion, const Slot& slot) {
  Player* pl = minion->player;
  if (board.get_nb_free_slots(pl))
    board.add_thing(minion, slot);
  return false;
}

void Engine::signal(Instance* caster, Event event) {
  board.clean_deads();
}

bool Engine::heal(Instance* _from, int hp, Instance* _to) {
  Thing* from = CAST(_from, Thing);
  Thing* to = CAST(_to, Thing);
  int n = to->heal(hp, from);
  return n>0;
}

bool Engine::damage(Instance* _from, int hp, Instance* _to) {
  Thing* from = CAST(_from, Thing);
  Thing* to = CAST(_to, Thing);
  int n = to->hurt(hp, from);
  return n>0;
}

bool Engine::attack(Instance* from, Instance* target) {
  assert(from && target);
  NI;
  return true;
}

//void Engine::send_display_message(PMessage msg) {
//  //assert(!issubclassP(msg, Msg_ReceiveCard));
//  if (!is_simulation) // useless if it's a simulation
//    display.push_back(msg);
//}


//void SimulationEngine::save_state(int num = 0) {
//  /*to  run  a  simulation  based  on  current  state,
//  and  see  what  happens  if  we  take  some  action*/
//  saved_turn[num] = turn;
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


