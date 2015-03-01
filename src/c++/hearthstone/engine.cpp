#include "actions.h"
#include "board.h"
#include "cards.h"
#include "collection.h"
#include "creatures.h"
#include "decks.h"
#include "effects.h"
#include "engine.h"
#include "events.h"
#include "heroes.h"
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
  //Board::Slot::set_engine(this);
  //Thing::set_engine(this);
  //Action::set_engine(this);
  //Effect::set_engine(this);
  Card::set_engine(this);
  Deck::set_engine(this);
  //Message::set_engine(this);
  //Player::set_engine(this);
}

void Engine::start_game() {
  assert(0);
  //players[0]->draw_init_cards(3);
  //turn = 1;
  //players[1]->draw_init_cards(4, coin = true);
  //turn = 0;
}

ListAction Engine::list_player_actions(Player* player) {
  assert(0); return{};
  //ListAction actions = player->list_actions();
  ////actions = filter_actions(actions);  //  filter  actions
  ////actions = [a  for  a  in  actions  if  a  and  a.is_valid()  and  a.cost <= player.mana];
  //return  actions;
}

void Engine::play_turn() {
  assert(0);
  //PPlayer & player = get_current_player();
  ////board.onEvent(Event::StartTurn, player);
  //player->onStartTurn();

  //PAction action;
  //while (!is_game_ended() && !issubclass(action, Act_EndTurn)) {
  //  ListAction actions = list_player_actions(player);
  //  action = player->choose_actions(actions);    //  action  can  be  Msg_EndTurn
  //  action->execute();
  //}
  //turn += 1;
}

bool Engine::is_game_ended() {
  return board.is_game_ended();
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

//void Engine::display_status(PMsg_Status & msg) {
//  assert(!is_simulation);
//  // default behavior : just print on screen
//  msg->print();
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

  //def  send_status(, message) :
  //pass  //  we  don't  care

void Engine::display_status(PMsgStatus msg) {
  assert(!"display_status should never be called for SimulationEngine");
}

void Engine::heal(PInstance from, int hp, PInstance to) {
  NI;
}

void Engine::signal(Event event, PInstance from) {
  NI;
}

void Engine::damage(PInstance from, int hp, PInstance to) {
  NI;
}



