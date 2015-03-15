#ifndef __CURSES_INTERFACE_H__
#define __CURSES_INTERFACE_H__
#include "common.h"
#include "players.h"
#include "creatures.h"
#include "engine.h"

#include <curses.h>
#include <panel.h>
//extern struct WINDOW;
//extern struct PANEL;

void init_screen();

void congratulate_winner(Player* winner, int turn);

void end_screen();

//# Viz classes = copy object specs
struct pos_t { 
  int y, x;
  pos_t() = default;
  pos_t(const pos_t& ) = default;
  pos_t(int y, int x) : y(y), x(x) {}

  bool operator == (const pos_t& p) const {
    return x == p.x && y == p.y;
  }
  bool operator != (const pos_t& p) const {
    return !(*this == p);
  }
};

union AllTypes {
  int _int;
  bool _bool;
  struct { pos_t _pos_t; };
};
typedef unordered_map<string, AllTypes> ArgMap;

// ### Panel viz object = root viz class -------

struct VizPanel {
  WINDOW* win;
  PANEL* panel;

  VizPanel() : 
    win(nullptr), panel(nullptr) {}

  virtual ~VizPanel();

  virtual WINDOW* draw(const ArgMap& args) = 0;
};

typedef shared_ptr<VizPanel> PVizPanel;

//### Button -----------

struct VizButton : public VizPanel {
  string text;
  enum Align {
    left, right, center
  };

  VizButton(int y, int x, string text, Align align = center, int ty = 3, int tx = 0);

  virtual WINDOW* draw(const ArgMap& args);
};

typedef shared_ptr<VizButton> PVizButton;

struct VizHeroPowerButton : public VizButton {
  string subtext;
  int cost;
  bool used;

  VizHeroPowerButton(int y, int x, string text, string subtext, int cost = 2);

  virtual WINDOW* draw(const ArgMap& args);
};

typedef shared_ptr<VizHeroPowerButton> PVizHeroPowerButton;

void wait_delete(float duration, PVizButton button, VizThing* viz);
void temp_panel(VizThing* viz, string text, int color, float duration = 2);


//### General thing (minion, weapon, hero...)------

struct VizInstance : public VizPanel {
  const PInstance obj;

  VizInstance(PInstance obj);
};

struct VizThing : public VizInstance {
  int hp, max_hp, max_card_hp;
  int atq, max_atq, max_card_atq;
  int effects;
  volatile int wait;

#define IS_EFFECT(eff)  bool is_##eff() const {return (effects & Thing::StaticEffect::##eff)!=0;}
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
  IS_EFFECT(silenced);
#undef IS_EFFECT

  PThing thing()  { return dynamic_pointer_cast<Thing>(obj); }

  VizThing(PThing obj, int ty, int tx, int y, int x);

  bool check();

  virtual ~VizThing();

  int buff_color(int val, bool highlight = false, bool standout = false) const;
  int buff_color(const int* val, bool highlight = false, bool standout = false) const;

  virtual WINDOW* draw(const ArgMap& args);

  int& getattr(int VizThing::*what) {
    return (*this).*what;
  }

  bool update_stats(const Msg_Status& msg, bool show_hp = true);
};

//### Hero -----------

struct VizHero : public VizThing {
  int armor;
  PHero hero(){ return dynamic_pointer_cast<Hero>(obj); }

  VizHero(PHero hero, pos_t pos);

  bool check();

  virtual WINDOW* draw(const ArgMap& args);

  PVizHeroPowerButton create_hero_power_button();
};


//### Minion -----------

struct VizMinion : public VizThing {
  PMinion minion(){ return dynamic_pointer_cast<Minion>(obj); }

  VizMinion(PMinion minion);

  virtual WINDOW* draw(const ArgMap& args);
};

struct VizWeapon : public VizThing {
  PWeapon weapon(){ return dynamic_pointer_cast<Weapon>(obj); }

  VizWeapon(PWeapon weapon);

  virtual WINDOW* draw(const ArgMap & args);
  
  bool update_stats(const Msg_Status& msg);
};


//### Player -----------

struct VizPlayer {
  Player* const player;
  Player::State state;

  VizPlayer(Player* player);

  bool check() const;

  void set_weapon(PWeapon weapon);

  void unset_weapon(PWeapon weapon);
};


//### Slot -----------

struct VizSlot : public VizPanel {
  const Slot slot;  // copy it

  VizSlot(Slot slot) :
    VizPanel(), slot(slot) {}

  // return position, space
  pos_t get_screen_pos() const;
  int get_screen_space() const;

  virtual WINDOW* draw(const ArgMap& args);
};


//### Card -----------

struct VizCard : public VizPanel {
  static const pos_t card_size;
  PCard card;
  WINDOW* small_win;
  PANEL* small_panel;
  int cost;

  VizCard(PCard card);
  virtual ~VizCard();

  virtual WINDOW* draw(const ArgMap& args);
};

//### Board --------

struct VizBoard {
  Board* board;
  const bool switch_heroes;
  const bool animated;
  PVizButton end_turn;
  unordered_map<const Player*, PVizHeroPowerButton> hero_power_buttons;

  VizBoard(Board* board, bool switch_heroes = false, bool animated = true);

  Player* get_top_bottom_player(bool top);

  pos_t get_minion_pos(PMinion minion);

  pos_t get_card_pos(PCard card, Player* player);

  pos_t get_hero_pos(Player* player);

  enum {
    bkgd = 0x01,
    decks = 0x02,
    hero = 0x04,
    cards = 0x08,
    mana = 0x10,
    minions = 0x20,
  };
  void draw(int what = 0xFF, Player* which_ = nullptr, bool last_card = true);
};



//# Overload human interface
struct HumanPlayer : public Player {
  /// human player : ask the player what to do
  HumanPlayer(PHero hero, string name, Deck* deck) :
    Player(hero, name, deck) {}

  static bool mouse_in_win(WINDOW* win, int y, int x);

  virtual ListCard mulligan(ListCard & cards) const;

  virtual const Action* choose_actions(ListAction actions, PInstance& choice, Slot& slot) const;
};


//# overloaded HS engine
struct CursesEngine : public Engine {
  string log;
  FILE* logfile;

  /// overload display functions
  CursesEngine(Player* player1, Player* player2);

  virtual void wait_for_display();
  
  void dbg_add_minion(Player* player, PCardMinion card);
};


#endif
































