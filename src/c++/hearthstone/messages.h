#ifndef __MESSAGES_H__
#define __MESSAGES_H__
#include "common.h"
#include "Board.h"
#include "creatures.h"
#include "players.h"


struct Message {
  const PInstance caster;

  Message(PInstance caster) :
    caster(caster) {}

  virtual const char* cls_name() const = 0;
  virtual string tostr() const = 0;

  virtual bool draw(Engine* engine) = 0;
};


struct TargetedMessage : public Message {
  const PInstance target;

  TargetedMessage(PInstance caster, PInstance target) :
    Message(caster), target(target) {}
};


struct CardMessage : public Message {
  const PCard card;

  CardMessage(PInstance caster, PCard card) :
    Message(caster), card(card) {}
};


// game messages

struct Msg_StartTurn : public Message {
  Msg_StartTurn(PInstance caster) :
    Message(caster) {}

  virtual const char* cls_name() const { return "Msg_StartTurn"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_EndTurn : public Message {
  Msg_EndTurn(PInstance caster) :
    Message(caster) {}

  virtual const char* cls_name() const { return "Msg_EndTurn"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_PlayerUpdate : public Message {
  const Player::State state;
  const char* what;
  Msg_PlayerUpdate(PInstance caster, const Player::State& state, const char* what) :
    Message(caster), state(state), what(what) {}

  virtual const char* cls_name() const { return "Msg_PlayerUpdate"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_ThingUpdate : public Message {
  const Thing::State state;
  const char* what;
  Msg_ThingUpdate(PThing caster, const Thing::State& state, const char* what) :
    Message(caster), state(state), what(what) {}

  virtual const char* cls_name() const {return "Msg_ThingUpdate";}
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};


// play card messages

struct Msg_NewCard : public CardMessage {
  /// allocate a new VizCard 

  Msg_NewCard(PInstance caster, PCard card) :
    CardMessage(caster, card) {}

  virtual const char* cls_name() const { return "Msg_NewCard"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_ReceiveCard : public CardMessage {
  // this player receives a new card in his hand
  Player* const player;
  const int turn;

  /// give a defined card to player
  Msg_ReceiveCard(PInstance caster, PCard card, Player* target, int turn) :
    CardMessage(caster, card), player(target), turn(turn) {}

  virtual const char* cls_name() const { return "Msg_ReceiveCard"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_BurnCard : public Msg_ReceiveCard {
  // this card get burned
  Msg_BurnCard(PInstance caster, PCard card, Player* target) :
    Msg_ReceiveCard(caster, card, target, 0) {}

  virtual const char* cls_name() const { return "Msg_DrawBurnCard"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
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
  Msg_ThrowCard(PInstance caster, PCard card) :
    CardMessage(caster, card) {}
  virtual const char* cls_name() const { return "Msg_ThrowCard"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

//struct Msg_Fatigue(Message) :
//  """ fatigue damage points (no more cards) """
//  def __init__(caster, damage) :
//  Message.__init__(caster)
//  damage = damage
//  virtual string tostr() const
//  return "%s takes %d points of fatigue" % (caster, damage)


// spell messages

struct Msg_Arrow : public TargetedMessage {
  const char ch;
  const char* const color;

  Msg_Arrow(PThing caster, PThing target, char ch, const char* color) :
    TargetedMessage(caster, target), ch(ch), color(color) {}

  virtual const char* cls_name() const { return "Msg_Arrow"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

// hero power message

struct Msg_HeroPower : public Message {
  // the hero uses its hero power
  Msg_HeroPower(PHero hero) :
    Message(hero) {}

  virtual const char* cls_name() const { return "Msg_HeroPower"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_Arrow_HeroPower : public Msg_Arrow {
  Msg_Arrow_HeroPower(PHero caster, PThing target, char ch, const char* color) :
    Msg_Arrow(caster, target, ch, color) {}

  virtual const char* cls_name() const { return "Msg_Arrow_HeroPower"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};


// minion messages

struct Msg_AddInstance : public Message {
  Msg_AddInstance(PInstance instance) :
    Message(instance) {}
};
struct Msg_AddMinion : public Msg_AddInstance {
  const Slot pos;
  PMinion minion() { return CASTP(caster, Minion); }

  Msg_AddMinion(PInstance minion, Slot pos) :
    Msg_AddInstance(minion), pos(pos) {}

  virtual const char* cls_name() const { return "Msg_AddMinion"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};
struct Msg_AddWeapon : public Msg_AddInstance {
  PWeapon weapon() { return CASTP(caster, Weapon); }

  Msg_AddWeapon(PInstance weapon) :
    Msg_AddInstance(weapon) {}

  virtual const char* cls_name() const { return "Msg_AddWeapon"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};
//struct Msg_AddSecret(Msg_AddThing) :
//  virtual string tostr() const
//  return "%s sets %s" % (caster, thing)

struct Msg_RemoveInstance : public Message {
  Msg_RemoveInstance(PInstance instance) :
    Message(instance) {}
  virtual string tostr() const;
};
struct Msg_RemoveMinion : public Msg_RemoveInstance {
  const Slot pos;
  PMinion minion() { return CASTP(caster, Minion); }

  Msg_RemoveMinion(PInstance minion, Slot pos) :
    Msg_RemoveInstance(minion), pos(pos) {}

  virtual const char* cls_name() const { return "Msg_RemoveMinion"; }
  virtual bool draw(Engine* engine);
};
struct Msg_RemoveWeapon : public Msg_RemoveInstance {
  PWeapon weapon() { return CASTP(caster, Weapon); }

  Msg_RemoveWeapon(PInstance weapon) :
    Msg_RemoveInstance(weapon) {}

  virtual const char* cls_name() const { return "Msg_RemoveWeapon"; }
  virtual bool draw(Engine* engine);
};

//struct Msg_DeadSecret(Msg_Dead) :
//  pass
//struct Msg_DeadHero(Msg_Dead) :
//  pass


// attack / heal

struct Msg_Attack : public TargetedMessage {
  const PCreature caster() const { return CASTP(TargetedMessage::caster, Creature); }
  const PCreature target() const { return CASTP(TargetedMessage::target, Creature); }

  Msg_Attack(PCreature caster, PCreature target) :
    TargetedMessage(caster, target) {}

  virtual const char* cls_name() const { return "Msg_Attack"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_Damage : public TargetedMessage {
  const int amount;
  Msg_Damage(PInstance caster, PInstance target, int damage) :
    TargetedMessage(caster, target), amount(damage) {}

  virtual const char* cls_name() const { return "Msg_Damage"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_Heal : public TargetedMessage {
  const int amount;
  Msg_Heal(PInstance caster, PInstance target, int hp) :
    TargetedMessage(caster, target), amount(hp) {}

  virtual const char* cls_name() const { return "Msg_Heal"; }
  virtual string tostr() const;
  virtual bool draw(Engine* engine);
};

struct Msg_ZoneBlink : public Message {
  const Target zone;
  const int amount;
  const string color;
  const float blink_wait;

  Msg_ZoneBlink(PInstance caster, Target zone, int damage, string color, float wait=0.1f) :
    Message(caster), zone(zone), amount(damage), color(color), blink_wait(wait) {}

  virtual bool draw(Engine* engine);
};
struct Msg_ZoneDamage : public Msg_ZoneBlink {
  Msg_ZoneDamage(PInstance caster, Target zone, int damage) :
    Msg_ZoneBlink(caster, zone, damage, "BLACK_on_RED") {}
  virtual const char* cls_name() const { return "Msg_ZoneDamage"; }
  virtual string tostr() const;
};
struct Msg_ZoneHeal : public Msg_ZoneBlink {
  Msg_ZoneHeal(PInstance caster, Target zone, int damage) :
    Msg_ZoneBlink(caster, zone, damage, "BLACK_on_GREEN") {}
  virtual const char* cls_name() const { return "Msg_ZoneHeal"; }
  virtual string tostr() const;
};























#endif
