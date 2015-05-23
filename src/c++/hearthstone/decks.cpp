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
    SEND_DISPLAY_MSG(Msg_NewCard, nullptr, card);
  }
  else {
    fatigue++;
    SEND_DISPLAY_MSG(Msg_Fatigue, player->state.hero, fatigue);
    engine->board.damage(nullptr, fatigue, player->state.hero.get());
  }
  return card;
}

// draw initial cards in starting hands and do mulligan
ListPCard Deck::draw_init_cards(int nb, FuncMulligan mulligan) {
  ListPConstCard keep;
  while (len(keep) < nb)
    keep.push_back(draw_one_card());
  ListPConstCard discarded = player->mulligan(keep);
  assert(len(discarded) + len(keep) == nb);

  // draw replacement cards
  while (len(keep) < nb)
    keep.push_back(draw_one_card());

  // put back discarded cards in deck
  for (auto c : discarded)
    cards.push_back(CONSTCASTP(c,Card));

  return *(ListPCard*)(&keep);  // whatever...
}

void Deck::print() const {
  printf("Enumerating deck of %s (%d cards):\n", player->name.c_str(), cards.size());
  for (auto c : cards)
    printf(" %s\n", c->tostr().c_str());
  printf("\n");
}

PDeck fake_deck(const Collection& cardbook, bool debug, ArrayString fake_cards, int nb) {
  ListPCard cards;
  if (debug) {
    assert(fake_cards.size());
    int mul = 1 + (nb - 1) / len(fake_cards);
    for (auto c : fake_cards)
      for (int i = 0; i < mul && len(cards) < nb; i++)
        cards.push_back(cardbook.get_by_name(c)->copy());
  }
  else {
    const ListPConstCard& coll = cardbook.get_collectibles();
    for (int i = 0; i < nb; i++)
      cards.push_back(coll[randint(0, coll.size() - 1)]->copy());
  }
  return NEWP(Deck, cards);
}