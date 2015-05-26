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

void congratulate_winner(const Player* winner, int turn);

void end_screen();

//# Viz classes = copy object specs
struct pos_t { 
  int y, x;
  pos_t() = default;
  pos_t(const pos_t& ) = default;
  pos_t(int y, int x) : y(y), x(x) {}

  bool empty() const { return x == 0 && y == 0; }
  bool operator == (const pos_t& p) const {
    return x == p.x && y == p.y;
  }
  bool operator != (const pos_t& p) const {
    return !(*this == p);
  }
};

union AllTypes {
  int _int;
  float _float;
  bool _bool;
  struct { pos_t _pos_t; };
};
typedef unordered_map<string, AllTypes> ArgMap;

// ### Panel viz object = root viz class -------

struct VizPanel {
  SET_ENGINE();
  WINDOW* win;
  PANEL* panel;

  VizPanel() : 
    win(nullptr), panel(nullptr) {}

  void hide();

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
  PVizHero viz_hero;
  string subtext;
  int cost;

  VizHeroPowerButton(PVizHero viz_hero, int y, int x, string text, string subtext, int cost = 2);

  virtual WINDOW* draw(const ArgMap& args);
};

typedef shared_ptr<VizHeroPowerButton> PVizHeroPowerButton;

void wait_delete(float duration, PVizButton button, VizThing* viz);
void temp_panel(VizThing* viz, string text, int color, float duration = 2);

//### Slot -----------

struct VizSlot : public VizPanel {
  const Slot slot;  // copy it

  VizSlot(Slot slot) :
    VizPanel(), slot(slot) {}

  // return position, space
  pos_t get_screen_pos() const;
  pos_t get_center() const;
  int get_screen_space() const;

  virtual WINDOW* draw(const ArgMap& args);
};


//### General thing (minion, weapon, hero...)------

struct VizInstance : public VizPanel {
  PConstInstance obj;
  PConstCardInstance card;
  string name;

  VizInstance(const PInstance obj);
  virtual ~VizInstance(){}
};

struct VizThing : public VizInstance {
  Thing::State state;
  struct hgh_time_out {
    int hgh;
    double expire;
    hgh_time_out() : hgh(0), expire(0){}
  };
  hgh_time_out hgh_atq, hgh_hp, hgh_armor;

#define IS_EFFECT(eff)  bool is_##eff() const {return (state.static_effects & Thing::StaticEffect::##eff)!=0;}
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

  const Thing* thing() const { return CAST(obj.get(), const Thing); }
  PConstCardThing card_thing() const;

  VizThing(const PThing obj, int ty, int tx, int y, int x);

  //bool check();

  int get_hgh(const hgh_time_out* hgh) const;
  int buff_color(int val, bool highlight = false) const;
  int buff_color(const int* val, int max_card_val=-1, bool highlight = false) const;
  void update_state(const Thing::State& state, bool show_diff=true);

  virtual WINDOW* draw(const ArgMap& args);
};

//### Hero -----------

struct VizHero : public VizThing {
  const Hero* hero() const { return CAST(obj.get(), const Hero); }
  PConstCardHero card_thing() const;
  string player_name, hero_name;
  int w_max_atq;  // max atq of weapon

  VizHero(const PHero hero, pos_t pos);

  PVizHeroPowerButton create_hero_power_button();

  virtual WINDOW* draw(const ArgMap& args);
};


//### Minion -----------

struct VizMinion : public VizThing {
  static const pos_t size;
  const Minion* minion() const { return CAST(obj.get(), const Minion); }

  VizMinion(const PMinion minion, VizSlot pos);

  virtual WINDOW* draw(const ArgMap& args);
};

struct VizWeapon : public VizThing {
  const Weapon* weapon() const { return CAST(obj.get(), const Weapon); }
  //const Card_Weapon* card_thing() const { return CAST(card.get(), const Card_Weapon); }

  VizWeapon(const PWeapon weapon);

  virtual WINDOW* draw(const ArgMap & args);
  
  void update_state(const Thing::State& from);
};


//### Player -----------

struct VizPlayer {
  Player* const player;
  Player::State state;

  VizPlayer(Player* player);
  virtual ~VizPlayer();

  //bool check() const;
  void update_state(const Player::State& from);
};


//### Card -----------

struct VizCard : public VizPanel {
  static const pos_t card_size;
  PConstCard card;
  WINDOW* small_win;
  PANEL* small_panel;
  int cost;

  VizCard(PConstCard card);
  virtual ~VizCard();

  virtual WINDOW* draw(const ArgMap& args);
};

//### Board --------

struct VizBoard {
  static bool switch_heroes, animated;
  static double accel; // time acceleration

  Board* const board;
  PVizButton end_turn;
  unordered_map<const Player*, PVizHeroPowerButton> hero_power_buttons;
  ListPInstance deads;  // deleted at the end of each wait_display

  VizBoard(Board* board);
  ~VizBoard();

  static void sleep(double seconds);
  static double now();

  Player* get_top_bottom_player(bool top);
  pos_t get_minion_pos(const Minion* minion);
  pos_t get_card_pos(PCard card, Player* player);
  pos_t get_hero_pos(Player* player);

  enum {
    bkgd_hero = 0x01,
    bkgd_board = 0x02,
    bkgd_midline = 0x100,
    bkgd = bkgd_hero | bkgd_board | bkgd_midline,
    decks = 0x04,
    hero = 0x08,
    cards = 0x10,
    mana = 0x20,
    minions = 0x40,
    weapon = 0x80,
    all = 0xFFFFFFFF,
  };
  void draw(int what = all, Player* which_ = nullptr, bool last_card = true, chtype bkg_color=0);

  void show_panels(bool lock = true, bool capture = true);

  typedef vector<chtype> screen_copy_t;
  vector<screen_copy_t> history;
  void show_history();
};



//# Overload human interface
struct HumanPlayer : public Player {
  /// human player : ask the player what to do
  HumanPlayer(PHero hero, string name, Deck* deck) :
    Player(hero, name, deck) {}

  virtual ListPConstCard mulligan(ListPConstCard & cards);

  virtual const Action* choose_actions(ListAction actions, Instance*& choice, Slot& slot);
};


//# overloaded HS engine
struct CursesEngine : public Engine {
  string log;
  FILE* logfile;

  CursesEngine();

  virtual void init_players(Player* player1, Player* player2);

  virtual void wait_for_display();
};


#endif
































