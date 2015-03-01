#ifndef __CURSES_INTERFACE_H__
#define __CURSES_INTERFACE_H__
#include <stdlib.h>
#include <curses.h>
#include <panel.h>
#include "common.h"

typedef WINDOW WIN; // shortcut

int mvwaddstr_ex(WIN* scr_id, int y, int x, const char* cstr, int attr = 0) {
#ifdef _WIN32
  int oldattr;
  if (attr) {
    oldattr = getattrs(scr_id); wattrset(scr_id, attr);
  }
  int ret = mvwaddstr(scr_id, y, x, cstr);
  if (attr) wattrset(scr_id, oldattr);
  return ret;
#else
  if (attr) return scr_id.addstr(y, x, str(cstr), attr)
  return scr_id.addstr(y, x, str(cstr))
#endif
}
void addwch(chtype ch, int attr = 0, WINDOW* win = nullptr, int y = -1, int x = -1, int nb = 1);

void init_screen();

int print_middle(WINDOW* win, int y, int x, int width, string text, int attr = 0);

int print_longtext(WINDOW* win, int y, int x, int endy, int endx, string text, int attr = 0);

void delete_panel(PANEL* panel);

PANEL* top_panel();

void debug();

void show_ACS();

void show_unicode();

//def rounded_box(win) :
//ty, tx = getmaxyx(win)
//box(win)
//addwch(2320, win = win, x = 0, y = 0)
//addwch(2321, win = win, x = tx - 1, y = 0)

void strong_box(WIN* win, int attr = 0) {
  //wborder(win, *((ACS_BLOCK, ) * 8))
  if (false) { // code == 'UTF-8'):
    //h, w = getmaxyx(win)
    //  w -= 1
    //  h -= 1
    //  addwch(9556, attr, win = win, x = 0, y = 0)
    //  addwch(9552, attr, win = win, nb = w - 1)
    //  addwch(9559, attr, win = win)
    //  for i in range(1, h) :
    //    addwch(9553, attr, win = win, x = 0, y = i)
    //    addwch(9553, attr, win = win, x = w, y = i)
    //    addwch(9562, attr, win = win, x = 0, y = h)
    //    addwch(9552, attr, win = win, nb = w - 1)
    //    addwch(9565, attr, win = win)
  }
  else
    wborder(win, 9553, 9553, 9552, 9552, 9556, 9559, 9562, 9565);
}

void weak_box(WIN* win, int attr = 0) {
  wborder(win, ':', ':', '-', '-', ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
}

//def manual_box(win, y, x, h, w) :
//w -= 1
//h -= 1
//mvwaddch(win, y, x, ACS_ULCORNER)
//whline(win, ACS_HLINE, w - 1)
//mvwaddch(win, y, x + w, ACS_URCORNER)
//mvwvline(win, y + 1, x, ACS_VLINE, h - 1)
//mvwvline(win, y + 1, x + w, ACS_VLINE, h - 1)
//mvwaddch(win, y + h, x, ACS_LLCORNER)
//whline(win, ACS_HLINE, w - 1)
//mvwaddch(win, y + h, x + w, ACS_LRCORNER)

/*struct ptr_and_type {
  void* ptr;
  type_info* type;
};
static vector<ptr_and_type> panel_to_obj;
template<typename T>
void set_panel_userptr(PANEL* panel, T* obj) {
  set_panel_userptr(panel, len(panel_to_obj));
  panel_to_obj.push_back({ (void*)obj, @typeid(T) });
}
ptr_and_type get_panel_userptr(panel) {
  return panel_to_obj[panel_userptr(panel)];
}*/


#ifdef _WIN32
static HANDLE show_panel_lock = CreateMutex(NULL, FALSE, NULL);
void lock_panels() {
  WaitForSingleObject(show_panel_lock, INFINITE);
}
void release_panels() {
  ReleaseMutex(show_panel_lock);
}
void show_panels() {
  lock_panels();
  update_panels();
  doupdate();
  release_panels();
}
#else
todo...
#endif


//# Viz classes = copy object specs

union AllTypes {
  bool _bool;
  int _int;
  //WIN* _WIN;
};
typedef unordered_map<string, AllTypes> ArgMap;
#define DEFARG(type, name, defaut) \
  type name = (args.find(#name) != args.end()) ? args.at(#name)._##type : defaut;

#define get_win_size(win,ty,tx) int ty,tx; getmaxyx(win,ty,tx)
#define get_win_pos(win,y,x) int y,x; getbegyx(win,y,x)

//### General thing (minion, weapon, hero...)------

struct VizPanel {
  WIN* win;
  PANEL* panel;

  VizPanel() : 
    win(nullptr), panel(nullptr) {}

  virtual ~VizPanel();

  virtual WIN* draw(ArgMap& args) = 0;
};

struct VizInstance : public VizPanel {
  const PInstance _obj;

  VizInstance(PInstance obj);
};


struct VizThing : public VizInstance {
  int hp, max_hp, max_card_hp;
  int atq, max_atq, max_card_atq;
  unsigned int effects;
  unsigned int status_id;
  volatile int wait;
  PThing thing()  { return dynamic_pointer_cast<Thing>(_obj); }
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
#undef IS_EFFECT

  VizThing(PThing obj, int ty, int tx, int y, int x);

  void check();

  virtual ~VizThing();

  int buff_color(int val, bool highlight = false, bool standout = false) const;
  int buff_color(const int* val, bool highlight = false, bool standout = false) const;

  virtual WIN* draw(ArgMap& args);

  int& getattr(int VizThing::*what) {
    return (*this).*what;
  }

  bool update_stats(const Msg_Status& msg, bool show_hp = true);
};


//### Hero -----------

struct VizHero : public VizThing {
  int armor;
  PHero hero(){ return dynamic_pointer_cast<Hero>(_obj); }

  VizHero(PHero hero, int y, int x) :
    VizThing(hero, 4, 13, y, x ) {
    armor = hero->state.armor;
  }

  bool check() {
    assert(armor == hero()->state.armor);
    VizThing::check();
  }

  virtual WIN* draw(ArgMap& args) {
    WIN* win = VizThing::draw(args);
    if (!win) return win;
    DEFARG(int, highlight, 0);
    get_win_size(win, ty, tx);
    print_middle(win, 1, 1, tx - 2, hero()->player->name);
    string hero_name = split(hero()->card->name)[0];
    print_middle(win, 2, 1, tx - 2, "(" + hero_name + ")", BLUE_on_BLACK);
    if (armor) {
      string tar = string_format("[%d]", armor);
      mvwaddstr(win, ty - 2, tx - 1 - len(tar), tar);
    }
    VizPlayer* pl = hero()->player->viz;
    int atq = (pl->weapon ? pl->weapon->state.atq : 0) + atq;
    if (atq) {
      int hh = pl->weapon ? pl->weapon->viz->buff_color(&pl->weapon->viz->atq) : buff_color(&atq);
      mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ",atq), highlight | hh);
    }
  }

  VizHeroPowerButton create_hero_power_button() {
    PCardHero card = dynamic_pointer_cast<Card_Hero>(hero()->card);
    get_win_pos(win, y, x);
    auto name = split(card->ability->name);
    if (len(name) == 1)
      name = { name[0].substr(0, 4), name[0].substr(4, 999) };
    string up = name[0];
    string down = name[1];
    return VizHeroPowerButton(y, x + 24, up, down, card->ability->cost, 9, 4);
  }
};


//### Minion -----------

class VizMinion : public VizThing {
  PMinion minion(){ return dynamic_pointer_cast<Minion>(_obj); }

  VizMinion(PMinion minion) :
    VizThing(minion, 5, 11, 
    minion->engine.board->viz->get_minion_y(minion), 
    minion->engine.board->viz->get_minion_x(minion)) {}

  virtual WIN* draw(ArgMap & args) {
    DEFARG(int, x, minion()->engine.board->viz->get_minion_x(minion()));
    //DEFARG(int, y, 0);
    WIN* win = VizThing::draw(args);
    if (!win) return win;
    DEFARG(int, int highlight, 0);
    get_win_size(win, ty, tx);
    string name = minion()->card->get_name_fr();
    print_longtext(win, 1, 1, ty - 1, tx - 1, name, YELLOW_on_BLACK);
    mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", atq), highlight | buff_color(&atq));
    if (is_death_rattle())
      mvwaddstr_ex(win, ty - 1, tx / 2, "D", highlight);
    else if (is_trigger())
      mvwaddstr_ex(win, ty - 1, tx / 2, "Z", highlight);
    if (is_silence()) {
      WIN* line = derwin(win, 1, tx, ty / 2, 0);
      wbkgd(line, 0, BLACK_on_RED);
      mvwchgat(line, 0, 1, tx - 2, 0, BLACK_on_RED);
      delwin(line);
    }
  }
};

struct VizWeapon : public VizThing {
  PWeapon weapon(){ return dynamic_pointer_cast<Weapon>(_obj); }

  VizWeapon(PWeapon weapon) :
    VizThing(weapon, 4, 11, weapon->hero->viz->get_posy(), 
                            weapon->hero->viz->get_posx()-18) {}

  virtual WIN* draw(ArgMap & kwargs) {
    WIN* win = VizThing::draw(args);
    if (!win) return win;
    DEFARG(int, highlight, 0);
    get_win_size(win, ty, tx);
    string name = weapon()->card->get_name_fr();
    print_longtext(win, 1, 1, ty - 1, tx - 1, name, GREEN_on_BLACK);
    int ref_atq = min(self.obj.card.atq, self.max_atq);
    mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", atq), highlight | buff_color(&atq));
  }
  
  bool update_stats(const Msg_Status& msg) {
    return VizThing::update_stats(msg, false);
  }
};


//### Player -----------

struct VizPlayer {
  Player* const player;
  Player::State state;

  VizPlayer(Player* player) :
    player(player) {}

  bool check() const {
    Player* pl = player;
    for (auto card : state.cards)
      assert(in(card, pl->state.cards);
    if (pl->weapon || state.weapon)
      state.weapon->viz->check();
    assert(len(state.secrets) == len(pl->state.secrets));
    assert(len(state.minions) == len(pl->state.minions));
    for (int i = 0; i < len(state.minions); i++) {
      assert(state.minions[i] == pl->state.minions[i]);
      state.minions[i]->viz->check();
    }
  }

  void set_weapon(PWeapon weapon) {
    unset_weapon(state.weapon);
    weapon->viz = NEWP(VizWeapon, weapon);
    state.weapon = weapon;
  }

  void unset_weapon(PWeapon weapon) {
    assert(weapon == state.weapon);
    if (state.weapon)
      state.weapon->viz->reset(nullptr);
  }
};


//### Button -----------

struct VizButton : public VizPanel {
  string text;
  enum Align {
    left, right, center
  };

  VizButton(int y, int x, string text, Align align = center, int tx = 0, int ty = 3) :
    VizPanel(), text(text) {
    if (tx == 0) tx = len(text) + 2;
    switch (align) {
    case left: break;
    case center: x -= tx / 2; break;
    case right: x -= tx; break;
    }
    win = newwin(ty, tx, y, x);
    panel = new_panel(win);
    set_panel_userptr(panel, this);
  }

  ~VizButton() {
    delete_panel(panel);
    win = nullptr;
    panel = nullptr;
  }

  void draw(const ArgMap& args) {
    DEFARG(int, highlight, 0);
    DEFARG(int, bkgd, 0);
    DEFARG(bool, show_box, true);
    DEFARG(int, ytext, 0);
    DEFARG(int, y, 0);
    DEFARG(int, coltext, 0);
    get_win_pos(win, px, py);
    if (y && y != py) move_panel(panel, y, px);
    wbkgd(win, bkgd);
    if (show_box) {
      wattron(win, highlight);
      box(win, ACS_VLINE, ACS_HLINE);
      wattroff(win, highlight);
    }
    get_win_size(win, ty, tx);
    print_middle(win, ytext ? ytext : ty/2, 1, tx - 2, text, coltext);
  }
};

typedef shared_ptr<VizButton> PVizButton;

class VizHeroPowerButton : public VizButton {
  string subtext;
  int cost;
  bool used;

  VizHeroPowerButton(int y, int x, string text, string subtext, int cost = 2) :
    VizButton(y, x, text), subtext(subtext), cost(cost), used(false) {}

  virtual WIN* draw(ArgMap& args) {
    DEFARG(bool, blink, false);
    int coltext = YELLOW_on_BLACK;
    if (used) args['bkgd'] = BLACK_on_YELLOW;
    args["ytext"] = 1; 
    args["coltext"] = coltext;
    VizButton::draw(args);

    if (blink) {
      for (int i = 0; i < 10 * blink; ++i) {
        args['bkgd'] = (i % 2) ? YELLOW_on_BLACK : BLACK_on_YELLOW;
        VizButton::draw(args);
        show_panels();
        Sleep(0.1);
        get_win_size(win, ty, tx);
        mvwaddstr_ex(self.win, 0, tx / 2 - 1, "(%d)"%self.cost, cyan_on_black);
        print_longtext(self.win, 2, 1, ty - 1, tx - 1, self.subtext, coltext);
      }
    }
  }
};

void wait_delete(float duration, PVizButton button, VizThing* viz) {
  t = 0;
  while (t < duration) {
    lock_panels();
    touchwin(button.win);
    top_panel(button.panel); // # remains at top
    release_panels();
    show_panels();
    Sleep(0.1);
    t += 0.1;
  }
  lock_panels();
  delete button;
  release_panels();
  show_panels();
  viz.wait -= 1;
}
void temp_panel(VizThing* viz, string text, int color, float duration = 2) {
  viz->wait += 1;
  get_win_pos(viz->win, y, x);
  get_win_size(viz->win, ty, tx);
  PVizButton button = NEWP(VizButton,y + ty / 2 - 1, x + tx / 2, text);
  button->draw(box = False, bkgd = color);
  wait_delete(duration, button, viz);
  //Thread(target = wait_delete, args = (duration, button, viz)).start();
}


//### Slot -----------
//
//          def get_screen_pos(self) :
//          """ return position, space """
//          NR, NC = getmaxyx(stdscr)
//          n = len(self.owner.viz.minions)
//          top, bot = self.engine.board.viz.get_top_bottom_players()
//          y = 6 if self.owner is top else 14
//          sp = ([3] * 5 + [2, 1, 0])[n] # spacement between minions
//          return (y, int(NC - 3 - (11 + sp)*n) / 2 + (11 + sp)*self.index), sp
//
//          Slot.get_screen_pos = get_screen_pos
//
//          def draw_Slot(self, highlight = 0, bkgd = 0, **kwargs) :
//          if not hasattr(self, "win") :
//            (y, x), sp = self.get_screen_pos()
//            win = self.win = newwin(5, sp, y, x - sp)
//            self.panel = new_panel(win)
//            set_panel_userptr(self.panel, self)
//
//            if bkgd or highlight :
//top_panel(self.panel)
//wbkgd(self.win, bkgd or highlight)
//            else:
//delete_panel(self.panel)
//del self.panel
//del self.win
//
//
//
//### Card -----------
//card_size = (14, 15)
//
//def draw_Card(self, pos = None, highlight = 0, cost = None, small = True, bkgd = 0, hide = False, **kwargs) :
//if hide :
//if hasattr(self, "panel") : hide_panel(self.panel)
//if hasattr(self, "small_panel") : hide_panel(self.small_panel)
//return
//
//name = self.name_fr or self.name
//desc = self.desc_fr or self.desc
//
//if not small :
//ty, tx = card_size
//if hasattr(self, "small_panel") :
//y, x = getbegyx(self.small_win)
//if not pos :
//self.ty = ty
//self.tx = tx
//NR, NC = getmaxyx(stdscr)
//pos = NR - ty, x
//if pos[0]>y:  pos = y, pos[1]  # cannot be below small panel
//if not hasattr(self, "win") :
//self.win = newwin(ty, tx, pos[0], pos[1])
//self.panel = new_panel(self.win)
//set_panel_userptr(self.panel, self)
//win, panel = self.win, self.panel
//top_panel(panel)
//else: # small card version
//small = min(small, card_size[0])
//if hasattr(self, "panel") :
//hide_panel(self.panel)
//if not hasattr(self, "small_win") :
//ty, tx = small, card_size[1]
//assert pos, pdb.set_trace()
//self.small_win = newwin(ty, tx, pos[0], pos[1])
//self.small_panel = new_panel(self.small_win)
//set_panel_userptr(self.small_panel, self)
//elif panel_hidden(self.small_panel) :
//top_panel(self.small_panel)
//win, panel = self.small_win, self.small_panel
//ty, tx = getmaxyx(win)
//if type(small) == int and small != ty : # redo
//delete_panel(panel)
//del self.small_win
//del self.small_panel
//return self.draw(pos = pos, highlight = highlight, cost = cost, small = small, bkgd = bkgd)
//small = getmaxyx(win)[0]
//
//ty, tx = getmaxyx(win)
//if pos and pos != getbegyx(win) :
//move_panel(panel, *pos)
//
//wbkgd(win, bkgd)
//if highlight : wattron(win, highlight)
//box(win)
//if 0<small<card_size[0] :
//  mvwaddch(win, ty - 1, 0, ACS_VLINE)
//  mvwaddch(win, ty - 1, tx - 1, ACS_VLINE)
//  if highlight : wattroff(win, highlight)
//    if 0<small<card_size[0] :
//      mvwaddstr(win, ty - 1, 1, ' '*(tx - 2))
//
//      if issubclass(type(self), Card_Minion) :
//        mid = card_size[0] / 2
//        y, x, h, w = 1, 2, mid - 2, tx - 4
//        manual_box(win, y, x, h, w)
//        print_longtext(win, y + 1, x + 1, y + h - 1, x + w - 1, name, YELLOW_on_BLACK)
//        mvwaddstr(win, y + h - 1, x + 1, "%2d "%self.atq)
//        mvwaddstr(win, y + h - 1, x + w - 4, "%2d "%self.hp)
//        print_longtext(win, mid, 2, ty - 1, tx - 2, desc)
//      else:
//r = 4 if issubclass(type(self), Card_Weapon) else 3
//mvwaddch(win, r, 0, ACS_LTEE, highlight)
//mvwhline(win, r, 1, ACS_HLINE, tx - 2)
//mvwaddch(win, r, tx - 1, ACS_RTEE, highlight)
//if issubclass(type(self), Card_Weapon) :
//name_color = GREEN_on_BLACK
//mvwaddstr(win, r, 2, " %d "%self.atq)
//hpt = " %d "%self.hp
//mvwaddstr(win, r, tx - 2 - len(hpt), hpt)
//else:
//name_color = magenta_on_black
//print_longtext(win, 1, 1, r, tx - 1, name, name_color)
//print_longtext(win, r + 1, 2, ty, tx - 2, desc)
//
//# print cost
//if cost == None:
//cost = self.cost
//if cost == self.cost :
//mvwaddstr(win, 0, 0, "(%d)"%cost, BLACK_on_CYAN)
//elif cost<self.cost :
//mvwaddstr(win, 0, 0, "(%d)"%cost, WHITE_on_GREEN)
//else:
//mvwaddstr(win, 0, 0, "(%d)"%cost, WHITE_on_RED)
//
//
//def card_delete(self) :
//if hasattr(self, 'panel') :
//delete_panel(self.panel)
//del self.win
//del self.panel
//if hasattr(self, 'small_panel') :
//delete_panel(self.small_panel)
//del self.small_win
//del self.small_panel
//
//Card.delete = card_delete
//
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
  SET_ENGINE();
  Board* board;
  const bool switch_heroes;
  const bool animated;
  VizButton end_turn;
  unordered_map<Player*,VizButton> hero_power_buttons;

  VizBoard(Board* board, bool switch_heroes = false, bool animated = true);

//def get_top_bottom_players(self) :
//player = self.engine.get_current_player()
//adv = self.engine.get_other_player()
//if self.switch == False and self.engine.turn % 2 :
//player, adv = adv, player # prevent top / down switching
//return adv, player
//
//def get_minion_pos(self, minion) :
//try :
//index = minion.owner.viz.minions.index(minion)
//except ValueError :
//return None
//slot = Slot(minion.owner, index, None)
//return slot.get_screen_pos()[0]
//
//def get_card_pos(self, card) :
//NR, NC = getmaxyx(stdscr)
//nc = len(card.owner.viz.cards) + 1e-8
//ty, tx = card_size
//actual_NC = min(NC, tx*nc)
//startx = int((NC - actual_NC) / 2)
//i = card.owner.viz.cards.index(card)
//return 24, startx + int((actual_NC - tx)*i / (nc - 1))
//
//def get_hero_pos(self, player) :
//NR, NC = getmaxyx(stdscr)
//top, bot = self.get_top_bottom_players()
//if player == top : return (1, (NC - 18) / 2)
//if player == bot : return (20, (NC - 18) / 2)
//assert False
//
//def draw(self, what = 'bkgd decks hero cards mana minions', which = None, last_card = True) :
//NR, NC = getmaxyx(stdscr)
//
//# clear screen
//adv, player = self.get_top_bottom_players()
//which = { player, adv } if not which else { which }
//assert all([issubclass(type(pl), Player) for pl in which]), debug()
//
//if 'bkgd' in what :
//# background
//erase()
//mvhline(2, 0, ACS_CKBOARD, NC)
//mvhline(3, 0, ACS_CKBOARD, NC)
//mvhline(12, 0, ord('-'), NC)
//mvhline(21, 0, ACS_CKBOARD, NC)
//mvhline(22, 0, ACS_CKBOARD, NC)
//self.end_turn.draw()
//# draw hero power
//for pl in which :
//self.hero_power_buttons[pl].draw(y = self.get_hero_pos(pl)[0])
//
//# draw decks on the right side
//if 'decks' in what :
//for i in range(3) :
//mvvline(2, NC - 1 - i, ACS_CKBOARD, 20)
//for i in[5, 15] :
//mvaddch(i, NC - 2, ACS_HLINE)
//addwch(9558)
//text = ' %2d'%len(i<12 and adv.deck or player.deck)
//for j, ch in enumerate(text) :
//mvaddch(i + 1 + j, NC - 2, ord(ch))
//addwch(9553)
//mvaddch(i + 4, NC - 2, ACS_HLINE)
//addwch(9564)
//
//# draw heroes
//if 'hero' in what :
//for pl in which :
//pl.hero.draw(y = self.get_hero_pos(pl)[0])
//if pl.viz.weapon :
//pl.viz.weapon.draw(y = self.get_hero_pos(pl)[0])
//
//# draw cards
//if 'cards' in what :
//if adv in which :
//for card in adv.viz.cards[:None if last_card else - 1] :
//card.draw(hide = True)  # hide adversary cards
//print_middle(stdscr, 0, 0, NC, " Adversary has %d cards. "%len(adv.cards))
//if player in which :
//for card in player.viz.cards[:None if last_card else - 1] :
//card.draw(pos = self.get_card_pos(card), small = NR - 24)
//
//# draw mana
//if 'mana' in what :
//for who, i in[(adv, 2), (player, 22)] :
//if who not in which : continue
//p = i<12 and adv or player
//text = "%2d/%d " % (p.mana, p.max_mana)
//mvaddstr(i, NC - 11 - len(text), text, BLACK_on_CYAN)
//addwch(9830, cyan_on_black, nb = p.mana)
//addwch(9826, cyan_on_black, nb = p.max_mana - p.mana)
//
//# draw minions
//if 'minions' in what :
//for pl in which :
//for m in pl.viz.minions :
//m.draw(y = self.get_minion_pos(m)[0])
//
//show_panels()
//
//
//# attach each show function to a message
//all_globs = globals().keys()
//draw_funcs = [key for key in all_globs if key.startswith("draw_")]
//for key in draw_funcs :
//if key[5:] in all_globs :
//setattr(globals()[key[5:]], "draw", globals()[key])
};


//# Overload human interface
#include "players.h"

struct HumanPlayerCurses : public HumanPlayer {
  //  ''' human player : ask the player what to do'''

  static bool mouse_in_win(WIN* win, int y, int x) {
    int wy, wx; getbegyx(win, wy, wx);
    int height, width; getmaxyx(win, height, width);
    return wx <= x && x < wx + width && wy <= y && y < wy + height;
  }

  virtual void mulligan(ListCard cards);

  virtual Action* choose_actions(ListAction actions);
};


//# overloaded HS engine
//from hs_engine import HSEngine
//
//class CursesHSEngine(HSEngine) :
//  ''' overload display functions '''
//  def __init__(self, *args) :
//  HSEngine.__init__(self, *args)
//  self.display = []
//  self.log = ''
//  self.logfile = open('log.txt', 'w')
//
//  def display_msg(self, msg) :
//  self.display.append(msg)
//
//  def wait_for_display(self) :
//  while self.display :
//    msg = self.display.pop(0)
//    if msg.draw() == False :
//      self.display.insert(1, msg)  # needs to be displayed later
//    else :
//    line = "[%s] %s\n" % (type(msg).__name__, msg)
//    self.logfile.write(line)
//    self.logfile.flush()
//    self.log += line
//
//
//    def dbg_add_minion(player, card) :
//    from copy import deepcopy
//    card = deepcopy(card)
//    card.owner = player
//    engine.send_message(Msg_AddMinion(player, Minion(card), pos = engine.board.get_free_slots(player)[0]))
//


#endif
































