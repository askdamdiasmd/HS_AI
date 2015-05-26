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


void Engine::reset()  {
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
  board = Board();
  is_simulation = false;
}

const Player* Engine::get_winner() const {
  return board.get_winner();
}

PInstance Engine::random(ListPInstance& instances) {
  // default implementation
  if (len(instances))
    return instances[randint(0, len(instances) - 1)];
  else
    return PInstance();
}

PCard Engine::random(ListPCard& cards) {
  // default implementation
  if (len(cards))
    return cards[randint(0, len(cards) - 1)];
  else
    return PCard();
}


