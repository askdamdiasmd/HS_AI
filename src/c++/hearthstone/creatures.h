#ifndef __CREATURES_H__
#define __CREATURES_H__
#include "common.h"

/// ------------ generalistic thing (hero, weapon, minon, secret) -------

struct VizInstance;
typedef shared_ptr<VizInstance> PVizInstance;
struct VizSecret;
typedef shared_ptr<VizSecret> PVizSecret;
struct VizThing;
typedef shared_ptr<VizThing> PVizThing;
struct VizWeapon;
typedef shared_ptr<VizWeapon> PVizWeapon;
struct VizMinion;
typedef shared_ptr<VizMinion> PVizMinion;
struct VizHero;
typedef shared_ptr<VizHero> PVizHero;

struct Instance {
  SET_ENGINE();
  PConstCardInstance card; // card from which it was instanciated
  Player* player; // owner
  PVizInstance viz; // vizualization object

  PHero hero(); // helper function

  Instance() {} // default = empty

  void set_controller(Player* p) {
    player = p;
  }
  void init(PConstCardInstance card, Player* p) {
    this->card = card;
    set_controller(p);
  }

  virtual string tostr() const = 0;

  //virtual PInstance copy() const = 0;

  virtual float score_situation() = 0;
};


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
    death_rattle  = 0x00001000, // minion has a death_rattle
    trigger       = 0x00002000, // minion contains some triggers
    silenced      = 0x00004000,
  };

  struct State {
    int hp, max_hp, armor;
    int atq, max_atq;
    int n_atq, n_max_atq, n_remaining_power;  // number of remaining hero power (for hero)

    int static_effects; // bit-OR combination of StaticEffect
    int spell_power;

    /*List .action_filters = []  // reacting to actions[(Act_class, handler), ...]
    state.modifiers = [] // modifying messages, list: [(Msg_Class, event), ...]
    state.triggers = [] // reacting to messages, list: [(Msg_Class, event), ...]
    state.enraged_trigger = None
    state.effects = []  // list of effects without triggers : ['taunt', 'stealth', ...]*/
  } state;

  PVizThing viz_thing();

  PConstCardThing card_thing() const;

  //Thing();  // proper init to zero

  Thing(int atq, int hp);

//  /*def save_state(num = 0) :
//    state.saved[num] = dict(hp = state.hp, max_hp = state.max_hp, armor = state.armor, atq = state.atq, max_atq = state.max_atq, enraged = state.enraged,
//    fresh = state.fresh, n_atq = state.n_atq, n_max_atq = state.n_max_atq, dead = state.dead, UPDATE_STATUS_id = state.UPDATE_STATUS_id,
//    action_filters = list(state.action_filters), modifiers = list(state.modifiers),
//    triggers = list(state.triggers), enraged_trigger = state.enraged_trigger, effects = list(state.effects))
//    def restore_state(num = 0) :
//    state.__dict__.update(state.saved[num])
//    state.action_filters = list(state.action_filters)
//    state.modifiers = list(state.modifiers)
//    state.triggers = list(state.triggers)
//    state.effects = list(state.effects)
//    def end_simulation() :
//    state.saved = dict()*/
//
//  //def add_effects(effects, inform = True) :
//  //for e in effects : // we have to initalize effects
//  //if type(e) == str:
//  //if e not in state.effects : // useless otherwise
//  //state.effects.append(e)
//  //if e in('charge', 'windfury') :
//  //state.popup()  // redo popup to set n_atq / n_max_atq
//  //else:
//  //e.bind_to()
//  //if inform :
//  //state.engine.send_UPDATE_STATUS(Msg_UPDATE_STATUS('effects'))
//
//  //def remove_effect(effect, inform = True) :
//  //if effect not in state.effects : return  // nothing to do
//  //state.effects.remove(effect)
//  //if type(effect) != str:
//  //effect.undo()
//  //// verify that nothing remains...
//  //for _, eff in state.triggers:
//  //assert effect is not eff, pdb.set_trace()
//  //for _, eff in state.action_filters :
//  //assert effect is not eff, pdb.set_trace()
//  //for _, eff in state.modifiers :
//  //assert effect is not eff, pdb.set_trace()
//  //else:
//  //state.popup()  // reset n_max_atq
//  //if inform:
//  //state.engine.send_UPDATE_STATUS(Msg_UPDATE_STATUS('effects'))
//
//  virtual string tostr() const {
//    return string_format("%s (%X) %d/%d", card->name, this, state.atq, state.hp);
//  }

#define IS_EFFECT(eff)  bool is_##eff() const {return (state.static_effects & StaticEffect::##eff)!=0;}
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
  IS_EFFECT(death_rattle);
  IS_EFFECT(trigger);
#undef IS_EFFECT

  void add_static_effect(StaticEffect eff) {
    state.static_effects |= eff;
  }
  void remove_static_effect(StaticEffect eff) {
    state.static_effects &= ~eff;
  }

  virtual ListAction list_actions() = 0;

  virtual void popup() { // executed when created
    state.n_max_atq = is_windfury() ? 2 : 1;
  }

  virtual void start_turn() {
    remove_static_effect(StaticEffect::fresh); // we were here before
    state.n_atq = 0;  // didn't attack yet this turn
  }

  virtual void end_turn() {
    if (is_frozen() && state.n_atq == 0)
      remove_static_effect(StaticEffect::frozen);
  }

  bool is_damaged() const {
    return state.hp < state.max_hp;
  }

  void hurt(int damage, Thing* caster = nullptr);

  void heal(int hp);

  void change_hp(int hp);

  void change_atq(int atq);

  void silence();

  void check_dead();

  void kill_me();
};


////// ------------ Creature (hero or minion) ----------

struct Creature : public Thing {
  Creature(int atq, int hp) :
    Thing(atq, hp) {}

  void attack(Creature* target);
};


////// ------------ Minion ----------

struct Minion : public Creature {
  enum Category {
    None = 0,
    Beast,
    Demon,
  } category;

  PVizMinion viz_minion();

  PConstCardMinion card_minion() const;

  Minion(int atq, int hp, Category cat = Category::None) :
    Creature(atq, hp), category(cat) {}
  
  Minion(PConstCardMinion card, Player* player);

  //virtual PInstance copy() const;

  virtual string tostr() const {
    NI;  return "minion";
  }

  //bool is_targetable(Player* by_who) {
  //  if (is_untargetable()) return false;
  //  if (controller != by_who && is_stealth())  return false;
  //  return true;
  //}

  ListAction list_actions();

  //void hurt(int damage, Thing* caster = nullptr) {
  //  assert(damage > 0);
  //  if(is_divine_shield()) {
  //    remove_static_effect(StaticEffect::divine_shield);
  //    UPDATE_STATUS("static_effects");
  //  }
  //  else
  //    Creature::hurt(damage, caster);
  //}

  //void ask_for_death() {
  //  add_static_effect(StaticEffect::dead);
  //  engine->board.clean_dead();
  //}

  //float score_situation() {
  //  /* Effect	cost per point
  //      Destroy minion	5.33
  //      Draw card	1.84
  //      Board damage	1.84
  //      Divine Shield	1.40
  //      WindFury	1.19
  //      Freeze	1.02
  //      Silence	0.83
  //      Damage	0.82
  //      Stealth	0.61
  //      Durability	0.60
  //      Attack	0.57
  //      Taunt	0.51
  //      SpellPower	0.46
  //      Health	0.41
  //      Heal	0.34
  //      Self hero - heal	0.34
  //      Charge	0.33

  //      intrisic ? ? -0.17

  //      Opponent draw card - 1.98
  //      Discard cards - 1.25
  //      Overload - 0.83
  //      Self hero damage - 0.27
  //    */

  //  if (is_dead()) return 0;
  //  int atq = is_frozen() ? 0 : state.atq;
  //  atq *= 1 + is_windfury();
  //  return (state.hp*0.41 + atq*0.57 +
  //    is_divine_shield()*1.4 +
  //    is_windfury()*1.19 +
  //    is_stealth()*0.6 +
  //    is_taunt()*0.51 +
  //    state.spell_power*0.46);
  //}

  virtual float score_situation() {
    NI;  return 0;
  }
};


////// ------------ Hero ----------

struct Hero : public Creature {
  PVizHero viz_hero();

  PConstCardHero card_hero() const;

  Hero(int hp); // creator for collection
  Hero(PConstCardHero hero); 

  //virtual PInstance copy() const { NI; return nullptr; }

  virtual string tostr() const;

  virtual ListAction list_actions();

  //virtual void start_turn() {
  //  Creature::start_turn();
  //  state.n_remaining_power = 1;
  //  if( is_insensible() )
  //    remove_static_effect(StaticEffect::insensible);
  //}

  //void use_hero_power() {
  //  state.n_remaining_power -= 1;
  //  assert(state.n_remaining_power >= 0);
  //}

  //void add_armor(int n) {
  //  state.armor += n;
  //  UPDATE_STATUS("armor");
  //}

  ////void death() {
  ////  //state.silence() # do nothing ?
  ////  // do not remove from board
  ////}

  virtual float score_situation();
};


////// ------------ Weapon ----------

struct Weapon : public Thing {
  PVizWeapon viz_weapon();

  //Weapon(const PCardWeapon card, Player* player) :
  //  Thing(card, controller) {}

  virtual PInstance copy() const { NI; return nullptr; }

  //ListAction list_actions() {
  //  Player* hero = controller->hero;
  //  if (hero->state.n_atq >= hero->state.n_max_atq || hero->is_frozen())
  //    return{};
  //  else
  //    return{ make_shared<Act_WeaponAttack>(this, engine->board.get_attackable_characters(controller)) };
  //}

  //virtual void attack(Creature* target) {
  //  Hero* hero = controller->hero;
  //  hero->state.atq += state.atq;
  //  hero->attack(target);
  //  hero->state.atq -= state.atq;
  //}

  //virtual void ask_for_death() {
  //  assert(0);
  //  //state.engine.send_message(Msg_DeadWeapon(), immediate = True)
  //}

  virtual float score_situation();
};


////// ------------ Secret ----------

struct Secret : public Instance {
  //PVizSecret viz_secret() { return issubclassP(viz, PVizSecret); }

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
  //virtual PInstance copy() const { NI; return nullptr; }
  virtual float score_situation() {
    NI;  return 0;
  }
  virtual string tostr() const {
    assert(0);
    return "Secret";
  }
};


#endif