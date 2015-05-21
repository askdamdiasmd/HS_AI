#include "curses_interface.h"
#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>
#else
#endif
#include "Cards.h"
#include "messages.h"
#include "Board.h"
#include "decks.h"

// Thread definition and handling
#ifdef _WIN32
#define DECLARE_THREAD(name, params)  \
  struct name##_args_t { params; }; \
  DWORD WINAPI name(LPVOID lpParam) { \
    name##_args_t* args = (name##_args_t*)lpParam;
#ifndef DONT_USE_THREADS
#define CLOSE_THREAD(ret_value) \
  delete args; \
  return ret_value;}
#define START_THREAD(name,...) \
    {name##_args_t* name##args = new name##_args_t {##__VA_ARGS__}; \
  CreateThread(NULL,0,name,name##args,0,NULL);}
#else
#define CLOSE_THREAD(ret_value) \
  return ret_value;}
#define START_THREAD(name,...) \
  {name##_args_t name##args = { ##__VA_ARGS__ }; \
  name(&name##args);}
#endif
#else
#error
#endif

#define SETWATTR(win,attr) \
  int oldattr = 0; \
  if (attr) {\
    oldattr = getattrs(win);\
    wattrset(win, attr);\
      }
#define UNSETWATTR(win,attr) \
  if (attr) wattrset(win, oldattr);

#define get_win_size(win,ty,tx) assert(win); const int ty = getmaxy(win), tx = getmaxx(win)
#define get_win_pos(win,y,x) assert(win); const int y = getbegy(win), x = getbegx(win)

static pos_t nullpos{ -1, -1 };

#define DEFARG(type, name, defaut) \
  type name = (args.find(#name) != args.end()) ? args.at(#name)._##type : defaut;

#define UInt(i) {i}
#define KEYINT(key, val)    { key, UInt(val) } // only works if val is int
static inline AllTypes Ufloat(float pos) {
  AllTypes res;
  res._float = pos;
  return res;
}
#define KEYFLOAT(key, val)    { key, Ufloat(val) } // only works if val is int

static inline AllTypes UPos(pos_t pos) {
  AllTypes res;
  res._pos_t = pos;
  return res;
}
#define KEYPOS1(key, pos)   { key, UPos(pos) }
#define KEYPOS2(key, y, x)  { key, UPos( {y, x} ) }

static inline AllTypes UBool(bool b) {
  AllTypes res;
  res._bool = b;
  return res;
}
#define KEYBOOL(key, b)  { key, UBool(b) }

#ifdef _WIN32
// Mutex are BAD because they can't block the same thread
static HANDLE draw_panel_lock = CreateSemaphore(NULL, 1, 1, NULL);
void lock_panels(HANDLE lock) {
  WaitForSingleObject(lock, INFINITE);
}
void unlock_panels(HANDLE lock) {
  ReleaseSemaphore(lock, 1, NULL);
}
#else
todo...
#endif

// draw panel lock
//#define LOCKP {TRACE("LOCKD at line %d in func %s\n",__LINE__,__FUNCDNAME__); lock_panels(draw_panel_lock);}
//#define UNLOCKP {unlock_panels(draw_panel_lock);TRACE("UNLOCKD at line %d in func %s\n",__LINE__,__FUNCDNAME__);}
#define LOCKP   lock_panels(draw_panel_lock)
#define UNLOCKP unlock_panels(draw_panel_lock)
// delete panel lock
#define LOCKD LOCKP //{LOCKP; lock_panels(del_panel_lock);}
#define UNLOCKD UNLOCKP //{unlock_panels(del_panel_lock); UNLOCKP;}

void show_panels(bool lock=true) {
  if(lock)  LOCKP;
  update_panels();
  doupdate();
  if (lock)  UNLOCKP;
}

#define DELETE_PANEL(win, panel) { \
  LOCKD;  \
  del_panel(panel); \
  panel = nullptr;  \
  delwin(win);  \
  win = nullptr;  \
  UNLOCKD;  \
}

/*
static void extend_console(int min_NR, int min_NC) {
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(console, &info);
  //GetWindowRect(console, &rect);
  SMALL_RECT rect = info.srWindow;
  if (rect.Bottom<min_NR - 1)
    rect.Bottom = min_NR - 1;
  if (rect.Right<min_NC - 1)
    rect.Right = min_NC - 1;
  SetConsoleWindowInfo(console, TRUE, &rect);
}
*/

static unordered_map<string, int> str_to_color;

void init_screen() {
  //extend_console(30, 80);
  initscr();

  int NR, NC;
  getmaxyx(stdscr, NR, NC);
  resize_term(max(NR,30), max(NC,80));
  getmaxyx(stdscr, NR, NC);

  if (NR < 30) {
    endwin();
    fprintf(stderr, "error: screen not high enough (min 30 lines)");
    assert(0);
    exit(-1);
  }
  else if (NC < 80) {
    endwin();
    fprintf(stderr, "error: screen not wide enough (min 80 columns)");
    assert(0);
    exit(-1);
  }

  start_color();
  cbreak();
  curs_set(0);
  noecho();
  keypad(stdscr, true);
  mouseinterval(0);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
  timeout(10); // check every 10 ms

  // init color pairs
#define create_color_pair(num, col_fg,col_bg)  \
  init_pair(num, COLOR_##col_fg, COLOR_##col_bg); \
  str_to_color[#col_fg "_on_" #col_bg] = COLOR_PAIR(num)
  create_color_pair(1, CYAN, BLACK);
#define CYAN_on_BLACK     COLOR_PAIR(1)
  create_color_pair(2, YELLOW, BLACK);
#define YELLOW_on_BLACK   COLOR_PAIR(2)
  create_color_pair(3, MAGENTA, BLACK);
#define MAGENTA_on_BLACK  COLOR_PAIR(3)
  create_color_pair(4, BLUE, BLACK);
#define BLUE_on_BLACK     COLOR_PAIR(4)
  create_color_pair(5, RED, BLACK);
#define RED_on_BLACK      COLOR_PAIR(5)
  create_color_pair(6, GREEN, BLACK);
#define GREEN_on_BLACK    COLOR_PAIR(6)
  create_color_pair(7, WHITE, BLACK);
#define WHITE_on_BLACK    COLOR_PAIR(7)
  create_color_pair(8, WHITE, RED);
#define WHITE_on_RED      COLOR_PAIR(8)
  create_color_pair(9, WHITE, GREEN);
#define WHITE_on_GREEN    COLOR_PAIR(9)
  create_color_pair(10, BLACK, WHITE);
#define BLACK_on_WHITE    COLOR_PAIR(10)
  create_color_pair(11, BLACK, GREEN);
#define BLACK_on_GREEN    COLOR_PAIR(11)
  create_color_pair(12, BLACK, RED);
#define BLACK_on_RED      COLOR_PAIR(12)
  create_color_pair(13, BLACK, CYAN);
#define BLACK_on_CYAN     COLOR_PAIR(13)
  create_color_pair(14, BLACK, BLUE);
#define BLACK_on_BLUE     COLOR_PAIR(14)
  create_color_pair(15, BLACK, YELLOW);
#define BLACK_on_YELLOW   COLOR_PAIR(15)
#undef create_color_pair
}

int my_getch() {
  // because we use a non-blocking getch()
  int ch;
  while ((ch = getch()) == ERR);
  return ch;
}

void congratulate_winner(const Player* winner, int turn) {
  int NC = getmaxx(stdscr);
  VizButton button(10, NC / 2 - 3, string_format("  %s wins after %d turns!  ", winner->name.c_str(), (turn + 1) / 2), VizButton::center, 5);
  button.draw({ KEYINT("highlight", BLACK_on_YELLOW) });
  show_panels();
  my_getch();
}

void end_screen() {
  endwin();
}

static int mvwaddstr_ex(WINDOW* scr_id, int y, int x, const char* cstr, int attr = 0) {
#ifdef _WIN32
  int oldattr = getattrs(scr_id); 
  wattrset(scr_id, attr);
  int ret = mvwaddstr(scr_id, y, x, cstr);
  wattrset(scr_id, oldattr);
  return ret;
#else
  if (attr) return scr_id.addstr(y, x, str(cstr), attr)
    return scr_id.addstr(y, x, str(cstr))
#endif
}

static void addwch_ex(chtype ch, int attr = 0, int nb = 1, WINDOW* win = nullptr, int y = -1, int x = -1) {
  if (!win) win = stdscr;
  // display unicode character
  if (x < 0)
    getyx(win, y, x);
  else
    wmove(win, y, x);
  whline(win, ch | attr, nb);
  wmove(win, y, x + nb);
}

static void clear_rect(WINDOW* win, int y, int x, int h, int w) {
  for (int j = 0; j < h; ++j)
    addwch_ex(getbkgd(stdscr), 0, w, stdscr, y + j, x);
}

static void mvwchgat_ex(WINDOW* scr_id, int y, int x, int num, attr_t attr, int color) {
#ifdef _WIN32
  mvwchgat(scr_id, y, x, num, attr, color, nullptr);
#else
  scr_id.chgat(y, x, num, attr | color_pair(color));
#endif
}

static void mvwaddch_ex(WINDOW* scr_id, int y, int x, chtype ch, attr_t attr = A_NORMAL) {
#ifdef _WIN32
  mvwaddch(scr_id, y, x, ch | attr);
#else
  scr_id.addch(y, x, ch, attr);
#endif
}

static int getmouse_ex(MEVENT* event) {
#ifdef _WIN32
  int res = nc_getmouse(event);
#else
  todo
#endif
  return res;
}

static void print_middle(WINDOW* win, int y, int x, int width, string text, int attr = 0) {
  if (!win) win = stdscr;
  x = max(x, x + (width - len(text) + 1) / 2);
  if (len(text) > width)  text = text.substr(0, width);

  SETWATTR(win, attr);
  mvwaddstr(win, y, x, text.c_str());
  UNSETWATTR(win, attr);
}

static int print_longtext(WINDOW* win, int y, int x, int endy, int endx, string text, int attr = 0) {
  if (!win) win = stdscr;
  //assert(x >= 0 && y >= 0 && endx > x && endy > y);
  vector<string> words = split(text);

  int width = endx - x;
  while (!words.empty() && y < endy) {
    // pick enough text to fill one line
    string line = pop_front(words);

    while (!words.empty() && len(line + words[0]) + 1<width)
      line += " " + pop_front(words);

    if (len(line) > width) {
      words.insert(words.begin(), "-" + line.substr(width, 999));
      line = line.substr(0, width);
    }
    print_middle(win, y, x, width, line, attr);
    y++;
  }
  return 0;
}

static PANEL* my_top_panel() {
  // due to a bug in unicurses we recode it here
#ifdef _WIN32
  return panel_below(nullptr); // get top panel
#else
  NI; return nullptr; // curses.panel.top_panel();
#endif
}

static void show_ACS() {
  /* int NR, NC;
  getmaxyx(stdscr, NR, NC);
  acs = [ch for ch in dir(uc) if ch.startswith('ACS_')];
  i = 0;
  for ch in acs :
  mvhline(i, 0, eval('' + ch), NC);
  mvaddstr(i, 0, ch);
  i += 1;
  if i == NR :
  refresh();
  my_getch();
  clear();
  i = 0;
  refresh();
  my_getch();
  clear();*/
}

static void show_unicode() {
  clear();
  int NR, NC;
  getmaxyx(stdscr, NR, NC);
  for (int i = 1; i < 65536; ++i) {
    waddch(stdscr, i);
    if ((i + 1) % (NR*NC) == 0) {
      refresh();
      my_getch();
      clear();
    }
  }
  refresh();
  my_getch();
  clear();
}

static void strong_box(WINDOW* win, int attr = 0) {
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

static void weak_box(WINDOW* win, int attr = 0) {
  wborder(win, ':', ':', '-', '-', ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
}

static void manual_box(WINDOW* win, int y, int x, int h, int w) {
  w--; h--;
  mvwaddch(win, y, x, ACS_ULCORNER);
  whline(win, ACS_HLINE, w - 1);
  mvwaddch(win, y, x + w, ACS_URCORNER);
  mvwvline(win, y + 1, x, ACS_VLINE, h - 1);
  mvwvline(win, y + 1, x + w, ACS_VLINE, h - 1);
  mvwaddch(win, y + h, x, ACS_LLCORNER);
  whline(win, ACS_HLINE, w - 1);
  mvwaddch(win, y + h, x + w, ACS_LRCORNER);
}


//# Viz classes = copy object specs --------------------

Engine* VizPanel::engine = nullptr;

void VizPanel::hide() {
  if (panel) {
    LOCKP;
    bottom_panel(panel);
    hide_panel(panel);
    UNLOCKP;
  }
}

VizPanel::~VizPanel() {
  if (win && panel)
    DELETE_PANEL(win, panel);
}

//### General thing (minion, weapon, hero...)------

VizInstance::VizInstance(const PInstance obj) :
  VizPanel(), obj(obj), card(obj->card), 
  name(obj->card->get_name_fr()) {}

PVizThing Thing::viz_thing() { return CASTP(viz, VizThing); }

PConstCardThing VizThing::card_thing() const { 
  return CASTP(card, const Card_Thing); 
}

VizThing::VizThing(const PThing obj, int ty, int tx, int y, int x) :
  VizInstance(obj), state(obj->state) {
  // create panel
  win = newwin(ty, tx, y, x);
  assert(win);
  panel = new_panel(win);
  set_panel_userptr(panel, this);
}

//bool VizThing::check() {
//  const Thing* obj = thing();
//  assert(state.hp == obj->state.hp);
//  assert(state.max_hp == obj->state.max_hp);
//  assert(state.atq == obj->state.atq);
//  assert(state.max_atq == obj->state.max_atq);
//  const int nocare = ~(Thing::StaticEffect::fresh); // don't care about these
//  assert((nocare&state.static_effects) == (nocare&obj->state.static_effects));
//  return true;
//}

int VizThing::get_hgh(const hgh_time_out* hgh) const {
  if (VizBoard::now() < hgh->expire)
    return hgh->hgh;
  else
    return 0;
}

int VizThing::buff_color(int val, bool highlight) const {
  int res;
  if (val>0)
    res = highlight ? WHITE_on_GREEN : GREEN_on_BLACK;
  else if (val == 0)
    res = highlight ? WHITE_on_BLACK : 0;
  else if (val < 0)
    res = highlight ? WHITE_on_RED : RED_on_BLACK;
  return res;
}

int VizThing::buff_color(const int* val, int max_card_val, bool highlight) const {
  int max_val;
  if (val == &state.hp) {
    max_val = state.max_hp;
    if(max_card_val<0)  max_card_val = card_thing()->thing()->state.max_hp;
  }
  else if (val == &state.atq) {
    max_val = state.max_atq;
    if (max_card_val<0)  max_card_val = card_thing()->thing()->state.max_atq;
  }
  else if (val == &state.armor) {
    if (max_card_val<0)  max_card_val = 999999;
  }
  else assert(!"error: unrecognized val");
  int res;
  if (*val < max_val)
    res = highlight ? WHITE_on_RED : RED_on_BLACK;
  else if (*val > max_card_val)
    res = highlight ? WHITE_on_GREEN : GREEN_on_BLACK;
  else
    res = highlight ? BLACK_on_WHITE : 0;
  return res;
}

DECLARE_THREAD(wait_and_draw, PVizInstance thing; float duration)
  VizBoard::sleep(args->duration);
  args->thing->draw({});
  ungetch(1); // refresh actions if needed
CLOSE_THREAD(0)

void VizThing::update_state(const Thing::State& copy, bool show_diff) {  
  const float duration = 1;
  Thing::State before = state;  // remember so that we can compare later

  // update attribute
  state = copy;

  int* can_change[] = { &state.max_hp, &state.hp, &state.atq, &state.armor };
  hgh_time_out* hgh_tags[] = { &hgh_hp, &hgh_hp, &hgh_atq, &hgh_armor };
  const bool anim = show_diff && engine->board.viz->animated;
  bool changed = false, change_max_hp = false;
  for (int i = 0; i < sizeof(can_change) / sizeof(*can_change); i++) {
    int* val = can_change[i];
    int oldval = *(((int*)&before) + (val - (int*)&state));
    int newval = *val;
    int diff = newval - oldval;
    // animation if change
    if (anim && diff) {
      if (val == &state.max_hp) 
        change_max_hp = true;
      else {
        if (val == &state.hp && !change_max_hp) 
          temp_panel(this, string_format("%+d", diff), buff_color(diff, true), 1.5);
        changed = true;
        hgh_tags[i]->hgh = A_BOLD;
        hgh_tags[i]->expire = VizBoard::now() + duration;
      }
    }
  }
  if (anim && changed) {
    draw({}); // highlight some stuffs
    show_panels();  // show them
    START_THREAD(wait_and_draw, obj->viz, duration+0.1f); // and wait a bit
  }
  else
    draw({});
}

WINDOW* VizThing::draw(const ArgMap& args) {
  DEFARG(pos_t, pos, nullpos);
  DEFARG(int, y, 0);
  DEFARG(int, bkgd, 0);
  DEFARG(int, highlight, 0);
  LOCKP;
  if (!win) {UNLOCKP;  return win;}
  get_win_pos(win, py, px);
  if (pos != nullpos && pos != pos_t(py, px))
    move_panel(panel, pos.y, pos.x);
  else if (y && y != py)
    move_panel(panel, y, px);

  if (is_frozen() || is_insensible())
    bkgd = BLACK_on_CYAN;
  else if (is_divine_shield())
    bkgd = BLACK_on_YELLOW;

  wbkgd(win, bkgd);
  wattron(win, highlight);
  if (is_stealth())
    weak_box(win, highlight);
  else if (is_taunt())
    strong_box(win, highlight);
  else
    box(win, ACS_VLINE, ACS_HLINE);
  wattroff(win, highlight);
  // show just HP
  get_win_size(win, ty, tx);
  string thp = string_format(" %d ", state.hp);
  mvwaddstr_ex(win, ty - 1, tx - 1 - len(thp), thp.c_str(), get_hgh(&hgh_hp) | buff_color(&state.hp));
  UNLOCKP;
  return win;
}


//### Hero -----------

PVizHero Hero::viz_hero() { return issubclassP(viz, VizHero); }

PConstCardHero VizHero::card_thing() const { 
  return CASTP(card, const Card_Hero);
}

VizHero::VizHero(const PHero hero, pos_t pos) :
  VizThing(hero, 4, 13, pos.y, pos.x), 
  player_name(hero->player->name), hero_name(split(hero->card->name)[0]) {}

WINDOW* VizHero::draw(const ArgMap& args) {
  VizThing::draw(args);
  DEFARG(int, highlight, 0);
  LOCKP;
  if (!win) { UNLOCKP;  return win; }
  get_win_size(win, ty, tx);
  print_middle(win, 1, 1, tx - 2, player_name);
  print_middle(win, 2, 1, tx - 2, "(" + hero_name + ")", BLUE_on_BLACK);
  if (state.armor) {
    string tar = string_format("[%d]", get_hgh(&hgh_armor) | state.armor);
    mvwaddstr(win, ty - 2, tx - 1 - len(tar), tar.c_str());
  }
  if (state.atq) {
    mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", state.atq).c_str(), 
                 get_hgh(&hgh_atq) | buff_color(&state.atq, w_max_atq));
  }
  UNLOCKP;
  return win;
}

PVizHeroPowerButton VizHero::create_hero_power_button() {
  PConstCardHero card = hero()->card_hero();
  get_win_pos(win, y, x);
  auto name = split(card->ability->name);
  if (len(name) == 1)
    name = { name[0].substr(0, 4), name[0].substr(4, 999) };
  string up = name[0];
  string down = name[1];
  PVizHero me = CONSTCAST(hero(),Hero)->viz_hero();
  assert(me.get() == this);
  return NEWP(VizHeroPowerButton, me, y, x + 24, up, down, card->ability->cost);
}

//### Minion -----------

const pos_t VizMinion::size = {5, 11};

PVizMinion Minion::viz_minion() { return CASTP(viz, VizMinion); }

VizMinion::VizMinion(const PMinion minion, VizSlot pos) :
  VizThing(minion, size.y, size.x, pos.get_screen_pos().y, pos.get_screen_pos().x-6) {}

WINDOW* VizMinion::draw(const ArgMap& args) {
  VizThing::draw(args);
  DEFARG(int, x, getbegx(win));
  DEFARG(int, highlight, 0);
  LOCKP;
  if (!win) { UNLOCKP;  return win; }
  get_win_size(win, ty, tx);
  print_longtext(win, 1, 1, ty - 1, tx - 1, name, YELLOW_on_BLACK);
  mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", state.atq).c_str(), 
               get_hgh(&hgh_atq) | buff_color(&state.atq));
  if (is_death_rattle())
    mvwaddstr_ex(win, ty - 1, tx / 2, "D", highlight);
  else if (is_trigger())
    mvwaddstr_ex(win, ty - 1, tx / 2, "Z", highlight);
  if (is_silenced()) {
    WINDOW* line = derwin(win, 1, tx, ty / 2, 0);
    wbkgd(line, BLACK_on_RED);
    mvwchgat_ex(line, 0, 1, tx - 2, 0, BLACK_on_RED);
    delwin(line);
  }
  UNLOCKP;
  return win;
}

PVizWeapon Weapon::viz_weapon() { return issubclassP(viz, VizWeapon); }

VizWeapon::VizWeapon(const PWeapon weapon) :
VizThing(weapon, 4, 11, getbegy(weapon->hero()->viz->win), 
                        getbegx(weapon->hero()->viz->win) - 18) {}

WINDOW* VizWeapon::draw(const ArgMap & args) {
  VizThing::draw(args);
  DEFARG(int, highlight, 0);
  LOCKP;
  if (!win) { UNLOCKP;  return win; }
  get_win_size(win, ty, tx);
  print_longtext(win, 1, 1, ty - 1, tx - 1, name, GREEN_on_BLACK);
  mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", state.atq).c_str(), 
               get_hgh(&hgh_atq) | buff_color(&state.atq));
  UNLOCKP; 
  return win;
}

void VizWeapon::update_state(const Thing::State& from) {
  VizThing::update_state(from, false);
}

//### Player -----------

VizPlayer::VizPlayer(Player* player) :
  player(player), state(player->state) {}

//bool VizPlayer::check() const {
//  const Player::State& pl_state = player->state;
//  assert(state.mana == pl_state.mana);
//  assert(state.max_mana == pl_state.max_mana);
//  assert(state.cards == pl_state.cards);
//  assert(state.hero == pl_state.hero);
//  if (pl_state.weapon || state.weapon) {
//    assert(pl_state.weapon == state.weapon);
//    state.weapon->viz_weapon()->check();
//  }
//  assert(state.secrets == pl_state.secrets);
//  assert(state.minions == pl_state.minions);
//  for (auto& m: pl_state.minions)
//    m->viz_minion()->check();
//  return true;
//}

void VizPlayer::update_state(const Player::State& from) {
  state = from; // copy
  player->engine->board.viz->draw(VizBoard::all,player);
}


//### Button -----------

VizButton::VizButton(int y, int x, string text, Align align, int ty, int tx ) :
VizPanel(), text(text) {
  if (tx == 0) tx = len(text) + 2;
  switch (align) {
  case left: break;
  case center: x -= tx / 2; break;
  case right: x -= tx; break;
  }
  win = newwin(ty, tx, y, x);
  assert(win);
  panel = new_panel(win);
  set_panel_userptr(panel, this);
}

WINDOW* VizButton::draw(const ArgMap& args) {
  DEFARG(int, highlight, 0);
  DEFARG(int, bkgd, 0);
  DEFARG(bool, show_box, true);
  DEFARG(int, ytext, 0);
  DEFARG(int, y, 0);
  DEFARG(int, coltext, 0);
  LOCKP;
  get_win_pos(win, py, px);
  if (y && y != py) move_panel(panel, y, px);
  wbkgd(win, bkgd);
  if (show_box) {
    wattron(win, highlight);
    box(win, ACS_VLINE, ACS_HLINE);
    wattroff(win, highlight);
  }
  get_win_size(win, ty, tx);
  print_middle(win, ytext ? ytext : ty / 2, 1, tx - 2, text, coltext);
  UNLOCKP;
  return win;
}

VizHeroPowerButton::VizHeroPowerButton(PVizHero viz_hero, int y, int x, string text, string subtext, int cost) :
  VizButton(y, x, text, VizButton::center, 4, 9), 
  viz_hero(viz_hero), subtext(subtext), cost(cost) {}

WINDOW* VizHeroPowerButton::draw(const ArgMap& args) {
  DEFARG(float, blink, 0);
  int coltext = YELLOW_on_BLACK;
  ArgMap kwargs = args;
  if (viz_hero->state.n_remaining_power==0) // used
    kwargs["bkgd"]._int = BLACK_on_YELLOW;
  kwargs["ytext"]._int = 1;
  kwargs["coltext"]._int = coltext;
  VizButton::draw(kwargs);
  if (blink) {
    const double until = VizBoard::now() + blink;
    for (int i = 0; VizBoard::now() < until; ++i) {
      kwargs["bkgd"]._int = (i % 2) ? YELLOW_on_BLACK : BLACK_on_YELLOW;
      VizButton::draw(kwargs);
      show_panels();
      VizBoard::sleep(0.1);
    }
  }
  LOCKP;
  get_win_size(win, ty, tx);
  mvwaddstr_ex(win, 0, tx / 2 - 1, string_format("(%d)", cost).c_str(), CYAN_on_BLACK);
  print_longtext(win, 2, 1, ty - 1, tx - 1, subtext, coltext);
  UNLOCKP;
  return win;
}

DECLARE_THREAD(wait_delete, float duration; PVizButton button)
  const double until = VizBoard::now() + args->duration;
  while (VizBoard::now() < until) {
    LOCKP;
    touchwin(args->button->win);
    top_panel(args->button->panel); // # remains at top
    UNLOCKP;
    show_panels();
    VizBoard::sleep(0.1);
  }
  args->button.reset();
  show_panels();
  ungetch(1); // refresh actions if needed
CLOSE_THREAD(0);

void temp_panel(VizThing* viz, string text, int color, float duration) {
  get_win_pos(viz->win, y, x);
  get_win_size(viz->win, ty, tx);
  PVizButton button = NEWP(VizButton, y + ty / 2 - 1, x + tx / 2, text);
  button->draw({ KEYBOOL("box", false), KEYINT("bkgd", color) });
  START_THREAD(wait_delete, duration, button);
}


//### Slot -----------

  // return position, space
pos_t VizSlot::get_screen_pos() const {
  get_win_size(stdscr, NR, NC);
  pos_t res;
  int n = len(slot.player->viz->state.minions);
  int sp = get_screen_space();
  res.x = int(NC - 3 - (11 + sp)*n) / 2 + (11 + sp)*slot.pos;
  res.y = slot.player == slot.player->engine->board.viz->get_top_bottom_player(true) ? 6 : 14;
  return res;
}
pos_t VizSlot::get_center() const {
  pos_t pos = get_screen_pos();
  return pos_t(pos.y + VizMinion::size.y / 2, pos.x + 1);
}
int VizSlot::get_screen_space() const {
  int n = len(slot.player->viz->state.minions);
  return min(3, 7 - n); // spacement between minions
}

WINDOW* VizSlot::draw(const ArgMap& args) {
  DEFARG(int, highlight, 0);
  DEFARG(int, bkgd, 0);
  LOCKP;
  if (!win) {
    pos_t pos = get_screen_pos();
    int sp = get_screen_space();
    win = newwin(5, sp, pos.y, pos.x - sp);
    assert(win);
    panel = new_panel(win);
    set_panel_userptr(panel, this);
  }

  if (bkgd || highlight) {
    top_panel(panel);
    wbkgd(win, bkgd ? bkgd : highlight);
    UNLOCKP;
  }
  else {
    UNLOCKP;
    VizPanel::~VizPanel();
  }
  return win;
}


//### Card -----------

const pos_t VizCard::card_size = { 14, 15 };

VizCard::VizCard(PCard card) :
  VizPanel(), card(card), small_win(nullptr), small_panel(nullptr) {
  cost = card->cost;
}

WINDOW* VizCard::draw(const ArgMap& args) {
  DEFARG(pos_t, pos, nullpos);
  DEFARG(int, highlight, 0);
  DEFARG(int, cost, -1);
  DEFARG(bool, petit, true);  // show small_panel
  DEFARG(int, petit_size, 0); // height of small_panel
  DEFARG(int, bkgd, 0);
  DEFARG(bool, hide, false);
  
  LOCKP;
  if (hide) {
    if (panel) hide_panel(panel);
    if (small_panel) hide_panel(small_panel);
    UNLOCKP;
    return win;
  }

  get_win_size(stdscr, NR, NC);
  string name = card->get_name_fr();
  string desc = card->get_desc_fr();

  WINDOW* win = nullptr;
  PANEL* panel = nullptr;
  if (!petit || petit_size>=card_size.y) {
    int ty = card_size.y, tx = card_size.x;
    if (this->small_panel) {
      // set position based on petit card
      get_win_pos(this->small_win, y, x);
      if (pos.y < 0) pos.y = NR - ty, pos.x = x;
      if (pos.y > y)  pos.y = y; // cannot be below petit panel
      hide_panel(this->small_panel);
    }
    // create big win
    if (!this->win) {
      this->win = newwin(ty, tx, pos.y, pos.x);
      assert(this->win);
      this->panel = new_panel(this->win);
      set_panel_userptr(this->panel, this);
    }
    win = this->win;
    panel = this->panel;
    top_panel(panel);
  }
  else {
    // petit card version
    petit_size = min(petit_size, card_size.y);
    if (this->panel)
      hide_panel(this->panel);
    if (!this->small_win) {
      int ty = petit_size, tx = card_size.x;
      assert(pos != nullpos);
      this->small_win = newwin(ty, tx, pos.y, pos.x);
      assert(this->small_win);
      this->small_panel = new_panel(this->small_win);
      set_panel_userptr(this->small_panel, this);
    }
    else if (panel_hidden(this->small_panel)==OK)  // return 0 if true...
      top_panel(this->small_panel);
    win = this->small_win;
    panel = this->small_panel;

    get_win_size(win, ty, tx);
    if (petit_size>0 && petit_size != ty) {
      UNLOCKP;
      DELETE_PANEL(this->small_win, this->small_panel);
      ArgMap kwargs = args;
      kwargs["petit"]._bool = true;
      kwargs["petit_size"]._int = petit_size;
      kwargs["pos"]._pos_t = pos;
      return draw(kwargs);
    }
    petit_size = getmaxy(win);
  }

  get_win_size(win, ty, tx);
  assert(ty < 20);
  get_win_pos(win, y, x);
  assert(ty > 1);
  if (pos != nullpos && pos != pos_t(y, x))
    move_panel(panel, pos.y, pos.x);

  wbkgd(win, bkgd);
  if (highlight) wattron(win, highlight);
  box(win, ACS_VLINE, ACS_HLINE);
  if (0 < petit_size && petit_size < card_size.y) {
    mvwaddch(win, ty - 1, 0, ACS_VLINE);
    mvwaddch(win, ty - 1, tx - 1, ACS_VLINE);
  }
  if (highlight) wattroff(win, highlight);
  if (0 < petit_size && petit_size < card_size.y)
    mvwaddstr(win, ty - 1, 1, string(tx - 2, ' ').c_str());

  if (issubclassP(card, Card_Minion)) {
    int mid = card_size.y / 2;
    int y = 1, x = 2, h = mid - 2, w = tx - 4;
    manual_box(win, y, x, h, w);
    print_longtext(win, y + 1, x + 1, y + h - 1, x + w - 1, name, YELLOW_on_BLACK);
    PConstMinion m = issubclassP(card, Card_Minion)->minion();
    mvwaddstr(win, y + h - 1, x + 1, string_format("%2d ", m->state.atq).c_str());
    mvwaddstr(win, y + h - 1, x + w - 4, string_format("%2d ", m->state.hp).c_str());
    print_longtext(win, mid, 2, ty - 1, tx - 2, desc);
  }
  else {
    int r = issubclassP(card, Card_Weapon) ? 4 : 3;
    mvwaddch_ex(win, r, 0, ACS_LTEE, highlight);
    mvwhline(win, r, 1, ACS_HLINE, tx - 2);
    mvwaddch_ex(win, r, tx - 1, ACS_RTEE, highlight);
    int name_color = MAGENTA_on_BLACK;
    if (issubclassP(card, Card_Weapon)) {
      PConstWeapon weapon = issubclassP(card, Card_Weapon)->weapon();
      name_color = GREEN_on_BLACK;
      mvwaddstr(win, r, 2, string_format(" %d ", weapon->state.atq).c_str());
      string hpt = string_format(" %d ", weapon->state.hp);
      mvwaddstr(win, r, tx - 2 - len(hpt), hpt.c_str());
    }
    print_longtext(win, 1, 1, r, tx - 1, name, name_color);
    print_longtext(win, r + 1, 2, ty, tx - 2, desc.c_str());
  }

  // print cost
  if (cost < 0) cost = this->cost;
  string tcost = string_format("(%d)", cost);
  if (cost == this->cost)
    mvwaddstr_ex(win, 0, 0, tcost.c_str(), BLACK_on_CYAN);
  else if (cost < this->cost)
    mvwaddstr_ex(win, 0, 0, tcost.c_str(), WHITE_on_GREEN);
  else
    mvwaddstr_ex(win, 0, 0, tcost.c_str(), WHITE_on_RED);
  UNLOCKP; 
  return win;
}

VizCard::~VizCard() {
  if (small_panel)
     DELETE_PANEL(small_win, small_panel);
}


//# Messages-------- -

static inline int interp(int i, int Max, int start, int end) {
  // func to interpolate, i varies in [0,m-1] 
  assert(0 <= i && i < Max);
  return int(0.5+ start + (end - start)*i / float(Max - 1));
}

bool Msg_PlayerUpdate::draw(Engine* engine) {
  caster->player->viz->update_state(state);
  return true;
}

bool Msg_NewCard::draw(Engine* engine) {
  if (!card->viz)
    card->viz = NEWP(VizCard, card);
  return true;
}

bool Msg_ReceiveCard::draw(Engine* engine) {
  assert(card->viz);  // card must already have been created !
  //if (caster && !caster->viz) return false; // wait caster to appear
  //if(!in(card,player->viz->state.cards))
  player->viz->state.cards.push_back(card);
  const Player* bottom_player = engine->board.viz->get_top_bottom_player(false);
  if (bottom_player == player && engine->board.viz->animated && turn>=0) {
    engine->board.viz->draw(VizBoard::cards, player, false);
    get_win_size(stdscr, NR, NC);
    int ty = VizCard::card_size.y, tx = VizCard::card_size.x;
    int sy = 12, sx = NC - tx;
    pos_t e = engine->board.viz->get_card_pos(card, player);
    for (int y = sy; y <= e.y; y++) {
      int x = int(0.5 + sx + (e.x - sx)*(y - sy) / float(e.y - sy));
      int h = max(0, NR - y);
      card->viz->draw({ KEYINT("highlight", BLACK_on_YELLOW), KEYPOS2("pos", y, x), 
                        KEYBOOL("petit", h<ty), KEYINT("petit_size", h >= ty ? 0 : h) });
      show_panels();
      VizBoard::sleep(0.05 + 0.6*(y == sy));
    }
  }
  engine->board.viz->draw(VizBoard::cards, player);
  return true;
}

bool Msg_BurnCard::draw(Engine* engine) {
  NI;
  return true;
}

bool Msg_ThrowCard::draw(Engine* engine) {
  Player* player = caster->player;
  const Player* top = engine->board.viz->get_top_bottom_player(true);
  if (player == top) {
    int sx = (getmaxx(stdscr) - VizCard::card_size.x) / 2;
    ArgMap kwargs = { KEYBOOL("petit", false), KEYPOS2("pos", 0, sx), KEYINT("highlight", BLACK_on_YELLOW) };
    card->viz->draw(kwargs);
    show_panels();
    VizBoard::sleep(1);
    if (engine->board.viz->animated) {
      for (int i = sx - 1; i >= 0; i -= 2) {
        kwargs["pos"]._pos_t.x = i;
        card->viz->draw(kwargs);
        show_panels();
        VizBoard::sleep(0.05*(i) / sx);
      }
      VizBoard::sleep(0.2);
    }
  }
  card->viz.reset();
  show_panels();
  return true;
}

bool Msg_StartTurn::draw(Engine* engine) {
  Player* player = caster->player;
  PVizButton button = NEWP(VizButton, 10, getmaxx(stdscr) / 2 - 3,
    string_format(" %s's turn! ", player->name.c_str()), 
    VizButton::center, 5, 20);
  button->draw({ KEYINT("highlight", BLACK_on_YELLOW) });
  show_panels();
  VizBoard::sleep(engine->board.viz->animated ? 1 : 0.1);
  button.reset();
  engine->board.viz->draw();
  return true;
}

bool Msg_EndTurn::draw(Engine* engine) {
  return true;
}

typedef unordered_map<Minion*, pos_t> minion_pos_t;
minion_pos_t Anim_MinionsPos(ListPMinion& minions, Player* player) {
  minion_pos_t pos;
  for (int i = 0; i < len(minions); i++)
    pos[minions[i].get()] = VizSlot(Slot(player, i)).get_screen_pos();
  return pos;
}
void Anim_MoveMinions(const minion_pos_t& old_pos, const minion_pos_t& new_pos) {
  const int r = VizMinion::size.x / 2 + 1;
  for (int i = 1; i<r; i++) {
    for (auto& item : new_pos) {
      pos_t n = item.second;
      if (in(item.first, old_pos)) {
        pos_t o = old_pos.at(item.first);
        item.first->viz->draw({ KEYPOS2("pos", interp(i, r, o.y, n.y), interp(i, r, o.x, n.x)) });
      }
    }
    show_panels();
    VizBoard::sleep(0.1);
  }
}

bool Msg_AddMinion::draw(Engine* engine) {
  PMinion new_minion = minion();
  Player* owner = new_minion->player;
  ListPMinion& minions = owner->viz->state.minions;

  if (engine->board.viz->animated && !minions.empty()) {
    auto old_pos = Anim_MinionsPos(minions, owner);
    // insert dummy, but it will be corrected just after by Msg_PlayerUpdate
    minions.insert(minions.begin() + pos.pos, PMinion());
    auto new_pos = Anim_MinionsPos(minions, owner);

    Anim_MoveMinions(old_pos, new_pos);
    minions.erase(minions.begin() + pos.pos); // remove fake stuff
  }

  // do this lastly otherwise weird black rectangle appears
  new_minion->viz = NEWP(VizMinion, new_minion, VizSlot(pos));
  minions.insert(minions.begin() + pos.pos, new_minion);  // add it in case of battlecry
  engine->board.viz->draw(VizBoard::minions, owner);
  return true;
}

bool Msg_AddWeapon::draw(Engine* engine) {
  PWeapon new_weapon = weapon();
  new_weapon->viz = NEWP(VizWeapon, new_weapon);
  CASTP(new_weapon->hero()->viz, VizHero)->w_max_atq = new_weapon->state.max_atq;
  return true;
}

bool Msg_ThingUpdate::draw(Engine* engine) {
  //if( caster->viz)  // if not dead
    CASTP(caster, Thing)->viz_thing()->update_state(state);
  return true;
}

bool Msg_Damage::draw(Engine* engine) {
  return true;
}

bool Msg_Heal::draw(Engine* engine) {
  return true;
}

bool Msg_ZoneBlink::draw(Engine* engine) {
  assert(zone.tags | TGT::minions);
  int what = VizBoard::all;
  Player* which = nullptr;
  if (zone.tags & TGT::friendly) {
    which = caster->player;
    what ^= VizBoard::bkgd_midline;
  }
  if (zone.tags & TGT::enemy) {
    which = engine->board.get_other_player(caster->player);
    what ^= VizBoard::bkgd_midline;
  }
  if (!(zone.tags & TGT::heroes)) {
    what ^= VizBoard::bkgd_hero;
  }

  const float wait = blink_wait;
  for (int i = 0; i < 2* amount; i++) {
    engine->board.viz->draw(what, which, true, str_to_color[color]);
    VizBoard::sleep(wait);
    engine->board.viz->draw(what, which);
    TRACE("last_blink\n");
    VizBoard::sleep(wait);
  }

  return true;
}

//def draw_Msg_SecretPopup(self) :
//secret = self.caster
//weapon.owner.viz.secrets.append(secret)
//self.engine.board.draw('secrets', which = secret.owner)

bool Msg_RemoveMinion::draw(Engine* engine) {
  PMinion dead_minion = minion();
  dead_minion->viz->hide();
  engine->board.viz->deads.push_back(dead_minion);
  Player* pl = dead_minion->player;
  ListPMinion& minions = pl->viz->state.minions;
  int i = index(minions, dead_minion);
  assert(pos.pos == i); // check consistency with true game state
  
  // store current minion screen positions
  if (engine->board.viz->animated) {
    auto old_pos = Anim_MinionsPos(minions, pl);
    remove(minions, dead_minion);  // delete from board.viz
    auto new_pos = Anim_MinionsPos(minions, pl);

    Anim_MoveMinions(old_pos, new_pos);
  }
  return true;
}

bool Msg_RemoveWeapon::draw(Engine* engine) {
  PWeapon dead_weapon = weapon();
  Player* pl = dead_weapon->player; // save before deletion
  assert(pl->viz->state.weapon == dead_weapon);
  dead_weapon->viz.reset(); // do this first (before object is deleted)
  dead_weapon->player->viz->state.weapon.reset(); // dead_weapon is NULL now
  engine->board.viz->draw(VizBoard::weapon, pl);
  return true;
}

bool Msg_Attack::draw(Engine* engine) {
  if (engine->board.viz->animated) {
    LOCKP;
    int ox, oy; getbegyx(caster()->viz->win, oy, ox);
    int oty, otx; getmaxyx(caster()->viz->win, oty, otx);
    top_panel(caster()->viz->panel); // set assailant as top panel
    int ny, nx; getbegyx(target()->viz->win, ny, nx);
    int nty, ntx;  getmaxyx(target()->viz->win, nty, ntx);
    UNLOCKP;
    nx += (ntx - otx) / 2;
    const int m = abs(oy - ny);
    const float t = 0.5f / (m + 2);
    const int M = m - (nty + 1) / 2;
    for (int _i = 1; _i <= 2 * M; _i++) {
      const int i = _i < M ? _i : 2 * M - _i; // increase then decrease
      caster()->viz->draw({ KEYPOS2("pos", interp(i, m, oy, ny), interp(i, m, ox, nx)) });
      show_panels();
      VizBoard::sleep(t);
    }
  }
  return true;
}

bool Msg_HeroPower::draw(Engine* engine) {
  PHero hero = CASTP(caster, Hero);
  PVizHeroPowerButton button = engine->board.viz->hero_power_buttons[hero->player];
  button->draw({ KEYFLOAT("blink", 0.5) });
  return true;
}

static pos_t get_center(VizPanel* viz) {
  int py, px; getbegyx(viz->win,py,px);
  int sy, sx; getmaxyx(viz->win,sy,sx);
  return pos_t(py + sy / 2, px + sx / 2);
}

static bool mouse_in_win(WINDOW* win, int y, int x) {
  get_win_pos(win, wy, wx);
  get_win_size(win, height, width);
  return wx <= x && x < wx + width && wy <= y && y < wy + height;
}

WINDOW* get_window_at(int y, int x) {
  PANEL* cur = my_top_panel();
  while (cur) {
    if (mouse_in_win(panel_window(cur), y, x))
      return panel_window(cur);
    cur = panel_below(cur);
  }
  return stdscr;
}

void anim_magic_burst(Engine* engine, pos_t start, pos_t end, chtype ch, int color, 
                      float tstep = 0.025, int tail=4 ) {
  tail++;
  int dis = int(0.5 + sqrt(pow2(start.y - end.y) + pow2(start.x - end.x)));
  vector<pair<pos_t,chtype> > pos;
  for (int t = 0; t<dis; t++) {
    int y = interp(t, dis, start.y, end.y);
    int x = interp(t, dis, start.x, end.x);
    LOCKP;
    WINDOW* win = get_window_at(y, x);
    int wy, wx; getbegyx(win, wy, wx);
    pos.emplace_back(pos_t(y, x), mvwinch(win, y - wy, x - wx));
    mvwaddch_ex(stdscr, y, x, ch, color);
    UNLOCKP;
    VizBoard::sleep(tstep);
    if (len(pos) >= tail) {
      chtype oldch = pos[0].second;
      LOCKP;
      mvwaddch_ex(stdscr, pos[0].first.y, pos[0].first.x, oldch);
      UNLOCKP;
      pos.erase(pos.begin());
    }
    show_panels();
  }
  while (!pos.empty()) {
    VizBoard::sleep(tstep);
    chtype oldch = pos[0].second;
    LOCKP;
    mvwaddch_ex(stdscr, pos[0].first.y, pos[0].first.x, oldch);
    show_panels(false);
    UNLOCKP;
    pos.erase(pos.begin());
  }
  engine->board.viz->draw();
}

bool Msg_Arrow::draw(Engine* engine) {
  if (engine->board.viz->animated) {
    if (!caster->viz) return false;
    anim_magic_burst(engine, get_center(caster->viz.get()), get_center(target->viz.get()), ch, str_to_color.at(color));
  }
  return true;
}
bool Msg_Arrow_HeroPower::draw(Engine* engine) {
  if (engine->board.viz->animated) {
    assert(issubclassP(caster, Hero));
    PVizHeroPowerButton button = engine->board.viz->hero_power_buttons[caster->player];
    anim_magic_burst(engine, get_center(button.get()), get_center(target->viz.get()), ch, str_to_color.at(color));
  }
  return true;
}


//### Board --------

VizBoard::VizBoard(Board* board, bool switch_heroes, bool animated) :
  board(board), switch_heroes(switch_heroes), animated(animated),
  end_turn(NEWP(VizButton,11, getmaxx(stdscr), "End turn", VizButton::right)) {
  VizPanel::set_engine(board->engine);
  for (int i = 0; i < 2; i++) {
    Player* pl = board->engine->board.players[i];
    pl->viz = NEWP(VizPlayer, board->engine->board.players[i]);
    pl->state.hero->viz = NEWP(VizHero, pl->state.hero, get_hero_pos(pl));
    hero_power_buttons[pl] = pl->state.hero->viz_hero()->create_hero_power_button();
  }
}

double VizBoard::accel = 1.0;

void VizBoard::sleep(double seconds) {
#ifdef _WIN32
  Sleep(int(1000 * seconds/accel));
#else
  NI;
#endif
}

double VizBoard::now() {
#ifdef _WIN32
  return accel*GetTickCount() / 1000.0;
#else
  NI;
#endif
}

Player* VizBoard::get_top_bottom_player(bool top) {
  Player* player = board->engine->board.get_current_player();
  Player* adv = board->engine->board.get_enemy_player();
  if (switch_heroes == false && board->engine->board.state.turn % 2)
    return top ? player : adv; // prevent top / down switching
  else
    return top ? adv : player;
}

pos_t VizBoard::get_minion_pos(const Minion* minion) {
  const int i = indexP(minion->player->viz->state.minions, minion);
  assert(i >= 0);
  Slot slot(minion->player, i, -1);
  return VizSlot(slot).get_screen_pos();
}

pos_t VizBoard::get_card_pos(PCard card, Player* player) {
  get_win_size(stdscr, NR, NC);
  double nc = len(player->viz->state.cards) + 1e-8;
  int ty = VizCard::card_size.y, tx = VizCard::card_size.x;
  int actual_NC = min(NC, int(tx*nc));
  int startx = int((NC - actual_NC) / 2);
  int i = index(player->viz->state.cards, card);
  return{ 24, startx + int((actual_NC - tx)*i / (nc - 1)) };
}

pos_t VizBoard::get_hero_pos(Player* player) {
  get_win_size(stdscr, NR, NC);
  if (player == get_top_bottom_player(true))
    return{ 1, (NC - 18) / 2 };
  else
    return{ 20, (NC - 18) / 2 };
}

void VizBoard::draw(int what, Player* which_, bool last_card, chtype bkgd_colr) {
  get_win_size(stdscr, NR, NC);

  // clear screen
  Player* adv = get_top_bottom_player(true);
  Player* player = get_top_bottom_player(false);
  vector<Player*> which;
  if (which_)
    which = { which_ }; // single player
  else
    which = { adv, player };  // both

  if (bkgd & what) {
    // background
    //erase();  // useless, just make boring screen blinks
    LOCKP;
    if(bkgd_colr) attron(bkgd_colr);
    if (in(adv, which)) {
      if (what & bkgd_hero) {
        mvhline(1, 0, ' ', NC - 3);
        mvhline(2, 0, ACS_CKBOARD, NC - 3);
        mvhline(3, 0, ACS_CKBOARD, NC - 3);
        mvhline(4, 0, ' ', NC - 3);
      }
      clear_rect(stdscr, 5, 0, 12 - 5, getmaxx(stdscr) - 3);
    }
    if (what & bkgd_midline) 
      mvhline(12, 0, '-', NC);
    if (in(player, which)) {
      clear_rect(stdscr, 13, 0, 20 - 13, getmaxx(stdscr) - 3);
      if (what & bkgd_hero) {
        mvhline(20, 0, ' ', NC - 3);
        mvhline(21, 0, ACS_CKBOARD, NC - 3);
        mvhline(22, 0, ACS_CKBOARD, NC - 3);
        mvhline(23, 0, ' ', NC - 3);
      }
    }
    if (bkgd_colr) attroff(bkgd_colr);
    UNLOCKP;
    end_turn->draw({});
    // draw hero power
    for (auto pl : which)
      hero_power_buttons.at(pl)->draw({ KEYINT("y", get_hero_pos(pl).y) });
  }

  // draw decks on the right side
  if (decks & what) {
    LOCKP;
    for (int i = 0; i < 3; i++)
      mvvline(2, NC - 1 - i, ACS_CKBOARD, 21);
    for (int i = 5; i <= 15; i += 10) {
      mvaddch(i, NC - 2, ACS_HLINE);
      addwch_ex(9558);
      string text = string_format(" %2d", len(i < 12 ? *adv->deck : *player->deck));
      for (int j = 0; j < len(text); j++) {
        char ch = text[j];
        mvaddch(i + 1 + j, NC - 2, ch);
        addwch_ex(9553);
        mvaddch(i + 4, NC - 2, ACS_HLINE);
        addwch_ex(9564);
      }
    }
    UNLOCKP;
  }

  //draw heroes
  if ((hero|weapon) & what) {
    for (auto pl : which) {
      ArgMap args = { KEYINT("y", get_hero_pos(pl).y) };
      pl->state.hero->viz->draw(args);
      if (pl->viz->state.weapon)
        pl->viz->state.weapon->viz->draw(args);
    }
  }

  // draw cards
  if (cards & what) {
    if (in(adv, which)) {
      ListPCard cards = adv->viz->state.cards;
      if (!last_card) cards.pop_back();
      for (auto card : cards)
        card->viz->draw({ KEYBOOL("hide", true) }); // hide adversary cards
      LOCKP;
      clear_rect(stdscr, 0, 0, 1, getmaxx(stdscr));
      print_middle(stdscr, 0, 0, NC, string_format(" Adversary has %d cards. ", len(adv->viz->state.cards)));
      UNLOCKP;
    }
    if (in(player, which)) {
      LOCKP;
      clear_rect(stdscr, 24, 0, getmaxy(stdscr) - 24, getmaxx(stdscr));
      UNLOCKP;
      ListPCard cards = player->viz->state.cards;
      if (!last_card) cards.pop_back();
      for (auto card : cards)
        card->viz->draw({ KEYPOS1("pos", get_card_pos(card, player)), KEYBOOL("petit", true), KEYINT("petit_size", NR - 24) });
    }
  }

  // draw mana
  if (mana & what) {
    for (int j = 0; j < 2; j++) {
      Player* who = j ? player : adv;
      int i = j ? 22 : 2;
      if (!in(who, which)) continue;
      Player* p = i < 12 ? adv : player;
      string text = string_format("%2d/%d ", p->viz->state.mana, p->viz->state.max_mana);
      LOCKP;
      mvwaddstr_ex(stdscr, i, NC - 11 - len(text), text.c_str(), BLACK_on_CYAN);
      addwch_ex(9830, CYAN_on_BLACK, p->state.mana);
      addwch_ex(9826, CYAN_on_BLACK, p->state.max_mana - p->state.mana);
      UNLOCKP;
    }
  }

  // draw minions
  if (minions & what) {
    for (Player* pl : which) {
      for (auto m : pl->viz->state.minions)
        m->viz->draw({ KEYPOS1("pos", get_minion_pos(m.get())) });
    }
  }

  show_panels();
}


// ----- Human player interface -------

ListPCard HumanPlayer::mulligan(ListPCard & cards) const {
  engine->wait_for_display();
  engine->board.viz->draw();

  int NR, NC; getmaxyx(stdscr,NR,NC);
  int nc = len(cards);
  VizButton end_button(25, (NC - 6) / 2, "  OK  ");
  
  struct ShowElem {
    VizPanel* viz;
    ArgMap args;
  };
  vector<ShowElem> showlist = { { &end_button, { KEYINT("highlight", BLACK_on_BLUE) } } };
  
  for (int i = 0; i < len(cards); i++) {
    VizCard* card = cards[i]->viz.get();
    showlist.push_back({ card, { KEYPOS2("pos", 6, int((NC - 6)*(i + 0.5) / nc - 7)),
                                 KEYINT("petit", false), 
                                 KEYINT("highlight", BLACK_on_GREEN) } });
  }
  
  ListPCard discarded;
  while (true) {
    // draw elements in show list
    for (auto sl : showlist) 
      sl.viz->draw(sl.args);
    show_panels();

    // wait for mouse click
    int ch = my_getch();

    if (ch == KEY_MOUSE) {
      MEVENT event;
      unsigned long mouse_state = getmouse_ex(&event);
      if (mouse_state == ERR) continue;

      VizPanel* which = nullptr;
      ArgMap* kwargs = nullptr;
      //forlen(sl, showlist) 
      for (auto& sl : showlist) {
        if (mouse_in_win(sl.viz->win, event.y, event.x)) {
          which = sl.viz;
          kwargs = &sl.args;
          break;
        }
      }
      VizCard* card = dynamic_cast<VizCard*>(which);
      if (event.bstate & BUTTON1_PRESSED) {
        if (!which) continue;
        if (which == &end_button)
          (*kwargs)["bkgd"]._int = BLACK_on_BLUE;
        else if (in(card->card, discarded)) {
          (*kwargs)["bkgd"]._int = 0;
          remove(discarded, card->card);
        }
        else {
          (*kwargs)["bkgd"]._int = BLACK_on_RED;
          discarded.push_back(card->card);
        }
      }
      else if (event.bstate & BUTTON1_RELEASED) {
        if (which == &end_button)
          break;
        else // reset end button
          showlist[0].args["bkgd"]._int = 0;
      }
    }
    else if (ch == ' ' || ch == '\n')
      break;
  }
  
  //clean up
  LOCKP;
  for (auto& card : cards ) 
    hide_panel(card->viz->panel);
  UNLOCKP;
  end_button.~VizButton();
  show_panels();
  
  // remove cards
  for (auto& card : discarded)
    remove(cards, card);
  return discarded;
}

const Action* HumanPlayer::choose_actions(ListAction actions, Instance*& choice, Slot& slot) const {
  // split actions
  struct ShowElem {
    const Action* action;
    PVizPanel viz;
    ArgMap kwargs; 
  };
  const Action* end_turn_action = nullptr;
  typedef vector<ShowElem> ShowList;
  ShowList showlist; // [(action, object, draw_kwargs)]
  ListPCard remaining_cards = state.cards; // copy
  for (auto a : actions) {
    if (issubclass(a, const Act_HeroPower)) {
      showlist.push_back({ a, engine->board.viz->hero_power_buttons[this], {} });
    }
    else if (issubclass(a, const Act_PlayCard)) {
      const Act_PlayCard* act = CAST(a, const Act_PlayCard);
      showlist.push_back({ a, act->card->viz, { KEYBOOL("petit", true), KEYINT("cost", a->get_cost()) } });
      if (findP(remaining_cards, act->card))
        remove(remaining_cards, findP(remaining_cards, act->card));
    }
    else if (issubclass(a, const Act_Attack)) {
      const Act_Attack* act = CAST(a, const Act_Attack);
      showlist.push_back({ a, act->creature->viz, {} });
    }/*
    else if (issubclass(a, Act_WeaponAttack)){
      showlist.push_back({a, this->state.hero->viz, {}});
    }*/
    else if (issubclass(a, const Act_EndTurn)) {
      end_turn_action = a;
      showlist.push_back({ a, engine->board.viz->end_turn, {} });
    }
    else
      assert(!"unrecognized action");
  }
  // we can also inspect non - playable cards
  for (auto card : remaining_cards)
    showlist.push_back({ nullptr, card->viz, { KEYBOOL("petit", true) } });
  
  auto erase_elems = [](ShowList& showlist) {
    for (auto& elem : showlist)
      elem.viz->draw({ KEYBOOL("petit", true) });// erase everything
  };

  engine->board.viz->draw();

  ShowList init_showlist = showlist;
  slot.pos = -1;  // init as none
  const Action* fake_act_slot = (Action*)1;
  const Action* fake_act_target = (Action*)2;
  VizPanel* last_sel = nullptr;
  VizPanel* active = nullptr;
  const Action* action = nullptr; // current action
  while (true) {
    struct ActElem {
      const Action* action;
      ArgMap* kwargs;
    };
    typedef unordered_map<VizPanel*, ActElem> PanelAction;
    PanelAction mapping;  // panel -> {obj->action, draw_kwargs}
    for (auto& sl : showlist) {
      assert(sl.viz);
      int highlight = sl.action ? BLACK_on_GREEN : 0;
      ArgMap kwargs = sl.kwargs;  // copy
      kwargs["highlight"]._int = highlight;
      sl.viz->draw(kwargs);
      mapping[sl.viz.get()] = { sl.action, &sl.kwargs };
    }
    if (active)
      active->draw({ KEYINT("bkgd", BLACK_on_WHITE) });
    show_panels();
    TRACE("showlist\n");

    int ch = my_getch();

    if (ch == KEY_MOUSE) {
      MEVENT event;
      unsigned long mouse_state = getmouse_ex(&event);
      if (mouse_state == ERR) continue;

      LOCKP;
      VizPanel* sel = nullptr;
      PANEL* cur = my_top_panel();
      while (cur) {
        VizPanel* obj = (VizPanel*)panel_userptr(cur);
        if (issubclass(obj, VizCard) || in(obj, mapping)) {
          if (mouse_in_win(panel_window(cur), event.y, event.x)) {
            sel = obj;
            break;
          }
        }
        cur = panel_below(cur);
      }
      UNLOCKP;

      // reset everybody
      for (auto elem : mapping) {
        (*elem.second.kwargs)["bkgd"]._int = 0;
        (*elem.second.kwargs)["petit"]._bool = true;
      }

      if (event.bstate & BUTTON1_PRESSED) {
        // set current all green
        if (in(sel, mapping)) {
          ActElem elem = mapping[sel];
          (*elem.kwargs)["bkgd"]._int = elem.action ? BLACK_on_GREEN : 0;
          (*elem.kwargs)["petit"]._bool = false;
          last_sel = sel;
        }
      }
      else if (event.bstate & BUTTON1_RELEASED) {
        if (sel != last_sel) continue;
        if (!in(sel, mapping)) continue;
        const Action* act = mapping[sel].action;
        if (!act) continue;
        ArgMap& kwargs = *mapping[sel].kwargs;

        // acknowledged choice
        if (act == fake_act_slot) 
          slot = CAST(sel, VizSlot)->slot;
        else if (act == fake_act_target) 
          choice = CONSTCAST(CAST(sel, VizThing)->obj.get(), Instance);
        else {
          action = act;
          active = sel;
          choice = nullptr;  // reset choices
          slot.pos = -1;
        }

        // define next choice
        erase_elems(showlist);
        if (action->need_slot && slot.pos < 0) {
          showlist.clear();
          ListSlot slots = engine->board.get_free_slots(const_cast<HumanPlayer*>(this));
          assert(!slots.empty());
          for (auto& sl : slots)
            showlist.push_back({ fake_act_slot, NEWP(VizSlot, sl), {} });
        } 
        else if (action->need_target() && !choice) {
          showlist.clear();
          ListPInstance& choices = action->target.resolve(const_cast<HumanPlayer*>(this));
          if (choices.empty())  return action;  // nothing to do then
          for (auto& obj : choices)
            showlist.push_back({ fake_act_target, obj->viz, {} });
        }
        else
          return action; 
      }
      else {
        erase_elems(showlist);
        showlist = init_showlist;
        active = nullptr;
      }
    }
    elif (ch == '\n') {  // enter
      if (!active)
        return end_turn_action;
    }
    elif (ch == 27) { // escape
      erase_elems(showlist);
      showlist = init_showlist;
      active = nullptr;
    }
    elif(ch == 1) {
      TRACE("getch(1)\n");
    }
    elif (ch == 'q') {  // quit
      endwin();
      //printf(engine->log.c_str());
      exit(-1);
    }
  }
}

//# overloaded HS engine
CursesEngine::CursesEngine() :
  Engine() {
  logfile = fopen("log.txt", "w");
}

void CursesEngine::wait_for_display() {
  TRACE("wait_for_display()\n");
  while (display.size()) {
    PMessage msg = display.front();
    display.pop_front();

    bool ok = msg->draw(this);
    if (ok) {
      string line = string_format("%s $ %s\n", msg->cls_name(), msg->tostr().c_str());
      fwrite(line.c_str(), line.size(), 1, logfile);
      TRACE(line.c_str());
      fflush(logfile);
      log += line;
    }
    else {
      assert(len(display) >= 1);
      auto it = display.begin();
      display.insert(++it, msg);
    }
  }

  // delete dead stuff
  ListPInstance& deads = this->board.viz->deads;
  while (!deads.empty()) {
    deads.back()->viz.reset();  // delete its viz
    deads.pop_back();
  }
}

//void CursesEngine::dbg_add_minion(Player* player, PCardMinion card) {
//  NI;
//  //card = deepcopy(card)
//  //card.owner = player
//  //engine.send_message(Msg_AddMinion(player, Minion(card), pos = engine.board.get_free_slots(player)[0]))
//}