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

  virtual string tostr() const = 0;

  //virtual PInstance copy() const = 0;

  virtual float score_situation() = 0;
};


/// ------------ physical object (hero, weapon, minon) -------

struct Thing : public Instance {
  enum StaticEffect {
    #define ENUM(i)  (1<<i)
    dead          = ENUM(0),
    taunt         = ENUM(1),
    windfury      = ENUM(2),
    divine_shield = ENUM(3),
    frozen        = ENUM(4),
    freezer       = ENUM(5),
    stealth       = ENUM(6),
    untargetable  = ENUM(7),
    fresh         = ENUM(8),
    enraged       = ENUM(9),
    charge        = ENUM(10),
    insensible    = ENUM(11),
    death_rattle  = ENUM(12), // minion has a death_rattle
    trigger       = ENUM(13), // minion contains some triggers
    silenced      = ENUM(14),
    aura          = ENUM(15),
    #undef ENUM
  };

  struct State {
    int hp, max_hp, armor;  // always reprensent current actual (total) hp
    int atq, max_atq;       // always represent current actual (total) atq
    int tmp_atq;            // get canceled at the end of each turn
    int aura_hp, aura_atq;  // get canceled when aura creature dies. 
                            // Should be ignored sometimes, eg. when x2 the atq or hp
    
    int n_atq, n_max_atq;   // number of remaining attacks
    int n_remaining_power;  // number of remaining hero power (for hero)

    int static_effects; // bit-OR combination of StaticEffect
    int spell_power;

    // IMPORTANT: when adding dynamic stuff here, don't forget to modify copy constructor
    ListPEffect effects;  // all effects which are not in lists below (NO DUPLICATES)
    ListPEff_Presence presence_effects;  // triggered on popup and canceled on depop
    ListPEff_DeathRattle death_rattles; // executed when thing dies
  } state;

  PVizThing viz_thing();
  PConstCardThing card_thing() const;

  Thing(int atq, int hp, int static_effects = 0);
  Thing(const Thing& copy);
  virtual ~Thing() {}

  virtual void list_actions(ListAction& actions) const = 0;

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
  IS_EFFECT(aura);
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

  virtual void popup(); // init when created

  virtual void start_turn(Player* current);
  virtual void end_turn(Player* current);

  bool is_damaged() const { return state.hp < state.max_hp; }

  int hurt(int damage, Instance* caster = nullptr);
  int heal(int hp, Instance* caster = nullptr);

  void change_hp(int hp, char type='n');
  void change_atq(int atq, char type='n');

  void silence(bool die=false);

  void check_dead();  // am I dead ?
  void destroy(bool clean_now = true);  // order of self-destruction
  virtual void signal_death();  // i am dead (called by Board::clean_deads)
};


////// ------------ Creature (hero or minion) ----------

struct Creature : public Thing {
  enum Breed {
    None = 0,
    Beast,
    Demon,
  };
  const Breed breed;
  const Act_Attack act_attack;

  Creature(int atq, int hp, int static_effects = 0, Breed breed = Breed::None) :
    Thing(atq, hp, static_effects), breed(breed), act_attack(nullptr) {}
  Creature(const Creature& copy) :
    Thing(copy), breed(copy.breed), act_attack(this) {}

  bool can_attack() const {
    return !(state.atq==0 || state.n_atq >= state.n_max_atq ||
            (is_fresh() && !is_charge()) || is_frozen());
  }

  virtual bool attack(Creature* target);
};


////// ------------ Minion ----------

struct Minion : public Creature {
  PVizMinion viz_minion();
  PConstCardMinion card_minion() const;
  
  Minion(int atq, int hp, int static_effects = 0, Breed breed = Breed::None) :
    Creature(atq, hp, static_effects, breed) {}
  Minion(const Minion& copy, Player* player);

  virtual string tostr() const;
  virtual void list_actions(ListAction& actions) const;

  virtual void popup();
  virtual void signal_death();

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
  PConstCardHero card_hero() const;
  PConstWeapon weapon() const;
  PVizHero viz_hero();

  Hero(int hp); // creator for collection
  Hero(PConstCardHero hero); 

  //virtual PInstance copy() const { NI; return nullptr; }

  virtual string tostr() const;

  virtual void list_actions(ListAction& actions) const;

  virtual void start_turn(Player* current);
  virtual void end_turn(Player* current);

  void equip_weapon();
  void unequip_weapon();

  void use_hero_power();

  void add_armor(int n);

  virtual bool attack(Creature* target);

  virtual float score_situation();
};


////// ------------ Weapon ----------

struct Weapon : public Thing {
  PVizWeapon viz_weapon();
  PConstCardWeapon card_weapon() const;

  Weapon(int atq, int hp, int static_effects = 0) :
    Thing(atq, hp, static_effects) {}

  Weapon(const Weapon& copy, Player* controller);

  virtual string tostr() const;

  virtual void popup();
  virtual void signal_death();

  virtual void list_actions(ListAction& actions) const {} // no action

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