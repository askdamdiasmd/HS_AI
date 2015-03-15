#ifndef __MESSAGES_H__
#define __MESSAGES_H__
#include "common.h"
#include "creatures.h"
#include "players.h"


struct Message {
  const PInstance caster;

  Message(PInstance caster) :
    caster(caster) {}

  virtual const char* cls_name() const = 0;
  virtual string tostr() const = 0;

  virtual void draw(Engine* engine) = 0;
};


struct TargetedMessage : public Message {
  PInstance const target;

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
  //const Player::State state;
  //Msg_StartTurn(PInstance caster, const Player::State& state) :
  //  Message(caster), state(state) {}
  Msg_StartTurn(PInstance caster) :
    Message(caster) {}

  virtual const char* cls_name() const { return "Msg_StartTurn"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_EndTurn : public Message {
  Msg_EndTurn(PInstance caster) :
    Message(caster) {}

  virtual const char* cls_name() const { return "Msg_EndTurn"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_Status : public Message {
  // just to tell the interface that something happened 
  int VizThing::* attr;
  const int val;
  const string desc;

  Msg_Status(PThing caster, int VizThing::*what, int val, string desc ) :
    Message(caster), attr(what), val(val), desc(desc) {
  }

  virtual const char* cls_name() const {return "Msg_Status";}
  virtual string tostr() const {
    return string_format("Change of status for %s", caster->tostr());/* ,
      ', '.join(['%s=%s' % (a, getattr(a)) for a in attrs]))*/
  }

  virtual void draw(Engine* engine) {
    NI;
  }
};


// play card messages

struct Msg_NewCard : public CardMessage {
  /// allocate a new VizCard 

  Msg_NewCard(PInstance caster, PCard card) :
    CardMessage(caster, card) {}

  virtual const char* cls_name() const { return "Msg_NewCard"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_ReceiveCard : public CardMessage {
  // this player receives a new card in his hand
  Player* const player;

  /// give a defined card to player
  Msg_ReceiveCard(PInstance caster, PCard card, Player* target) :
    CardMessage(caster, card), player(target) {}

  virtual const char* cls_name() const { return "Msg_ReceiveCard"; }
  virtual string tostr() const;
  virtual void draw(Engine* engine);
};

struct Msg_BurnCard : public Msg_ReceiveCard {
  // this card get burned
  Msg_BurnCard(PInstance caster, PCard card, Player* target) :
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

//struct Msg_UseMana(Message) :
//  def __init__(caster, cost) :
//  Message.__init__(caster)
//  cost = cost
//  def draw(Engine* engine) :
//  caster.use_mana(cost)
//  virtual string tostr() const
//  return "Player %s loses %d mana crystal" % (caster, cost)
//
//struct Msg_GainMana(Message) :
//  def __init__(caster, gain) :
//  Message.__init__(caster)
//  gain = gain
//  def draw(Engine* engine) :
//  caster.gain_mana(gain)
//  virtual string tostr() const
//  return "Player %s gain %d mana crystal" % (caster, gain)
//
//struct Msg_PlayCard(CardMessage) :
//  def __init__(caster, card, cost) :
//  CardMessage.__init__(caster, card)
//  cost = cost
//  virtual string tostr() const
//  return "%s plays %s" % (caster, card)
//  def draw(Engine* engine) :
//  engine.send_message(
//  [Msg_UseMana(caster, cost),
//  Msg_ThrowCard(caster, card)], immediate = True)
//
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
  Msg_AddInstance(PInstance caster) :
    Message(caster) {}
};

struct Msg_AddMinion : public Msg_AddInstance {
  const int pos;
  PMinion minion() { return issubclassP(caster, Minion); }

  Msg_AddMinion(PInstance caster, int pos) :
    Msg_AddInstance(caster), pos(pos) {}

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
//
//struct Msg_Popup(Message) :
//  # caster = minion / weapon / secret
//  def draw(Engine* engine) :
//  caster.popup()
//  virtual string tostr() const
//  return "%s pops up" % (caster)
//
//struct Msg_MinionPopup(Msg_Popup) :
//  def __init__(caster, pos) :
//  Msg_Popup.__init__(caster)
//  pos = pos
//struct Msg_SecretPopup(Msg_Popup) :
//  pass
//struct Msg_WeaponPopup(Msg_Popup) :
//  pass
//struct Msg_HeroPopup(Msg_Popup) :
//  pass


//struct Msg_CheckDead(Message) :
//  """ as soon as a minion dies, it asks for its cleaning """
//  def draw(Engine* engine) :
//  for i in engine.board.everybody :
//    if i.dead :
//      i.ask_for_death()
//      virtual string tostr() const
//      return "%s asks for dead cleaning." % caster
//
//
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



//struct Msg_DeathRattle(Message) :
//  """ death rattle: execute a pre-specified instruction """
//  def __init__(caster, msg, immediate = False) :
//  Message.__init__(caster)
//  msg = msg
//  immediate = immediate
//  virtual string tostr() const
//  return "Death rattle casted by %s: %s%s" % (caster, msg,
//  immediate and ' (immediate)' or '')
//  def draw(Engine* engine) :
//  engine.send_message(msg, immediate = immediate)


// attack / heal

//struct Msg_Damage(TargetedMessage) :
//  def __init__(caster, target, damage) :
//  TargetedMessage.__init__(caster, target)
//  damage = damage
//  def draw(Engine* engine) :
//  target.hurt(damage, caster)
//  virtual string tostr() const
//  return "%s takes %d damage from %s." % (target, damage, caster)
//
//struct Msg_HeroDamage(Msg_Damage) :
//  """ damage from hero power """
//  pass
//
//struct Msg_SpellDamage(Msg_Damage) :
//  """ damage from a spell """
//  pass
//
//struct Msg_RandomDamage(Message) :
//  ''' deal damage to a random target'''
//  def __init__(caster, target, damage) :
//  Message.__init__(caster, target)
//  damage = damage
//  def draw(Engine* engine) :
//  while True :
//    r = random.randint(len(target))
//    target = target[r]
//    engine.send_message(Msg_Damage(caster, target, damage), immediate = True)
//    virtual string tostr() const
//    return "%s takes %d damage." % (target, damage)
//
//
//struct Msg_MultiRandomDamage(Message) :
//  ''' deal damage to a random target seveal times'''
//  def __init__(caster, target, damage, each = 1) :
//  Msg_Damage.__init__(caster, target, damage)
//  each = each  # amount of damage of each hit
//  def draw(Engine* engine) :
//  # breaks down into new messages as time passes by
//  engine.send_message(
//  [Msg_RandomDamage(caster, target, each),
//  Msg_MultiRandomDamage(caster, target, damage - 1, each)],
//  immediate = True)
//  virtual string tostr() const
//  return "%s throws %dx%d hit damages randomly." % (caster, damage, each)
//
//
//  # Heal messages
//
//struct Msg_Heal(TargetedMessage) :
//  def __init__(caster, target, heal) :
//  TargetedMessage.__init__(caster, target)
//  heal = heal
//  def draw(Engine* engine) :
//  target.heal(heal)
//  virtual string tostr() const
//  return "%s heals %s by %dHP." % (caster, target, heal)
//
//struct Msg_HeroHeal(Msg_Heal) :
//  pass
//
//struct Msg_RandomHeal(Message) :
//  ''' heal a random target, see Target definition in Card::list_targets '''
//  def __init__(caster, Target, heal) :
//  Message.__init__(caster, Target)
//  heal = heal
//  def draw(Engine* engine) :
//  Target = caster.list_targets(target)
//  if Target : # enough Target
//    target = Target[random.randint(0, len(Target) - 1)]
//    engine.send_message(Msg_Heal(caster, target, heal), immediate = True)
//    virtual string tostr() const
//    return "%s heal by %d a random %s" % (caster, heal, target)
//
//
//
//    # Effect messages
//
//struct Msg_Silence(TargetedMessage) :
//  def __init__(caster, target) :
//  TargetedMessage.__init__(caster, target)
//  virtual string tostr() const
//  return "%s gets silenced by %s." % (target, caster)
//  def draw(Engine* engine) :
//  target.silence()
//
//
//struct Msg_BindEffect(TargetedMessage) :
//  def __init__(caster, target, effect) :
//  TargetedMessage.__init__(caster, target)
//  effect = effect
//  virtual string tostr() const
//  random = 'random 'if type(target) == str else ''
//  return "%s binds effect [%s] to %s%s." % (caster, effect, random, target)
//  def draw(Engine* engine) :
//  target = resolve_target()
//  if target :
//    effect.bind_to(target, caster = caster)


























#endif
