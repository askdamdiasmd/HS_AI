#ifndef __DECKS_H__
#define __DECKS_H__
#include "common.h"

struct Deck {
  SET_ENGINE();
  Player* player;
  typedef vector<PCard> ListCard;
  ListCard cards;
  int fatigue;

  Deck( ListCard cards ) :
    player(nullptr), cards(cards), fatigue(0) {
    assert(cards.size() == 30);
  }

  void set_owner(Player* owner);

  //def save_state(self, num = 0) :
  //saved[num] = dict(fatigue = fatigue, cards = list(cards))
  //def restore_state(self, num = 0) :
  //__dict__.update(saved[num])
  //cards = list(cards)
  //def end_simulation(self) :
  //saved = dict()

  int size() const {
    return len(cards);
  }

  // pop one random card from deck and return it (if null : fatigue)
  PCard draw_one_card();

  // draw initial cards in starting hands and do mulligan
  ListCard draw_init_cards(int nb, ListCard(*mulligan)(const ListCard&));
};



struct Collection;

PDeck fake_deck(const Collection& cardbook, bool debug = false, ArrayString fake_cards = {});



#endif