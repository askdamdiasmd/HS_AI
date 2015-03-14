#include "decks.h"
#include "Cards.h"
#include "engine.h"
#include "collection.h"
#include "creatures.h"
#include "players.h"
#include "messages.h"

Engine* Deck::engine = nullptr;

void Deck::set_owner(Player* owner) {
  player = owner;
}

PCard Deck::draw_one_card() {
  PCard card;
  if (!cards.empty()) {
    int r = randint(0, size() - 1);
    card = pop_at(cards, r);
    SEND_MSG(Msg_CardDrawn, PInstance(), card, player);
  }
  else {
    fatigue++;
    engine->damage(PInstance(), fatigue, player->state.hero);
  }
  return card;
}

// draw initial cards in starting hands and do mulligan
ListCard Deck::draw_init_cards(int nb, FuncMulligan mulligan) {
  ListCard keep;
  for (int i = 0; i < nb; ++i)
    keep.push_back(draw_one_card());
  ListCard discarded = player->mulligan(keep);

  // draw replacement cards
  int n_replace = nb - keep.size();
  for (int c = 0; c < n_replace; ++c)
    keep.push_back(draw_one_card());

  // put back discarded cards in deck
  cards.insert(cards.end(), discarded.begin(), discarded.end());

  return keep;
}

void Deck::print() const {
  printf("Enumerating deck of %s (%d cards):\n", player->name.c_str(), cards.size());
  for (auto c : cards)
    printf(" %s\n", c->tostr().c_str());
  printf("\n");
}

PDeck fake_deck(const Collection& cardbook, bool debug, ArrayString fake_cards) {
  ListCard cards;
  if (debug) {
    assert(fake_cards.size());
    int mul = 30 / len(fake_cards);
    for (auto c : fake_cards)
      for (int i = 0; i < mul && len(cards) < 30; i++)
        cards.push_back(cardbook.get_by_name(c)->copy());
  }
  else {
    const ListConstCard& coll = cardbook.get_collectibles();
    for (int i = 0; i < 30; i++)
      cards.push_back(coll[randint(0, coll.size() - 1)]->copy());
  }
  return NEWP(Deck, cards);
}