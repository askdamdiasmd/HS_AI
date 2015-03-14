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
  const Slot * const slot;

  VizSlot(Slot* slot) :
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


//# Messages-------- -
//
//def interp(i, max, start, end) :
//""" func to interpolate, i varies in [0,m-1] """
//assert 0 <= i<max, debug()
//return start + (end - start)*i / (max - 1)
//
//def draw_Message(self) :
//pass
//
//def draw_Action(self) :
//pass
//
//def draw_Msg_StartTurn(self) :
//player = self.caster
//player.viz.check()  # check consistency with real data
//NC = getmaxyx(stdscr)[1]
//button = Button(10, NC / 2 - 3, " %s's turn! "%player.name, tx = 20, ty = 5)
//button.draw(highlight = BLACK_on_YELLOW)
//show_panels()
//time.sleep(1 if self.engine.board.viz.animated else 0.1)
//button.delete()
//self.engine.board.viz.hero_power_buttons[player].used = False
//self.engine.board.draw()
//
//def draw_Msg_CardDrawn(self) :
//card = self.card
//self.caster.viz.cards.append(card)
//bottom_player = self.engine.board.viz.get_top_bottom_players()[1]
//if self.engine.board.viz.animated and bottom_player == self.caster :
//self.engine.board.draw('cards', which = self.caster, last_card = False)
//NR, NC = getmaxyx(stdscr)
//ty, tx = card_size
//sy, sx = 12, NC - tx
//ey, ex = self.engine.board.viz.get_card_pos(card)
//for y in range(sy, ey + 1) :
//x = int(0.5 + sx + (ex - sx)*(y - sy) / float(ey - sy))
//h = max(0, NR - y)
//card.draw(highlight = BLACK_on_YELLOW, pos = (y, x), small = 0 if h >= ty else h)
//show_panels()
//time.sleep(0.05 + 0.6*(y == sy))
//self.engine.board.draw('cards', which = self.caster)
//
//def draw_Msg_EndTurn(self) :
//pass
//
//def draw_Msg_UseMana(self) :
//self.engine.board.draw('mana', self.caster)
//
//def draw_Msg_ThrowCard(self) :
//card = self.card
//card.owner.viz.cards.remove(card)
//card.delete()
//self.engine.board.draw('cards', which = self.caster)
//
//def draw_Msg_PlayCard(self) :
//top, bot = self.engine.board.viz.get_top_bottom_players()
//if self.caster is top :
//sx = (getmaxyx(stdscr)[1] - card_size[1]) / 2
//kwargs = dict(small = False, cost = self.cost)
//self.card.draw(pos = (0, sx), highlight = BLACK_on_YELLOW, **kwargs)
//show_panels()
//time.sleep(1)
//if self.engine.board.viz.animated:
//for i in range(sx - 1, -1, -2) :
//self.card.draw(pos = (0, i), **kwargs)
//show_panels()
//time.sleep(0.05*(i) / sx)
//time.sleep(0.2)
//self.card.delete()
//show_panels()
//
//def draw_Msg_MinionPopup(self) :
//new_minion = self.caster
//owner = new_minion.owner
//if self.engine.board.viz.animated :
//old_pos = {}
//for i, m in enumerate(owner.viz.minions) :
//old_pos[m] = Slot(owner, i).get_screen_pos()[0]
//owner.viz.minions.insert(self.pos, new_minion)
//new_minion.viz = VizMinion(new_minion)
//if self.engine.board.viz.animated and old_pos :
//new_pos = {}
//for i, m in enumerate(owner.viz.minions) :
//new_pos[m] = Slot(owner, i).get_screen_pos()[0]
//r = VizMinion.size[1] / 2 + 1
//hide_panel(new_minion.viz.panel)
//for i in range(1, r) :
//for m, (oy, ox) in old_pos.items() :
//ny, nx = new_pos[m]
//m.draw(pos = (interp(i, r, oy, ny), interp(i, r, ox, nx)))
//show_panels()
//time.sleep(0.1)
//show_panel(new_minion.viz.panel)
//self.engine.board.draw('minions', which = owner)
//
//def draw_Msg_WeaponPopup(self) :
//weapon = self.caster
//weapon.owner.viz.set_weapon(weapon)
//self.engine.board.draw('heroes', which = weapon.owner)
//
//def draw_Msg_SecretPopup(self) :
//secret = self.caster
//weapon.owner.viz.secrets.append(secret)
//self.engine.board.draw('secrets', which = secret.owner)
//
//def draw_Msg_DeadMinion(self) :
//dead_minion = self.caster
//if dead_minion not in dead_minion.owner.viz.minions :
//return # sometimes, it is already dead
//dead_minion.viz.delete()
//if self.engine.board.viz.animated:
//pl = dead_minion.owner
//old_pos = {}
//for i, m in enumerate(pl.viz.minions) :
//old_pos[m] = Slot(pl, i).get_screen_pos()[0]
//dead_minion.owner.viz.minions.remove(dead_minion)
//if self.engine.board.viz.animated :
//new_pos = {}
//for i, m in enumerate(pl.viz.minions) :
//new_pos[m] = Slot(pl, i).get_screen_pos()[0]
//r = VizMinion.size[1] / 2 + 1
//for i in range(1, r) :
//for m, (ny, nx) in new_pos.items() :
//oy, ox = old_pos[m]
//m.draw(pos = (interp(i, r, oy, ny), interp(i, r, ox, nx)))
//show_panels()
//time.sleep(0.1)
//self.engine.board.draw('minions', which = dead_minion.owner)
//
//def draw_Msg_DeadWeapon(self) :
//dead_weapon = self.caster
//dead_weapon.owner.viz.unset_weapon(dead_weapon)
//dead_weapon.viz.delete()
//self.engine.board.draw('heroes', which = dead_weapon.owner)
//
//def draw_Msg_Status(self) :
//if hasattr(self.caster, 'viz') :
//if not self.caster.viz.update_stats(self) :
//return False
//show_panels()
//
//def draw_Msg_StartAttack(self) :
//if self.engine.board.viz.animated :
//if issubclass(type(self.caster), Weapon) :
//caster = self.caster.hero
//else :
//caster = self.caster
//oy, ox = getbegyx(caster.viz.win)
//oty, otx = getmaxyx(caster.viz.win)
//top_panel(caster.viz.panel) # set assailant as top panel
//ny, nx = getbegyx(self.target.viz.win)
//nty, ntx = getmaxyx(self.target.viz.win)
//nx += (ntx - otx) / 2
//m = abs(oy - ny)
//t = 0.5 / (m + 2)
//for i in range(1, m - (nty + 1) / 2) + range(m - (nty + 1) / 2, -1, -1) :
//caster.draw(pos = (interp(i, m, oy, ny), interp(i, m, ox, nx)))
//show_panels()
//time.sleep(t)
//
//def draw_Msg_StartHeroPower(self) :
//player = self.caster
//button = self.engine.board.viz.hero_power_buttons[player]
//button.used = True
//button.draw(blink = 0.5)
//
//
//def get_center(viz) :
//pos = getbegyx(viz.win)
//size = getmaxyx(viz.win)
//return pos[0] + size[0] / 2, pos[1] + size[1] / 2
//
//def anim_magic_burst(engine, start, end, ch, color, tstep = 0.03, erase = False) :
//dis = int(sum([(start[i] - end[i])**2 for i in range(2)])**0.5)
//pos = []
//for t in range(int(0.5 + dis)) :
//y, x = [int(0.5 + start[i] + (end[i] - start[i])*t / dis) for i in range(2)]
//pos.append((y, x))
//oldch = mvinch(y, x)
//mvaddch(y, x, ch, color)
//show_panels()
//time.sleep(tstep)
//if erase and len(pos) >= 3:
//y, x = pos.pop(-3)
//mvaddch(y, x, oldch & 0xFF, oldch)
//engine.board.viz.draw()
//
//
//def draw_Msg_HeroHeal(self) :
//if self.engine.board.viz.animated :
//player = self.caster
//button = self.engine.board.viz.hero_power_buttons[player]
//anim_magic_burst(self.engine, get_center(button), get_center(self.target.viz), ord('+'), black_on_green, erase = True)
//
//def draw_Msg_HeroDamage(self) :
//if self.engine.board.viz.animated :
//player = self.caster
//button = self.engine.board.viz.hero_power_buttons[player]
//anim_magic_burst(self.engine, get_center(button), get_center(self.target.viz), ord('*'), BLACK_on_RED, erase = True)
//
//
//
//### Board --------

struct VizBoard {
  Board* board;
  const bool switch_heroes;
  const bool animated;
  VizButton end_turn;
  unordered_map<Player*, PVizHeroPowerButton> hero_power_buttons;

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

  virtual const Action* choose_actions(ListAction actions) const;
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
































