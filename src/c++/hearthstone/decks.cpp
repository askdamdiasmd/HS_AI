#include "decks.h"
#include "Cards.h"
#include "engine.h"
#include "collection.h"
#include "creatures.h"

Engine* Deck::engine = nullptr;

void Deck::set_owner(Player* owner) {
  player = owner;
  for (auto card : cards)
    card->player = owner;
}

PCard Deck::draw_one_card() {
  if (!cards.empty()) {
    int r = randint(0, size() - 1);
    return pop_at(cards, r);
  }
  else {
    fatigue++;
    assert(0);//engine->damage_fatigue(player, fatigue);
    return PCard();
  }
}

// draw initial cards in starting hands and do mulligan
Deck::ListCard Deck::draw_init_cards(int nb, ListCard(*mulligan)(const ListCard&)) {
  ListCard cards;
  for (int i = 0; i < nb; ++i)
    cards.push_back(draw_one_card());
  ListCard keep = mulligan(cards);

  // draw replacement cards
  int n_replace = nb - keep.size();
  for (int c = 0; c < n_replace; ++c)
    keep.push_back(draw_one_card());

  return keep;
}

PDeck fake_deck(const Collection& cardbook, bool debug, ArrayString fake_cards) {
  Deck::ListCard cards;
  if (debug) {
    assert(fake_cards.size());
    int mul = 30 / len(fake_cards);
    for (auto c : fake_cards)
      for (int i = 0; i < mul && len(cards) < 30; i++)
        cards.push_back(cardbook.by_name.at(c)->copy());
  }
  else {
    const Deck::ListCard& coll = cardbook.collectibles;
    for (int i = 0; i < 30; i++)
      cards.push_back(coll[randint(0, coll.size() - 1)]->copy());
  }
  return NEWP(Deck, cards);
}