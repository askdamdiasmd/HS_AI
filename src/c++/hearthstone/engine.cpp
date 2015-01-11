#include "engine.h"
#include "board.h"
#include "players.h"
#include "creatures.h"
#include "cards.h"
#include "messages.h"
#include "actions.h"
#include "events.h"

static void Engine::set_default()  {
  // init global variables: everyone  can  access  board  or  send  messages
  Board::set_engine(this);
  Slot::set_engine(this);
  Thing::set_engine(this);
  Action::set_engine(this);
  Effect::set_engine(this);
  Card::set_engine(this);
  Deck::set_engine(this);
  Message::set_engine(this);
  Player::set_engine(this);
}

void Engine::start_game() {
  players[0]->draw_init_cards(3);
  turn = 1;
  players[1]->draw_init_cards(4, coin = true);
  turn = 0;
}

ListAction Engine::list_player_actions(PPlayer & player) {
  ListAction actions = player->list_actions();
  //actions = filter_actions(actions);  //  filter  actions
  //actions = [a  for  a  in  actions  if  a  and  a.is_valid()  and  a.cost <= player.mana];
  return  actions;
}

void Engine::play_turn() {
  PPlayer & player = get_current_player();
  //board.onEvent(Event::StartTurn, player);
  player->onStartTurn();

  PAction action;
  while (!is_game_ended() && !issubclass(action, Act_EndTurn)) {
    ListAction actions = list_player_actions(player);
    action = player->choose_actions(actions);    //  action  can  be  Msg_EndTurn
    action->execute();
  }
  turn += 1;
}

PPlayer Engine::get_winner() {
  PPlayer p1 = players[0];
  PPlayer p2 = players[1];
  if (p1.hero.dead and  !p2.hero.dead)
    return  p2;
  if (!p1.hero.dead and  p2.hero.dead)
    return  p1;
  return nullptr; // match nul
}


void SimulationEngine::save_state(int num = 0) {
  /*to  run  a  simulation  based  on  current  state,
  and  see  what  happens  if  we  take  some  action*/
  saved_turn[num] = turn;
  board.save_state(num);
}

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

virtual void SimulationEngine::display_status(PStatus & msg) {
  assert(!"display_status should never be called for SimulationEngine");
}








