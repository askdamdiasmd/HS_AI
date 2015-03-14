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

#define get_win_size(win,ty,tx) int ty,tx; getmaxyx(win,ty,tx)
#define get_win_pos(win,y,x) int y,x; getbegyx(win,y,x)

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
  x = max(x, x + (width - len(text)) / 2);
  if (len(text) > width)  text = text.substr(0, width);

  SETWATTR(win, attr);
  int ret = mvwaddstr(win, y, x, text.c_str());
  UNSETWATTR(win, attr);
  return ret;
}

static int print_longtext(WINDOW* win, int y, int x, int endy, int endx, string text, int attr = 0) {
  if (!win) win = stdscr;
  assert(x >= 0 && y >= 0 && endx > x && endy > y);

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

VizPanel::~VizPanel() {
  if (win && panel) {
    delete_panel(panel);
    win = nullptr;
    panel = nullptr;
  }
}

//### General thing (minion, weapon, hero...)------

VizInstance::VizInstance(PInstance obj) :
  VizPanel(), obj(obj) {}

PVizThing Thing::viz_thing() { return issubclassP(viz, VizThing); }

VizThing::VizThing(PThing obj, int ty, int tx, int y, int x) :
  VizInstance(dynamic_pointer_cast<Instance>(obj)) {
  hp = obj->state.hp;
  max_hp = obj->state.max_hp;
  max_card_hp = 0; //max_card_hp = dynamic_pointer_cast<Card_Minion>(obj->card)->state.max_hp;
  assert(&max_hp == &hp + 1);
  assert(&max_card_hp == &hp + 2);
  atq = obj->state.atq;
  max_atq = obj->state.max_atq;
  max_card_atq = 0;
  assert(&max_atq == &atq + 1);
  assert(&max_card_atq == &atq + 2);
  effects = obj->state.static_effects;
  wait = 0;
  // create panel
  win = newwin(ty, tx, y, x);
  assert(win);
  panel = new_panel(win);
  set_panel_userptr(panel, this);
}

bool VizThing::check() {
  PThing obj = thing();
  assert(hp == obj->state.hp);
  assert(max_hp == obj->state.max_hp);
  assert(atq == obj->state.atq);
  assert(max_atq == obj->state.max_atq);
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
  else
    res = highlight ? WHITE_on_RED : RED_on_BLACK;
  return res | (standout ? A_STANDOUT : 0);
}

int VizThing::buff_color(const int* val, bool highlight, bool standout) const {
  int res;
  int max_val = *(val + 1);
  int max_card_val = *(val + 2);
  if (*val < max_val)
    res = highlight ? WHITE_on_RED : RED_on_BLACK;
  else if (*val < max_card_val)
    res = highlight ? WHITE_on_GREEN : GREEN_on_BLACK;
  else
    res = highlight ? WHITE_on_BLACK : 0;
  return res | (standout ? A_STANDOUT : 0);
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
  string thp = string_format(" %d ", hp);
  mvwaddstr_ex(win, ty - 1, tx - 1 - len(thp), thp.c_str(), highlight | buff_color(&hp));
  return win;
}

bool VizThing::update_stats(const Msg_Status& msg, bool show_hp) {
  const bool anim = thing()->engine->board.viz->animated;
  // update attribute
  int oldval = getattr(msg.attr);
  int newval = getattr(msg.attr) = msg.val;
  // animation if hp changed
  if (anim && show_hp && msg.attr == &VizThing::hp) {
    int diff = newval - oldval;
    if (diff)
      temp_panel(this, string_format("%+d", diff), buff_color(diff, true, false), 1.5);
  }
  draw({});
  return true;
}

//### Hero -----------

PVizHero Hero::viz_hero() { return issubclassP(viz, VizHero); }

VizHero::VizHero(PHero hero, pos_t pos) :
  VizThing(hero, 4, 13, pos.y, pos.x) {
  armor = hero->state.armor;
}

bool VizHero::check() {
  assert(armor == hero()->state.armor);
  return VizThing::check();
}

WINDOW* VizHero::draw(const ArgMap& args) {
  WINDOW* win = VizThing::draw(args);
  if (!win) return win;
  DEFARG(int, highlight, 0);
  get_win_size(win, ty, tx);
  print_middle(win, 1, 1, tx - 2, hero()->player->name);
  string hero_name = split(hero()->card->name)[0];
  print_middle(win, 2, 1, tx - 2, "(" + hero_name + ")", BLUE_on_BLACK);
  if (armor) {
    string tar = string_format("[%d]", armor);
    mvwaddstr(win, ty - 2, tx - 1 - len(tar), tar.c_str());
  }
  VizPlayer* pl = hero()->player->viz.get();
  int attack = (pl->state.weapon ? pl->state.weapon->state.atq : 0) + atq;
  if (attack) {
    int hh = pl->state.weapon ? pl->state.weapon->viz_weapon()->buff_color(&pl->state.weapon->viz_weapon()->atq) : 
                                buff_color(&atq);
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

PVizMinion Minion::viz_minion() { return issubclassP(viz, VizMinion); }

VizMinion::VizMinion(PMinion minion) :
  VizThing(minion, 5, 11,
  minion->engine->board.viz->get_minion_pos(minion).y,
  minion->engine->board.viz->get_minion_pos(minion).x) {}

WINDOW* VizMinion::draw(const ArgMap& args) {
  DEFARG(int, x, minion()->engine->board.viz->get_minion_pos(minion()).x);
  //DEFARG(int, y, 0);
  WINDOW* win = VizThing::draw(args);
  if (!win) return win;
  DEFARG(int, highlight, 0);
  get_win_size(win, ty, tx);
  string name = minion()->card->get_name_fr();
  print_longtext(win, 1, 1, ty - 1, tx - 1, name, YELLOW_on_BLACK);
  mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", atq).c_str(), highlight | buff_color(&atq));
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

VizWeapon::VizWeapon(PWeapon weapon) :
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
  mvwaddstr_ex(win, ty - 1, 1, string_format(" %d ", atq).c_str(), highlight | buff_color(&atq));
  return win;
}

bool VizWeapon::update_stats(const Msg_Status& msg) {
  return VizThing::update_stats(msg, false);
}

//### Player -----------

VizPlayer::VizPlayer(Player* player) :
  player(player), state(player->state) {}

bool VizPlayer::check() const {
  Player* pl = player;
  for (auto card : state.cards)
    assert(in(card, pl->state.cards));
  if (pl->state.weapon || state.weapon)
    state.weapon->viz_weapon()->check();
  assert(len(state.secrets) == len(pl->state.secrets));
  assert(len(state.minions) == len(pl->state.minions));
  for (int i = 0; i < len(state.minions); i++) {
    assert(state.minions[i] == pl->state.minions[i]);
    state.minions[i]->viz_minion()->check();
  }
  return true;
}

void VizPlayer::set_weapon(PWeapon weapon) {
  unset_weapon(state.weapon);
  weapon->viz = NEWP(VizWeapon, weapon);
  state.weapon = weapon;
}

void VizPlayer::unset_weapon(PWeapon weapon) {
  assert(weapon == state.weapon);
  if (state.weapon)
    state.weapon->viz.reset();
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
  int n = len(slot->player->viz->state.minions);
  int sp = get_screen_space();
  res.x = int(NC - 3 - (11 + sp)*n) / 2 + (11 + sp)*slot->pos;
  res.y = slot->player == slot->player->engine->board.viz->get_top_bottom_player(true) ? 6 : 14;
  return res;
}
int VizSlot::get_screen_space() const {
  int n = len(slot->player->viz->state.minions);
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
  DEFARG(int, petit, true);
  DEFARG(int, bkgd, 0);

  get_win_size(stdscr, NR, NC);
  string name = card->get_name_fr();
  string desc = card->get_desc_fr();

  WINDOW* win = nullptr;
  PANEL* panel = nullptr;
  if (!petit) {
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
      win = this->win, panel = this->panel;
      top_panel(panel);
    }
    win = this->win;
    panel = this->panel;
  }
  else {
    // petit card version
    petit = min(petit, card_size.y);
    if (this->panel)
      hide_panel(this->panel);
    if (!this->small_win) {
      int ty = petit, tx = card_size.x;
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
    if (petit > 1 && petit != ty) {
      delete_panel(panel);
      this->small_win = nullptr;
      this->small_panel = nullptr;
      ArgMap kwargs = args;
      kwargs["petit"]._int = petit;
      kwargs["pos"]._pos_t = pos;
      return draw(kwargs);
    }
    petit = getmaxy(win);
  }

  get_win_size(win, ty, tx);
  get_win_pos(win, y, x);
  if (pos!=nullpos && pos != pos_t(y,x))
    move_panel(panel, pos.y, pos.x);

  wbkgd(win, bkgd);
  if (highlight) wattron(win, highlight);
  box(win, ACS_VLINE, ACS_HLINE);
  if (0 < petit && petit < card_size.y) {
    mvwaddch(win, ty - 1, 0, ACS_VLINE);
    mvwaddch(win, ty - 1, tx - 1, ACS_VLINE);
  }
  if (highlight) wattroff(win, highlight);
  if (0 < petit && petit < card_size.y)
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

void Msg_CardDrawn::draw(Engine* engine) {
  if (!card->viz) 
    card->viz = NEWP(VizCard, card);
}


//### Board --------

VizBoard::VizBoard(Board* board, bool switch_heroes, bool animated) :
  board(board), switch_heroes(switch_heroes), animated(animated),
  end_turn(11, getmaxx(stdscr), "End turn", VizButton::right) {
  for (int i = 0; i < 2; i++) {
    Player* pl = board->engine->players[i];
    pl->viz = NEWP(VizPlayer, board->engine->players[i]);
    pl->state.hero->viz = NEWP(VizHero, pl->state.hero, get_hero_pos(pl));
    hero_power_buttons[pl] = pl->state.hero->viz_hero()->create_hero_power_button();
  }
}

Player* VizBoard::get_top_bottom_player(bool top) {
  Player* player = board->engine->get_current_player();
  Player* adv = board->engine->get_other_player();
  if (switch_heroes == false && board->engine->turn % 2)
    return top ? player : adv; // prevent top / down switching
  else
    return top ? adv : player;
}

pos_t VizBoard::get_minion_pos(PMinion minion) {
  int i = index(minion->player->viz->state.minions, minion);
  assert(i >= 0);
  Slot slot(minion->player, i, -1);
  return VizSlot(&slot).get_screen_pos();
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
    end_turn.draw({});
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
      ListCard cards = adv->viz->state.cards;
      if (!last_card) cards.pop_back();
      for (auto card : cards)
        card->viz->draw({ KEYBOOL("hide", true) }); // hide adversary cards
      print_middle(stdscr, 0, 0, NC, string_format(" Adversary has %d cards. ", len(adv->viz->state.cards)));
      if (in(player, which)) {
        ListCard cards = player->viz->state.cards;
        if (!last_card) cards.pop_back();
        for (auto card : cards)
          card->viz->draw({ KEYPOS1("pos", get_card_pos(card, player)), KEYINT("small", NR - 24) });
      }
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
        m->viz->draw({ KEYINT("y", get_minion_pos(m).y) });
  }

  show_panels();
}


// ----- Human player interface -------

bool HumanPlayer::mouse_in_win(WINDOW* win, int y, int x) {
  get_win_pos(win, wy, wx);
  get_win_size(win, height, width);
  return wx <= x && x < wx + width && wy <= y && y < wy + height;
}

ListCard HumanPlayer::mulligan(ListCard & cards) const {
  engine->board.viz->draw();
  engine->wait_for_display();

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
  
  ListCard discarded;
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
  for (auto card : cards ) 
    hide_panel(card->viz->panel);
  end_button.~VizButton();
  show_panels();

  return discarded;
}

const Action* HumanPlayer::choose_actions(ListAction actions) const {
  /*
  // split actions
  struct ShowElem {
    const Action* action;
    VizPanel* viz;
    ArgMap kwargs; 
  };
  vector<ShowElem> showlist; // [(action, object, draw_kwargs)]
  ListCard remaining_cards = state.cards; // copy
  for (auto a : actions) {
    if (issubclass(a, Act_HeroPower)) {
      showlist.push_back({ a, &engine->board.viz->hero_power_buttons[this], {} });
    }
    else if (issubclass(a, Act_PlayCard)) {
      showlist.push_back({ a, a->viz, { 'small':True, 'cost' : a.cost }));
      if (in(a->card, remaining_cards)) // choice_of_cards
        remove(remaining_cards, a->card);
    }
    else if (issubclass(a, Act_MinionAttack)) {
      showlist.push_back((a, a->caster, {}));
    }
    else if (issubclass(a, Act_WeaponAttack)){
      showlist.push_back((a, this->state.hero->viz, {}));
    }
    else if (issubclass(a, Act_HeroAttack)) {
      showlist.push_back((a, this->state.hero->viz, {}));
    }
    else:
    end_turn_action = a
      showlist.push_back((a, self.engine.board.viz.end_turn, {}))
  }
  // we can also inspect non - playable cards
  for (auto card : remaining_cards)
    showlist.push_back((None, card, { 'small':True }))
  
  def erase_elems(showlist) :
  for a, obj, kwargs in showlist :
  obj.draw(small = True)  # erase everything
  
  init_showlist = showlist
  self.engine.board.draw()
  last_sel = None
  active = None
  while True:
  mapping = {}  # obj->action, draw_kwargs
  for a, obj, kwargs in showlist :
  highlight = black_on_green if a else 0
  assert hasattr(obj, 'draw'), debug()
  obj.draw(highlight = highlight, **kwargs)
  mapping[obj] = (a, kwargs)
  if active :
  active.draw(bkgd = black_on_white)
  show_panels()
  
  ch = getch()
  
  if ch == KEY_MOUSE:
  mouse_state = getmouse()
  if mouse_state == ERR : continue
  id, x, y, z, bstate = mouse_state
  #mvaddstr(5, 0, "mouse %d %d %s" % (y, x, bin(bstate)))
  
  sel = None
  cur = my_top_panel()
  while cur:
  obj = get_panel_userptr(cur)
  if issubclass(type(obj), Card) or obj in mapping :
  if self.mouse_in_win(panel_window(cur), y, x) :
  sel = obj
  break
  cur = panel_below(cur)
  
  # reset everybody
  for a, kwargs in mapping.values() :
  kwargs['bkgd'] = 0
  kwargs['small'] = True
  
  if bstate & BUTTON1_PRESSED :
  # set current all green
  if sel in mapping :
  a, kwargs = mapping[sel]
  kwargs['bkgd'] = black_on_green if a else 0
  kwargs['small'] = False
  last_sel = sel
  
  elif bstate & BUTTON1_RELEASED:
  if sel != last_sel : continue
  if sel not in mapping : continue
  act, kwargs = mapping[sel]
  if not act : continue
  
  if issubclass(type(act), Action) :
  action = act
  active = sel
  choices = []  # reset choices
  else:
  choices.append(act)
  erase_elems(showlist)
  while True :
  if len(choices) >= len(action.choices) :
  return action.select(choices)
  elif not action.choices[len(choices)] : # no choices!
  choices.append(None)
  else :
  # propose new choices
  showlist = [(obj, obj, {}) for obj in action.choices[len(choices)]]
  break
  
  else:
  erase_elems(showlist)
  showlist = init_showlist
  active = None
  
  elif ch == ord('\n') :
  if active == None :
  return end_turn_action.select(())
  
  elif ch == 27 : # escape
  erase_elems(showlist)
  showlist = init_showlist
  active = None
  elif ch in(CCHAR('d'), CCHAR('p')) :
  debug()
  else:
  endwin()
  print self.engine.log
  sys.exit()*/
  NI; return nullptr;
}

//# overloaded HS engine
CursesEngine::CursesEngine(Player* player1, Player* player2) :
  Engine(player1, player2) {
  logfile = fopen("log.txt", "w");
}

void CursesEngine::wait_for_display() {
  while (display.size()) {
    PMessage msg = display.front();
    display.pop_front();

    msg->draw(this);
    string line = string_format("%s $ %s\n", msg->cls_name(), msg->tostr().c_str());
    fwrite(line.c_str(), line.size(), 1, logfile);
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