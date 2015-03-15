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
    if (a->is_valid() && a->get_cost() <= player->state.mana)
      res.push_back(a);
  return res;
}

void Engine::play_turn() {
  Player* player = get_current_player();
  //board.onEvent(Event::StartTurn, player);
  player->start_turn();
  wait_for_display();

  const Action* action = nullptr;
  PInstance choice;
  Slot slot;
  while (!is_game_ended() && !issubclass(action, const Act_EndTurn)) {
    ListAction actions = list_player_actions(player);
    action = player->choose_actions(actions, choice, slot);    //  action  can  be  Msg_EndTurn
    action->execute(player->state.hero, choice, slot);
    wait_for_display();
  }
  turn += 1;
}

Player* Engine::get_winner() {
  assert(0);
  //PPlayer p1 = players[0];
  //PPlayer p2 = players[1];
  //if (p1.hero.dead and  !p2.hero.dead)
  //  return  p2;
  //if (!p1.hero.dead and  p2.hero.dead)
  //  return  p1;
  return nullptr; // match nul
}

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

  //def  send_status(, message) :
  //pass  //  we  don't  care

bool Engine::start_turn() {
  Player* player = get_current_player();
  signal(Event::StartTurn, player->state.hero);
  player->start_turn();
  return true;
}
bool Engine::end_turn() {
  Player* player = get_current_player();
  player->end_turn();
  signal(Event::EndTurn, player->state.hero);
  return true;
}

bool Engine::draw_card(PInstance caster, Player* player, int nb ) {
  while (nb-->0)
    player->draw_card();
  return true;
}

bool Engine::play_card(const Card* _card, PInstance caster) {
  PCard card = indexP(caster->player->state.cards, _card);  
  NI;
  return true;
}

void Engine::signal(Event event, PInstance from) {
  NI;
}

bool Engine::heal(PInstance from, int hp, PInstance to) {
  NI;
  return true;
}

bool Engine::damage(PInstance from, int hp, PInstance to) {
  NI;
  return true;
}

bool Engine::attack(PInstance from, PInstance target) {
  assert(from && target);
  NI;
  return true;
}

void Engine::send_display_message(PMessage msg) {
  //assert(!issubclassP(msg, Msg_ReceiveCard));
  if (!is_simulation) // useless if it's a simulation
    display.push_back(msg);
}
void Engine::display_status(PMsgStatus msg) {
  assert(!"display_status should never be called for SimulationEngine");
}


