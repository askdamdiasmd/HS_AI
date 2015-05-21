#ifndef __DECKS_H__
#define __DECKS_H__
#include "common.h"

struct Deck {
  SET_ENGINE();
  Player* player;
  ListPCard cards;
  int fatigue;

  Deck() :
    player(nullptr), fatigue(0) {}

  Deck( ListPCard cards ) :
    player(nullptr), cards(cards), fatigue(0) {
    assert(cards.size() == 30);
  }

  int size() const {
    return len(cards);
  }

  void set_owner(Player* owner);

  //def save_state(self, num = 0) :
  //saved[num] = dict(fatigue = fatigue, cards = list(cards))
  //def restore_state(self, num = 0) :
  //__dict__.update(saved[num])
  //cards = list(cards)
  //def end_simulation(self) :
  //saved = dict()

  // pop one random card from deck and return it (if null : fatigue)
  PCard draw_one_card();

  // draw initial cards in starting hands and do mulligan
  ListPCard draw_init_cards(int nb, FuncMulligan mulligan);

  void print() const;
};



struct Collection;

PDeck fake_deck(const Collection& cardbook, bool debug = false, ArrayString fake_cards = {}, int nb = 30);



#endif