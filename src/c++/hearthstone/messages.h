#ifndef __MESSAGES_H__
#define __MESSAGES_H__
#include "common.h"
#include "Board.h"
#include "creatures.h"
#include "players.h"


struct Message {
  Instance* const caster;

  Message(Instance* caster) :
    caster(caster) {}

  virtual const char* cls_name() const = 0;
  virtual string tostr() const = 0;

  virtual void draw(Engine* engine) = 0;
};


struct TargetedMessage : public Message {
  Instance* const target;

  TargetedMessage(Instance* caster, Instance* target) :
    Message(caster), target(target) {}
};


struct CardMessage : public Message {
  const PCard card;

  CardMessage(Instance* caster, PCard card) :
    Message(caster), card(card) {}
};


// game messages

struct Msg_StartTurn : public Message {
  Msg_StartTurn(Instance* caster) :
    Message(caster) {}

  virtual const char* cls_name() const { return "Msg_StartTurn"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_EndTurn : public Message {
  Msg_EndTurn(Instance* caster) :
    Message(caster) {}

  virtual const char* cls_name() const { return "Msg_EndTurn"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_PlayerUpdate : public Message {
  const Player::State state;
  const char* what;
  Msg_PlayerUpdate(Instance* caster, const Player::State& state, const char* what) :
    Message(caster), state(state), what(what) {}

  virtual const char* cls_name() const { return "Msg_PlayerUpdate"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_ThingUpdate : public Message {
  const Thing::State state;
  const char* what;
  Msg_ThingUpdate(Thing* caster, const Thing::State& state, const char* what) :
    Message(caster), state(state), what(what) {}

  virtual const char* cls_name() const {return "Msg_ThingUpdate";}
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};


// play card messages

struct Msg_NewCard : public CardMessage {
  /// allocate a new VizCard 

  Msg_NewCard(Instance* caster, PCard card) :
    CardMessage(caster, card) {}

  virtual const char* cls_name() const { return "Msg_NewCard"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_ReceiveCard : public CardMessage {
  // this player receives a new card in his hand
  Player* const player;

  /// give a defined card to player
  Msg_ReceiveCard(Instance* caster, PCard card, Player* target) :
    CardMessage(caster, card), player(target) {}

  virtual const char* cls_name() const { return "Msg_ReceiveCard"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_BurnCard : public Msg_ReceiveCard {
  // this card get burned
  Msg_BurnCard(Instance* caster, PCard card, Player* target) :
    Msg_ReceiveCard(caster, card, target) {}

  virtual const char* cls_name() const { return "Msg_DrawBurnCard"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

//struct Msg_DrawBurnCard(CardMessage) :
//  '''a card was burned because hand is too full'''
//  virtual string tostr() const
//  return '%s draw %s from the deck' % (caster, card)
//
//struct Msg_ThrowCard(CardMessage) :
//  virtual string tostr() const
//  return "Player %s loses %s" % (caster, card)
//  def draw(Engine* engine) :
//  caster.throw_card(card)

struct Msg_ThrowCard : public CardMessage {
  Msg_ThrowCard(Instance* caster, PCard card) :
    CardMessage(caster, card) {}
  virtual const char* cls_name() const { return "Msg_ThrowCard"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

//struct Msg_Fatigue(Message) :
//  """ fatigue damage points (no more cards) """
//  def __init__(caster, damage) :
//  Message.__init__(caster)
//  damage = damage
//  virtual string tostr() const
//  return "%s takes %d points of fatigue" % (caster, damage)
//  def draw(Engine* engine) :
//  caster.hero.hurt(damage)

// minion messages

struct Msg_AddInstance : public Message {
  Msg_AddInstance(Instance* instance) :
    Message(instance) {}
};

struct Msg_AddMinion : public Msg_AddInstance {
  const Slot pos;
  Minion* minion() { return CAST(caster, Minion); }

  Msg_AddMinion(Instance* minion, Slot pos) :
    Msg_AddInstance(minion), pos(pos) {}

  virtual const char* cls_name() const { return "Msg_AddMinion"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

//struct Msg_AddWeapon(Msg_AddThing) :
//  virtual string tostr() const
//  return "%s equipped a %s" % (caster, thing)
//struct Msg_AddSecret(Msg_AddThing) :
//  virtual string tostr() const
//  return "%s sets %s" % (caster, thing)

//struct Msg_Dead(Message) :
//  """ this thing just died, just for information """
//  # caster = minion / weapon / secret
//  virtual string tostr() const
//  return "%s dies." % caster
//  def draw(Engine* engine) :
//  caster.death()
//struct Msg_DeadMinion(Msg_Dead) :
//  pass
//struct Msg_DeadWeapon(Msg_Dead) :
//  pass
//struct Msg_DeadSecret(Msg_Dead) :
//  pass
//struct Msg_DeadHero(Msg_Dead) :
//  pass

// attack / heal

struct Msg_Damage : public TargetedMessage {
  const int amount;
  Msg_Damage(Instance* caster, Instance* target, int damage) :
    TargetedMessage(caster, target), amount(damage) {}

  virtual const char* cls_name() const { return "Msg_Damage"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_Heal : public TargetedMessage {
  const int amount;
  Msg_Heal(Instance* caster, Instance* target, int hp) :
    TargetedMessage(caster, target), amount(hp) {}

  virtual const char* cls_name() const { return "Msg_Heal"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};


























#endif
