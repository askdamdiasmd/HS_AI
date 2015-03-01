#ifndef __HEROES_H__
#define __HEROES_H__
#include "common.h"
#include "Cards.h"
#include "actions.h"

struct Card_HeroAbility : public Card {
  const Act_HeroPower action;

  Card_HeroAbility(int cost, string name, FuncAction actions, Target target);
  
  virtual string tostr() const {
    return string_format("Card Hero Ability: %s %s", name.c_str(), desc.c_str());
  }

  virtual PCard copy() const {
    return NEWP(Card_HeroAbility, *this);
  }

  virtual void list_actions(ListAction& list) const;
};


struct Card_Hero : public Card_Instance {
  PCardHeroAbility ability;

  Card_Hero(string name, HeroClass cls, PHero hero, PCardHeroAbility ability);

  virtual string tostr() const;

  virtual PCard copy() const {
    return NEWP(Card_Hero, *this);
  }

  virtual void list_actions(ListAction& list) const;
};




#endif


