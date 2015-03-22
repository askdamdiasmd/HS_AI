#ifndef __CREATURES_H__
#define __CREATURES_H__
#include "common.h"
#include "actions.h"

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
  PConstHero hero() const;

  Instance() {} // default = empty

  void set_controller(Player* p) {
    player = p;
  }
  void init(PConstCardInstance card, Player* p) {
    this->card = card;
    set_controller(p);
  }

  virtual void popup() { NI; }

  //virtual void start_turn(Player* current) { NI; }

  //virtual void end_turn(Player* current) { NI; }

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
    int hp, max_hp, armor;  // BY CONVENTION: hp always reprensent current actual hp
    int atq, max_atq;       // BY CONVENTION: atq always represent current actual atq
    int tmp_hp, tmp_atq;    // get canceled at the end of the turn
    //int aura_hp, aura_atq;  // get canceled when aura creature dies. Should not be taken into account sometimes.
    
    int n_atq, n_max_atq;   // number of remaining attacks
    int n_remaining_power;  // number of remaining hero power (for hero)

    int static_effects; // bit-OR combination of StaticEffect
    int spell_power;
  } state;

  PVizThing viz_thing();

  PConstCardThing card_thing() const;

  Thing(int atq, int hp, int static_effects = 0);

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
  bool is_targetable(Player* by_who) const {
    if (player != by_who && is_stealth())  return false;
    return true;
  }
  bool is_spell_targetable(Player* by_who) const {
    if (is_untargetable()) return false;
    return is_targetable(by_who);
  }

  void add_static_effect(StaticEffect eff, bool inform=true);
  void remove_static_effect(StaticEffect eff, bool inform=true);

  virtual void list_actions(ListAction& actions) const = 0;

  virtual void popup(); // init when created

  void start_turn(Player* current);  // auto trigger at start of MY turn

  void end_turn(Player* current);  // auto trigger at end of MY turn

  bool is_damaged() const { return state.hp < state.max_hp; }

  virtual void attack(Thing* target) = 0;

  int hurt(int damage, Thing* caster = nullptr);

  int heal(int hp, Thing* caster = nullptr);

  void change_hp(int hp);

  void change_atq(int atq);

  void silence();

  void check_dead();

  void kill_me();
};


////// ------------ Creature (hero or minion) ----------

struct Creature : public Thing {
  const Act_Attack act_attack;

  Creature(int atq, int hp, int static_effects = 0) :
    Thing(atq, hp, static_effects), act_attack(nullptr) {}

  Creature(const Creature& copy) :
    Thing(copy), act_attack(this) {}

  virtual void attack(Thing* target);
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

  Minion(int atq, int hp, int static_effects = 0, Category cat = Category::None) :
    Creature(atq, hp,static_effects), category(cat) {}
  
  Minion(const Minion& copy, Player* player);

  //virtual PInstance copy() const;

  virtual string tostr() const;

  virtual void popup();

  virtual void list_actions(ListAction& actions) const;

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

  virtual void list_actions(ListAction& actions) const;

  //virtual void start_turn();  // done in Thing::

  void use_hero_power();

  void add_armor(int n);

  virtual float score_situation();
};


////// ------------ Weapon ----------

struct Weapon : public Thing {
  //const Act_WeaponAttack act_attack;
  PVizWeapon viz_weapon();

  //Weapon(const PCardWeapon card, Player* player) :
  //  Thing(card, controller) {}

  //virtual PInstance copy() const { return ; }

  virtual void list_actions(ListAction& actions) const{ NI; }

  virtual void attack(Thing* target);

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

  virtual void list_actions(ListAction& actions) const {}

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