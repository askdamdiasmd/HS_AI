#ifndef __CREATURES_H__
#define __CREATURES_H__

#include "common.h"
#include "Cards.h"
#include "messages.h"
#include "players.h"


struct Engine;
struct Hero;
struct VizInstance;

/// ------------ generalistic thing (hero, weapon, minon, secret) -------

struct Instance {
  SET_ENGINE();
  const PCardMinion card; // card from which it was instanciated
  Player* controller; // owner

  Instance( PCardMinion card, Player* controller ) :
    card(card), controller(controller) {}

  virtual float score_situation() = 0;

  Hero* hero() const { return controller->hero; }

  virtual string tostr() const = 0;

  shared_ptr<VizInstance> viz;
};

typedef shared_ptr<Instance> PInstance;
typedef vector<Instance> ListInstance;

////// ------------ Secret ----------

struct Secret : public Instance {

    //def __init__(card, hero) :
    //  Thing.__init__(card)
    //  state.hero = hero
    //  //      state.active = False

    //  //  def save_state(num = 0) :
    //  //      Thing.save_state(num)
    //  //      state.saved[num]['active'] = state.active

    //  def list_actions() :
    //  return None

    //  def ask_for_death() :
    //  state.engine.send_message(Msg_DeadSecret(), immediate = True)

    //  def score_situation() :
    //  if state.dead : return 0
    //    assert 0

  virtual string tostr() const {
    return "Secret";
  }
};

typedef shared_ptr<Secret> PSecret;
typedef vector<PSecret> ListPSecret;
typedef vector<Secret*> ListSecret;


/// ------------ physical object (hero, weapon, minon) -------

struct Thing : public Instance {
  enum StaticEffect {
    taunt         = 0x00000001,
    windfury      = 0x00000002,
    divine_shield = 0x00000004,
    frozen        = 0x00000008,
    freezer       = 0x00000010,
    stealth       = 0x00000020,
    untargetable  = 0x00000040,
    fresh         = 0x00000080,
    dead          = 0x00000100,
    enraged       = 0x00000200,
    charge        = 0x00000400,
    insensible    = 0x00000800,
  };

  struct State {
    short hp, max_hp, armor;
    short atq, max_atq;
    short n_atq, n_max_atq, n_remaining_power;  // number of remaining hero power (for hero)
    int status_id;  // for display purpose

    unsigned int static_effects; // bit-OR combination of StaticEffect
    short spell_power;

    /*List .action_filters = []  // reacting to actions[(Act_class, handler), ...]
    state.modifiers = [] // modifying messages, list: [(Msg_Class, event), ...]
    state.triggers = [] // reacting to messages, list: [(Msg_Class, event), ...]
    state.enraged_trigger = None
    state.effects = []  // list of effects without triggers : ['taunt', 'stealth', ...]*/
  } state;

  Thing(const PCardMinion card, Player* owner = nullptr) :
    Instance(card, controller), state{ 0 } {
    state.atq = state.max_atq = card->atq;
    state.hp = state.max_hp = card->hp;
    add_static_effect(StaticEffect::fresh);
    state.n_max_atq = 1; // number of times we can attack per turn

    //state.add_effects(deepcopy(card.effects), inform = False)
  }

  /*def save_state(num = 0) :
    state.saved[num] = dict(hp = state.hp, max_hp = state.max_hp, armor = state.armor, atq = state.atq, max_atq = state.max_atq, enraged = state.enraged,
    fresh = state.fresh, n_atq = state.n_atq, n_max_atq = state.n_max_atq, dead = state.dead, UPDATE_STATUS_id = state.UPDATE_STATUS_id,
    action_filters = list(state.action_filters), modifiers = list(state.modifiers),
    triggers = list(state.triggers), enraged_trigger = state.enraged_trigger, effects = list(state.effects))
    def restore_state(num = 0) :
    state.__dict__.update(state.saved[num])
    state.action_filters = list(state.action_filters)
    state.modifiers = list(state.modifiers)
    state.triggers = list(state.triggers)
    state.effects = list(state.effects)
    def end_simulation() :
    state.saved = dict()*/

  //def add_effects(effects, inform = True) :
  //for e in effects : // we have to initalize effects
  //if type(e) == str:
  //if e not in state.effects : // useless otherwise
  //state.effects.append(e)
  //if e in('charge', 'windfury') :
  //state.popup()  // redo popup to set n_atq / n_max_atq
  //else:
  //e.bind_to()
  //if inform :
  //state.engine.send_UPDATE_STATUS(Msg_UPDATE_STATUS('effects'))

  //def remove_effect(effect, inform = True) :
  //if effect not in state.effects : return  // nothing to do
  //state.effects.remove(effect)
  //if type(effect) != str:
  //effect.undo()
  //// verify that nothing remains...
  //for _, eff in state.triggers:
  //assert effect is not eff, pdb.set_trace()
  //for _, eff in state.action_filters :
  //assert effect is not eff, pdb.set_trace()
  //for _, eff in state.modifiers :
  //assert effect is not eff, pdb.set_trace()
  //else:
  //state.popup()  // reset n_max_atq
  //if inform:
  //state.engine.send_UPDATE_STATUS(Msg_UPDATE_STATUS('effects'))

  virtual string tostr() const {
    return string_format("%s (%X) %d/%d", card->name, this, state.atq, state.hp);
  }

#define IS_EFFECT(eff)  bool is_##eff() const {return state.static_effects & StaticEffect::##eff;}
  IS_EFFECT(taunt);
  IS_EFFECT(windfury);
  IS_EFFECT(frozen);
  IS_EFFECT(divine_shield);
  IS_EFFECT(freezer);
  IS_EFFECT(stealth);
  IS_EFFECT(untargetable);
  IS_EFFECT(fresh);
  IS_EFFECT(dead);
  IS_EFFECT(enraged);
  IS_EFFECT(charge);
  IS_EFFECT(insensible);
#undef IS_EFFECT

  void add_static_effect(StaticEffect eff) {
    state.static_effects |= eff;
  }
  void remove_static_effect(StaticEffect eff) {
    state.static_effects &= ~eff;
  }

  virtual ListAction list_actions() = 0;

  //PAction filter_action(PAction& action) {
  //  /*for trigger, event in state.action_filters :
  //  if issubclass(type(action), trigger) :
  //    action = event.filter(action)*/
  //  return action; // default = do nothing
  //}

  //def modify_msg(msg) :
  //for trigger, event in state.modifiers :
  //if issubclass(type(msg), trigger) :
  //msg = event.modify(msg)
  //return msg

  //def react_msg(msg) :
  //for trigger, event in list(state.triggers) : //copy because modified online
  //if type(trigger) == str: continue
  //if issubclass(type(msg), trigger) :
  //event.trigger(msg)

  virtual void popup() { // executed when created
    state.n_max_atq = is_windfurry() ? 2 : 1;
  }

  virtual void start_turn() {
    remove_static_effect(StaticEffect::fresh); // we were here before
    state.n_atq = 0;  // didn't attack yet this turn
  }

  virtual void end_turn() {
    if (is_frozen() && state.n_atq == 0)
      remove_static_effect(StaticEffect::frozen);
  }

  void hurt(int damage, Thing* caster = nullptr) {
    assert(damage > 0);
    if (is_insensible())  return;
    int absorbed = min(damage, state.armor);
    if (absorbed) {
      state.armor -= absorbed;
      UPDATE_STATUS("armor");
    }
    damage -= absorbed;
    state.hp -= damage;
    //if( state.enraged_trigger ) state.enraged_trigger.trigger()
    UPDATE_STATUS("hp");
    if(caster && caster->is_freezer())
      add_static_effect(StaticEffect::frozen);
    check_dead();
  }

  bool is_damaged() const {
    return state.hp < state.max_hp;
  }

  void heal(int hp) {
    assert( hp>0 );
    state.hp = min(state.max_hp, state.hp + hp);
    //if( state.enraged_trigger ) state.enraged_trigger.trigger()
    UPDATE_STATUS("hp");
  }

  void change_hp(int hp) {
    state.max_hp += hp;
    assert(state.max_hp >= 1);
    state.hp += max(0, hp);  // only add if positive
    state.hp = min(state.hp, state.max_hp);
    //if state.enraged_trigger: state.enraged_trigger.trigger()
    UPDATE_STATUS("hp max_hp");
    check_dead();
  }

  void change_atq(int atq) {
    state.atq += atq;
    state.max_atq += atq;
    UPDATE_STATUS("atq max_atq");
  }

  void silence() {
    //state.action_filters = {};
    //state.modifiers = []
    //state.triggers = []
    //state.enraged_trigger = None
    //while state.effects :
    //e = state.effects.pop()
    //  if type(e) != str :
    //    e.undo()
    //    elif e == "windfury" :
    //    state.n_max_atq = 1  // undo windfury
    //    state.effects = ['silence']
    //    if not state.dead :
    //      state.engine.send_UPDATE_STATUS(Msg_UPDATE_STATUS('hp max_hp atq max_atq effects'))
    assert(0);
  }

  void check_dead() {
    if (state.hp <= 0)
      state.dead = true;
    engine->board.signal_dead(this);
  }

  void kill_me() {
    silence();
    engine->board.remove_thing(this);
  }
};

typedef shared_ptr<Thing> PThing;


////// ------------ Creature (hero or minion) ----------

struct Creature : public Thing {
  Creature(PCardMinion card, Player* controller = nullptr) :
    Thing(card, controller) {}

  void attack(Creature* target) {
    state.n_atq += 1;
    assert(state.n_atq <= state.n_max_atq);
    if (is_stealth()) {
      remove_static_effect(StaticEffect::stealth);
      UPDATE_STATUS("static_effects");
    }
    target->hurt(state.atq, this);
    if (target->state.atq) hurt(target->change_atq, target);
  }
};

typedef shared_ptr<Creature> PCreature;
typedef vector<Creature*> ListCreature;
typedef vector<PCreature> ListPCreature;


////// ------------ Minion ----------


struct Minion : public Creature {
  Minion(PCardMinion card, Player* controller = nullptr) :
    Creature(card, controller) {}

  bool is_targetable(Player* by_who) {
    if (is_untargetable()) return false;
    if (controller != by_who && is_stealth())  return false;
    return true;
  }

  ListAction list_actions() {
    if (state.n_atq >= state.n_max_atq ||
      state.atq <= 0 || is_frozen() ||
      (is_fresh() && !is_charge()) )
      return {};
    else 
      return { Act_MinionAttack(this, engine->board.get_attackable_characters(controller)) };
  }

  void hurt(int damage, Thing* caster = nullptr) {
    assert(damage > 0);
    if(is_divine_shield()) {
      remove_static_effect(StaticEffect::divine_shield);
      UPDATE_STATUS("static_effects");
    }
    else
      Creature::hurt(damage, caster);
  }

  void ask_for_death() {
    add_static_effect(StaticEffect::dead);
    engine->board.clean_dead();
  }

  float score_situation() {
    /* Effect	cost per point
        Destroy minion	5.33
        Draw card	1.84
        Board damage	1.84
        Divine Shield	1.40
        WindFury	1.19
        Freeze	1.02
        Silence	0.83
        Damage	0.82
        Stealth	0.61
        Durability	0.60
        Attack	0.57
        Taunt	0.51
        SpellPower	0.46
        Health	0.41
        Heal	0.34
        Self hero - heal	0.34
        Charge	0.33

        intrisic ? ? -0.17

        Opponent draw card - 1.98
        Discard cards - 1.25
        Overload - 0.83
        Self hero damage - 0.27
      */

    if (is_dead()) return 0;
    int atq = is_frozen() ? 0 : state.atq;
    atq *= 1 + is_windfury();
    return (state.hp*0.41 + atq*0.57 +
      is_divine_shield()*1.4 +
      is_windfury()*1.19 +
      is_stealth()*0.6 +
      is_taunt()*0.51 +
      state.spell_power*0.46);
  }
};

typedef shared_ptr<Minion> PMinion;
typedef vector<PMinion> ListPMinion;


////// ------------ Hero ----------

struct Hero : public Creature {
  Hero(PCardHero hero, Player* controller) :
    Creature(hero, controller) {
    popup();
  }

  void set_controller(Player* pl) {
    controller = pl;
    card->controller = pl;
    dynamic_cast<Card_Hero*>(card.get())->ability.controller = pl;
  }

  string tostr() const {
    return string_format("[%s (%s) %dHP]", controller->name, card->name, state.hp);
  }

  virtual void start_turn() {
    Creature::start_turn();
    state.n_remaining_power = 1;
    if( is_insensible() )
      remove_static_effect(StaticEffect::insensible);
  }

  void use_hero_power() {
    state.n_remaining_power -= 1;
    assert(state.n_remaining_power >= 0);
  }

  virtual ListAction list_actions() {
    ListAction res;
    if (state.n_remaining_power)
      res += card->ability.list_actions();

    if (state.n_atq < state.n_max_atq && state.atq>0 && not is_frozen()) {
      res.push_back(make_shared<Act_HeroAttack>(this, engine->board.get_attackable_characters(controller)));
    }
    return res;
  }

  void add_armor(int n) {
    state.armor += n;
    UPDATE_STATUS("armor");
  }

  //void death() {
  //  //state.silence() # do nothing ?
  //  // do not remove from board
  //}

  float score_situation() {
    // healthpoint : 1 at 0, 0.3 at 30
    // sum_i = 1..hp max(0, 1 - 0.0233*i)
    int life = min(42, state.armor + state.hp);
    return ((19767 - 233 * life)*life) / 20000.f;
  }
};

typedef shared_ptr<Hero> PHero;


////// ------------ Weapon ----------

struct Weapon : public Thing {
  Weapon(const PCardMinion card, Player* owner = nullptr) :
    Thing(card, controller) {}

  ListAction list_actions() {
    Player* hero = controller->hero;
    if (hero->state.n_atq >= hero->state.n_max_atq || hero->is_frozen())
      return{};
    else
      return{ make_shared<Act_WeaponAttack>(this, engine->board.get_attackable_characters(controller)) };
  }

  virtual void attack(Creature* target) {
    Hero* hero = controller->hero;
    hero->state.atq += state.atq;
    hero->attack(target);
    hero->state.atq -= state.atq;
  }

  virtual void ask_for_death() {
    assert(0);
    //state.engine.send_message(Msg_DeadWeapon(), immediate = True)
  }

  float score_situation() {
    if (is_dead()) return 0;
    int atq = is_frozen() ? 0 : state.atq;
    atq *= 1 + is_windfury();
    return (state.hp*0.6 + atq*0.57);
  }
};

typedef shared_ptr<Weapon> PWeapon;


#endif