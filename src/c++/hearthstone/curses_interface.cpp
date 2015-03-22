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

void init_screen() {
  extend_console(30, 80);
  initscr();

  int NR, NC;
  getmaxyx(stdscr, NR, NC);

  if (NR < 30) {
    endwin();
    fprintf(stderr, "error: screen not high enough (min 30 lines)");
    exit(-1);
  }
  else if (NC < 80) {
    endwin();
    fprintf(stderr, "error: screen not wide enough (min 80 columns)");
    exit(-1);
  }

  start_color();
  cbreak();
  curs_set(0);
  noecho();
  keypad(stdscr, true);
  mouseinterval(0);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);

  // init color pairs
#define create_color_pair(num, col_fg,col_bg)  init_pair(num, COLOR_##col_fg, COLOR_##col_bg);
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

void congratulate_winner(Player* winner, int turn) {
  int NC = getmaxx(stdscr);
  VizButton button(10, NC / 2 - 3, string_format("  %s wins after %d turns!  ", winner->name.c_str(), (turn + 1) / 2), VizButton::center, 5);
  button.draw({ KEYINT("highlight", BLACK_on_YELLOW) });
  show_panels();
  getch();
}

void end_screen() {
  endwin();
}

static void my_sleep(double seconds) {
#ifdef _WIN32
  Sleep(int(1000*seconds));
#else
  NI;
#endif
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

static int mvwchgat_ex(WINDOW* scr_id, int y, int x, int num, attr_t attr, int color) {
#ifdef _WIN32
  return mvwchgat(scr_id, y, x, num, attr, color, nullptr);
#else
  return scr_id.chgat(y, x, num, attr | color_pair(color));
#endif
}

static int getmouse_ex(MEVENT* event) {
#ifdef _WIN32
  return nc_getmouse(event);
#else
#endif
}

static int print_middle(WINDOW* win, int y, int x, int width, string text, int attr = 0) {
  if (!win) win = stdscr;
  x = max(x, x + (width - len(text) + 1) / 2);
  if (len(text) > width)  text = text.substr(0, width);

  SETWATTR(win, attr);
  int ret = mvwaddstr(win, y, x, text.c_str());
  UNSETWATTR(win, attr);
  return ret;
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

static void delete_panel(PANEL* panel) {
  WINDOW* win = panel_window(panel);
  del_panel(panel);
  delwin(win);
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
  getch();
  clear();
  i = 0;
  refresh();
  getch();
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
      getch();
      clear();
    }
  }
  refresh();
  getch();
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

VizPanel::~VizPanel() {
  if (win && panel) {
    delete_panel(panel);
    win = nullptr;
    panel = nullptr;
  }
}

//### General thing (minion, weapon, hero...)------

VizInstance::VizInstance(const Instance* obj) :
  VizPanel(), obj(obj), card(obj->card) {}

PVizThing Thing::viz_thing() { return CASTP(viz, VizThing); }

PConstCardThing VizThing::card_thing() const { 
  return CASTP(card, const Card_Thing); 
}

VizThing::VizThing(const Thing* obj, int ty, int tx, int y, int x) :
  VizInstance(obj), state(obj->state) {
  wait = 0;
  // create panel
  win = newwin(ty, tx, y, x);
  assert(win);
  panel = new_panel(win);
  set_panel_userptr(panel, this);
}

bool VizThing::check() {
  const Thing* obj = thing();
  assert(state.hp == obj->state.hp);
  assert(state.max_hp == obj->state.max_hp);
  assert(state.atq == obj->state.atq);
  assert(state.max_atq == obj->state.max_atq);
  assert(state.static_effects == obj->state.static_effects);
  return true;
}

VizThing::~VizThing() {
  double t = 0;
  while (wait && t < 5) {
    my_sleep(0.1);
    t += 0.1;
    wait = 0;
  }
  assert(t < 5);
}

int VizThing::buff_color(int val, bool highlight, bool standout) const {
  int res;
  if (val>0)
    res = highlight ? WHITE_on_GREEN : GREEN_on_BLACK;
  else if (val == 0)
    res = highlight ? WHITE_on_BLACK : 0;
  else if (val < 0)
    res = highlight ? WHITE_on_RED : RED_on_BLACK;
  return res | (standout ? A_STANDOUT : 0);
}

int VizThing::buff_color(const int* val, bool highlight, bool standout) const {
  int max_val, max_card_val;
  if (val == &state.hp) {
    max_val = state.max_hp;
    max_card_val = card_thing()->thing()->state.max_hp;
  }
  else if (val == &state.atq) {
    max_val = state.max_atq;
    max_card_val = card_thing()->thing()->state.max_atq;
  }
  else if (val == &state.armor) {
    max_val = max_card_val = 999999;
  }
  else assert(!"error: unrecognized val");
  int res;
  if (*val < max_val)
    res = highlight ? WHITE_on_RED : RED_on_BLACK;
  else if (*val < max_card_val)
    res = highlight ? WHITE_on_GREEN : GREEN_on_BLACK;
  else
    res = highlight ? WHITE_on_BLACK : 0;
  return res | (standout ? A_STANDOUT : 0);
}

void VizThing::update_state(const Thing::State& from, bool show_diff) {
  const bool anim = show_diff && engine->board.viz->animated;
  // update attribute
  int* can_change[] = { &state.hp, &state.atq, &state.armor };
  for (int* val : can_change) {
    int oldval = *val;
    int newval = *(((int*)&from) + (val - (int*)&state));
    int diff = newval - oldval;
    // animation if change
    if (anim && diff) 
        temp_panel(this, string_format("%+d", diff), buff_color(diff, true, false), 1.5);
  }
  // copy everything
  state = from;
  draw({});
}

WINDOW* VizThing::draw(const ArgMap& args) {
  DEFARG(pos_t, pos, nullpos);
  DEFARG(int, y, 0);
  DEFARG(int, bkgd, 0);
  DEFARG(int, highlight, 0);
  if (!win) return win;
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
  mvwaddstr_ex(win, ty - 1, tx - 1 - len(thp), thp.c_str(), highlight | buff_color(&state.hp));
  return win;
}


//### Hero -----------

PVizHero Hero::viz_hero() { return issubclassP(viz, VizHero); }

PConstCardHero VizHero::card_thing() const { 
  return CASTP(card, const Card_Hero);
}

VizHero::VizHero(const Hero* hero, pos_t pos) :
  VizThing(hero, 4, 13, pos.y, pos.x) {
}

WINDOW* VizHero::draw(const ArgMap& args) {
  WINDOW* win = VizThing::draw(args);
  if (!win) return win;
  DEFARG(int, highlight, 0);
  get_win_size(win, ty, tx);
  print_middle(win, 1, 1, tx - 2, hero()->player->name);
  string hero_name = split(hero()->card->name)[0];
  print_middle(win, 2, 1, tx - 2, "(" + hero_name + ")", BLUE_on_BLACK);
  if (state.armor) {
    string tar = string_format("[%d]", state.armor);
    mvwaddstr(win, ty - 2, tx - 1 - len(tar), tar.c_str());
  }
  VizPlayer* pl = hero()->player->viz.get();
  int attack = (pl->state.weapon ? pl->state.weapon->state.atq : 0) + state.atq;
  if (attack) {
    int hh = pl->state.weapon ? pl->state.weapon->viz_weapon()->buff_color(&pl->state.weapon->viz_weapon()->state.atq) : 
                                buff_color(&state.atq);
    mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", attack).c_str(), highlight | hh);
  }
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
  return NEWP(VizHeroPowerButton, y, x + 24, up, down, card->ability->cost);
}

//### Minion -----------

PVizMinion Minion::viz_minion() { return CASTP(viz, VizMinion); }

VizMinion::VizMinion(const Minion* minion, VizSlot pos) :
  VizThing(minion, 5, 11, pos.get_screen_pos().y, pos.get_screen_pos().x-6) {}

WINDOW* VizMinion::draw(const ArgMap& args) {
  DEFARG(int, x, minion()->engine->board.viz->get_minion_pos(minion()).x);
  WINDOW* win = VizThing::draw(args);
  if (!win) return win;
  DEFARG(int, highlight, 0);
  get_win_size(win, ty, tx);
  string name = minion()->card->get_name_fr();
  print_longtext(win, 1, 1, ty - 1, tx - 1, name, YELLOW_on_BLACK);
  mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", state.atq).c_str(), highlight | buff_color(&state.atq));
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
  return win;
}

PVizWeapon Weapon::viz_weapon() { return issubclassP(viz, VizWeapon); }

VizWeapon::VizWeapon(const Weapon* weapon) :
VizThing(weapon, 4, 11, getbegy(weapon->hero()->viz->win), 
                        getbegx(weapon->hero()->viz->win) - 18) {}

WINDOW* VizWeapon::draw(const ArgMap & args) {
  WINDOW* win = VizThing::draw(args);
  if (!win) return win;
  DEFARG(int, highlight, 0);
  get_win_size(win, ty, tx);
  string name = weapon()->card->get_name_fr();
  print_longtext(win, 1, 1, ty - 1, tx - 1, name, GREEN_on_BLACK);
  NI;//  int ref_atq = min(weapon()->card_weapon()->weapon->state.atq, max_atq);
  mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", state.atq).c_str(), highlight | buff_color(&state.atq));
  return win;
}

void VizWeapon::update_state(const Thing::State& from) {
  VizThing::update_state(from, false);
}

//### Player -----------

VizPlayer::VizPlayer(Player* player) :
  player(player), state(player->state) {}

bool VizPlayer::check() const {
  const Player::State& pl_state = player->state;
  assert(state.mana == pl_state.mana);
  assert(state.max_mana == pl_state.max_mana);
  assert(state.cards == pl_state.cards);
  assert(state.hero == pl_state.hero);
  if (pl_state.weapon || state.weapon) {
    assert(pl_state.weapon == state.weapon);
    state.weapon->viz_weapon()->check();
  }
  assert(state.secrets == pl_state.secrets);
  assert(state.minions == pl_state.minions);
  for (auto& m: pl_state.minions)
    m->viz_minion()->check();
  return true;
}

void VizPlayer::update_state(const Player::State& from) {
  state = from; // copy
  player->engine->board.viz->draw(0xFF,player);
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
  return win;
}

VizHeroPowerButton::VizHeroPowerButton(int y, int x, string text, string subtext, int cost ) :
  VizButton(y, x, text, VizButton::center, 4, 9), subtext(subtext), cost(cost), used(false) {}

WINDOW* VizHeroPowerButton::draw(const ArgMap& args) {
  DEFARG(bool, blink, false);
  int coltext = YELLOW_on_BLACK;
  ArgMap kwargs = args;
  if (used) kwargs["bkgd"]._int = BLACK_on_YELLOW;
  kwargs["ytext"]._int = 1;
  kwargs["coltext"]._int = coltext;
  VizButton::draw(kwargs);
  if (blink) {
    for (int i = 0; i < 10 * blink; ++i) {
      kwargs["bkgd"]._int = (i % 2) ? YELLOW_on_BLACK : BLACK_on_YELLOW;
      VizButton::draw(kwargs);
      show_panels();
      my_sleep(0.1);
    }
  }
  get_win_size(win, ty, tx);
  mvwaddstr_ex(win, 0, tx / 2 - 1, string_format("(%d)", cost).c_str(), CYAN_on_BLACK);
  print_longtext(win, 2, 1, ty - 1, tx - 1, subtext, coltext);
  return win;
}

void wait_delete(float duration, PVizButton button, VizThing* viz) {
  double t = 0;
  while (t < duration) {
    lock_panels();
    touchwin(button->win);
    top_panel(button->panel); // # remains at top
    release_panels();
    show_panels();
    my_sleep(0.1);
    t += 0.1;
  }
  lock_panels();
  button.reset();
  release_panels();
  show_panels();
  viz->wait -= 1;
}
void temp_panel(VizThing* viz, string text, int color, float duration) {
  viz->wait += 1;
  get_win_pos(viz->win, y, x);
  get_win_size(viz->win, ty, tx);
  PVizButton button = NEWP(VizButton, y + ty / 2 - 1, x + tx / 2, text);
  button->draw({ KEYBOOL("box", false), KEYINT("bkgd", color) });
  wait_delete(duration, button, viz);
  //Thread(target = wait_delete, args = (duration, button, viz)).start();
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
int VizSlot::get_screen_space() const {
  int n = len(slot.player->viz->state.minions);
  return min(3, 7 - n); // spacement between minions
}

WINDOW* VizSlot::draw(const ArgMap& args) {
  DEFARG(int, highlight, 0);
  DEFARG(int, bkgd, 0);
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
  }
  else
    VizPanel::~VizPanel();
  return win;
}


//### Card -----------

const pos_t VizCard::card_size = { 14, 15 };

VizCard::VizCard(PCard card) :
  VizPanel(), card(card), small_win(nullptr), small_panel(nullptr) {
  cost = card->cost;
}

WINDOW* VizCard::draw(const ArgMap& args) {
  DEFARG(bool, hide, false);
  if (hide) {
    if (panel) hide_panel(panel);
    if (small_panel) hide_panel(small_panel);
    return win;
  }
  DEFARG(pos_t, pos, nullpos);
  DEFARG(int, highlight, 0);
  DEFARG(int, cost, -1);
  DEFARG(bool, petit, true);  // show small_panel
  DEFARG(int, petit_size, 0); // height of small_panel
  DEFARG(int, bkgd, 0);

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
    else if (panel_hidden(this->small_panel))
      top_panel(this->small_panel);
    win = this->small_win;
    panel = this->small_panel;

    get_win_size(win, ty, tx);
    if (petit_size>0 && petit_size != ty) {
      delete_panel(panel);
      this->small_win = nullptr;
      this->small_panel = nullptr;
      ArgMap kwargs = args;
      kwargs["petit"]._bool = true;
      kwargs["petit_size"]._int = petit_size;
      kwargs["pos"]._pos_t = pos;
      return draw(kwargs);
    }
    petit_size = getmaxy(win);
  }

  get_win_size(win, ty, tx);
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
    NI; /*
    int r = issubclassP(card, Card_Weapon) ? 4 : 3;
    mvwaddch_ex(win, r, 0, ACS_LTEE, highlight);
    mvwhline(win, r, 1, ACS_HLINE, tx - 2);
    mvwaddch_ex(win, r, tx - 1, ACS_RTEE, highlight);
    if (issubclassP(type(self), Card_Weapon)) {
      PWeapon weapon = issubclassP(type(self), Card_Weapon)->weapon;
      int name_color = GREEN_on_BLACK;
      mvwaddstr(win, r, 2, string_format(" %d ", weapon->state.atq));
      string hpt = string_format(" %d ", weapon->state.hp);
      mvwaddstr(win, r, tx - 2 - len(hpt), hpt.c_str());
    }
    else {
      int name_color = MAGENTA_on_BLACK;
      print_longtext(win, 1, 1, r, tx - 1, name, name_color);
      print_longtext(win, r + 1, 2, ty, tx - 2, desc.c_str());
    }*/
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
  return win;
}

VizCard::~VizCard() {
  if (small_panel) {
    delete_panel(small_panel);
    small_win = nullptr;
    small_panel = nullptr;
  }
}


//# Messages-------- -

static inline int interp(int i, int Max, int start, int end) {
  // func to interpolate, i varies in [0,m-1] 
  assert(0 <= i && i < Max);
  return int(0.5+ start + (end - start)*i / float(Max - 1));
}

void Msg_PlayerUpdate::draw(Engine* engine) {
  caster->player->viz->update_state(state);
}

void Msg_NewCard::draw(Engine* engine) {
  if (!card->viz)
    card->viz = NEWP(VizCard, card);
}

void Msg_ReceiveCard::draw(Engine* engine) {
  assert(card->viz);  // card must already have been created !
  player->viz->state.cards.push_back(card);
  const Player* bottom_player = engine->board.viz->get_top_bottom_player(false);
  if (engine->board.viz->animated && bottom_player == player) {
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
      my_sleep(0.05 + 0.6*(y == sy));
    }
  }
  engine->board.viz->draw(VizBoard::cards, player);
}

void Msg_BurnCard::draw(Engine* engine) {
  NI;
}

void Msg_ThrowCard::draw(Engine* engine) {
  Player* player = caster->player;
  const Player* top = engine->board.viz->get_top_bottom_player(true);
  if (player == top) {
    int sx = (getmaxx(stdscr) - VizCard::card_size.x) / 2;
    ArgMap kwargs = { KEYBOOL("small", false), KEYPOS2("pos", 0, sx), KEYINT("highlight", BLACK_on_YELLOW) };
    card->viz->draw(kwargs);
    show_panels();
    my_sleep(1);
    if (engine->board.viz->animated) {
      for (int i = sx - 1; i >= 0; i -= 2) {
        kwargs["pos"]._pos_t.x = i;
        card->viz->draw(kwargs);
        show_panels();
        my_sleep(0.05*(i) / sx);
      }
      my_sleep(0.2);
    }
  }
  card->viz.reset();
  show_panels();
}

void Msg_StartTurn::draw(Engine* engine) {
  Player* player = caster->player;
  player->viz->check();
  PVizButton button = NEWP(VizButton, 10, getmaxx(stdscr) / 2 - 3,
    string_format(" %s's turn! ", player->name.c_str()), 
    VizButton::center, 5, 20);
  button->draw({ KEYINT("highlight", BLACK_on_YELLOW) });
  show_panels();
  my_sleep(engine->board.viz->animated ? 1 : 0.1);
  button.reset();
  engine->board.viz->hero_power_buttons[player]->used = false;
  engine->board.viz->draw();
}

void Msg_EndTurn::draw(Engine* engine) {
}

void Msg_AddMinion::draw(Engine* engine) {
  minion()->viz = NEWP(VizMinion, minion(), VizSlot(pos));
}

void Msg_ThingUpdate::draw(Engine* engine) {
  CAST(caster, Thing)->viz_thing()->update_state(state);
}

void Msg_Damage::draw(Engine* engine) {
}

void Msg_Heal::draw(Engine* engine) {
}

//void Msg_MinionPopup::draw(Engine* engine) {
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
  //my_sleep(0.1)
  //show_panel(new_minion.viz.panel)
  //self.engine.board.draw('minions', which = owner)
//}

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
//my_sleep(0.1)
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
//if not self.caster.viz.update_state(self) :
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
//my_sleep(t)
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
//my_sleep(tstep)
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

VizBoard::VizBoard(Board* board, bool switch_heroes, bool animated) :
  board(board), switch_heroes(switch_heroes), animated(animated),
  end_turn(NEWP(VizButton,11, getmaxx(stdscr), "End turn", VizButton::right)) {
  VizPanel::set_engine(board->engine);
  for (int i = 0; i < 2; i++) {
    Player* pl = board->engine->players[i];
    pl->viz = NEWP(VizPlayer, board->engine->players[i]);
    pl->state.hero->viz = NEWP(VizHero, pl->state.hero.get(), get_hero_pos(pl));
    hero_power_buttons[pl] = pl->state.hero->viz_hero()->create_hero_power_button();
  }
}

Player* VizBoard::get_top_bottom_player(bool top) {
  Player* player = board->engine->get_current_player();
  Player* adv = board->engine->get_enemy_player();
  if (switch_heroes == false && board->engine->turn % 2)
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

void VizBoard::draw(int what, Player* which_, bool last_card) {
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
    erase();
    mvhline(2, 0, ACS_CKBOARD, NC);
    mvhline(3, 0, ACS_CKBOARD, NC);
    mvhline(12, 0, '-', NC);
    mvhline(21, 0, ACS_CKBOARD, NC);
    mvhline(22, 0, ACS_CKBOARD, NC);
    end_turn->draw({});
    // draw hero power
    for (auto pl : which)
      hero_power_buttons.at(pl)->draw({ KEYINT("y", get_hero_pos(pl).y) });
  }

  // draw decks on the right side
  if (decks & what) {
    for (int i = 0; i < 3; i++)
      mvvline(2, NC - 1 - i, ACS_CKBOARD, 20);
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
  }

  //draw heroes
  if (hero & what) {
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
      print_middle(stdscr, 0, 0, NC, string_format(" Adversary has %d cards. ", len(adv->viz->state.cards)));
    }
    if (in(player, which)) {
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
      mvwaddstr_ex(stdscr, i, NC - 11 - len(text), text.c_str(), BLACK_on_CYAN);
      addwch_ex(9830, CYAN_on_BLACK, p->state.mana);
      addwch_ex(9826, CYAN_on_BLACK, p->state.max_mana - p->state.mana);
    }
  }

  // draw minions
  if (minions & what) {
    for (Player* pl : which)
      for (auto m : pl->viz->state.minions)
        m->viz->draw({ KEYPOS1("pos", get_minion_pos(m.get())) });
  }

  show_panels();
}


// ----- Human player interface -------

bool HumanPlayer::mouse_in_win(WINDOW* win, int y, int x) {
  get_win_pos(win, wy, wx);
  get_win_size(win, height, width);
  return wx <= x && x < wx + width && wy <= y && y < wy + height;
}

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
    int ch = getch();
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
  for (auto& card : cards ) 
    hide_panel(card->viz->panel);
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
      showlist.push_back({ a, act->thing->viz, {} });
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

    int ch = getch();

    if (ch == KEY_MOUSE) {
      MEVENT event;
      unsigned long mouse_state = getmouse_ex(&event);
      if (mouse_state == ERR) continue;

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
          choice = const_cast<Instance*>(CAST(sel, VizThing)->obj);
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
          for (auto& sl : engine->board.get_free_slots(const_cast<HumanPlayer*>(this)))
            showlist.push_back({ fake_act_slot, NEWP(VizSlot, sl), {} });
        } 
        else if (action->need_target() && !choice) {
          showlist.clear();
          for (auto obj : action->target.resolve(const_cast<HumanPlayer*>(this)))
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
    else if (ch == '\n') {  // enter
      if (!active)
        return end_turn_action;
    }
    else if (ch == 27) { // escape
      erase_elems(showlist);
      showlist = init_showlist;
      active = nullptr;
    }
    else {  // bad key = quit
      endwin();
      //printf(engine->log.c_str());
      exit(-1);
    }
  }
}

//# overloaded HS engine
CursesEngine::CursesEngine(Player* player1, Player* player2) :
  Engine(player1, player2) {
  logfile = fopen("log.txt", "w");
}

void CursesEngine::wait_for_display() {
  TRACE("wait_for_display()\n");
  while (display.size()) {
    PMessage msg = display.front();
    display.pop_front();

    msg->draw(this);
    string line = string_format("%s $ %s\n", msg->cls_name(), msg->tostr().c_str());
    fwrite(line.c_str(), line.size(), 1, logfile);
    TRACE(line.c_str());
    fflush(logfile);
    log += line;
  }
}

void CursesEngine::dbg_add_minion(Player* player, PCardMinion card) {
  NI;
  //card = deepcopy(card)
  //card.owner = player
  //engine.send_message(Msg_AddMinion(player, Minion(card), pos = engine.board.get_free_slots(player)[0]))
}