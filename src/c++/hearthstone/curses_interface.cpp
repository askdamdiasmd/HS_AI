#include <Windows.h>
#include "curses_interface.h"
#include "common.h"
#include "engine.h"
#include "creatures.h"
#include "Cards.h"


#define CYAN_on_BLACK     1
#define YELLOW_on_BLACK   2
#define MAGENTA_on_BLACK  3
#define BLUE_on_BLACK     4
#define RED_on_BLACK      5
#define GREEN_on_BLACK    6
#define WHITE_on_BLACK    7
#define WHITE_on_RED      8
#define WHITE_on_GREEN    9
#define BLACK_on_WHITE    10
#define BLACK_on_GREEN    11
#define BLACK_on_RED      12
#define BLACK_on_CYAN     13
#define BLACK_on_BLUE     14
#define BLACK_on_YELLOW   15

void extend_console(int min_NR, int min_NC) {
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
  //mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION);
  
  // init color pairs
  #define create_color_pair(col_fg,col_bg)  init_pair(col_fg##_on_##col_bg, COLOR_##col_fg, COLOR_##col_bg);
  create_color_pair(CYAN, BLACK);
  create_color_pair(YELLOW, BLACK);
  create_color_pair(MAGENTA, BLACK);
  create_color_pair(BLUE, BLACK);
  create_color_pair(RED, BLACK);
  create_color_pair(GREEN, BLACK);
  create_color_pair(WHITE, BLACK);
  create_color_pair(WHITE, RED);
  create_color_pair(WHITE, GREEN);
  create_color_pair(BLACK, WHITE);
  create_color_pair(BLACK, GREEN);
  create_color_pair(BLACK, RED);
  create_color_pair(BLACK, CYAN);
  create_color_pair(BLACK, BLUE);
  create_color_pair(BLACK, YELLOW);
  #undef create_color_pair
}

void addwch(chtype ch, int attr, WINDOW* win, int y, int x, int nb) {
  if (!win) win = stdscr;
  // display unicode character
  if (x < 0)
    getyx(win, y, x);
  else
    wmove(win, y, x);
  whline(win, ch | attr, nb);
  wmove(win, y, x + nb);
}

#define SETWATTR(win,attr) \
  int oldattr = 0; \
  if (attr) {\
    oldattr = getattrs(win);\
    wattrset(win, attr);\
    }
#define UNSETWATTR(win,attr) \
  if (attr) wattrset(win, oldattr);

int print_middle(WINDOW* win, int y, int x, int width, string text, int attr) {
  if (!win) win = stdscr;
  x = max(x, x + (width - len(text)) / 2);
  if (len(text) > width)  text = text.substr(0, width);

  SETWATTR(win, attr);
  int ret = mvwaddstr(win, y, x, text.data());
  UNSETWATTR(win, attr);
  return ret;
}

int print_longtext(WINDOW* win, int y, int x, int endy, int endx, string text, int attr) {
  if (win) win = stdscr;

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

void delete_panel(PANEL* panel) {
  WINDOW* win = panel_window(panel);
  del_panel(panel);
  delwin(win);
}

PANEL* top_panel() {
  // due to a bug in unicurses we recode it here
#ifdef _WIN32
  return panel_below(nullptr); // get top panel
#else
  NI; return nullptr; // curses.panel.top_panel();
#endif
}

void debug() {
  assert(0);
  /*endwin()
  pdb.set_trace()
  global stdscr
  stdscr = init_screen()
  touchwin(stdscr)
  show_panels()*/
}

void show_ACS() {
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

void show_unicode() {
  clear();
  int NR, NC;
  getmaxyx(stdscr, NR, NC);
  for (int i = 1; i < 65536; ++i) {
    waddch(stdscr,i);
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

void strong_box(WIN* win, int attr) {
  //wborder(win, *((ACS_BLOCK, ) * 8))
#ifdef _WIN32
  wborder(win, 9553, 9553, 9552, 9552, 9556, 9559, 9562, 9565);
#else
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
#endif
}

void weak_box(WIN* win, int attr) {
  wborder(win, ':', ':', '-', '-', ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
}



//# Viz classes = copy object specs --------------------

//### General thing (minion, weapon, hero...)------

VizInstance::VizInstance(PInstance obj) :
  VizPanel(), _obj(obj) {}

VizInstance::~VizInstance() {
  assert(win && panel);
  delete_panel(panel);
  win = nullptr;
  panel= nullptr;
}

VizThing::VizThing(PThing obj, int ty, int tx, int y, int x) :
  VizInstance(dynamic_pointer_cast<Instance>(obj)) {
  hp = obj->state.hp;
  max_hp = obj->state.max_hp;
  max_card_hp = 0; //max_card_hp = dynamic_pointer_cast<Card_Minion>(obj->card)->state.max_hp;
  assert(&max_hp == &hp + sizeof(int));
  assert(&max_card_hp == &hp + 2 * sizeof(int));
  atq = obj->state.atq;
  max_atq = obj->state.max_atq;
  max_card_atq = 0;
  assert(&max_atq == &atq + sizeof(int));
  assert(&max_card_atq == &atq + 2 * sizeof(int));
  effects = obj->state.static_effects;
  wait = 0;
  status_id = 0;
  // create panel
  win = newwin(ty, tx, y, x);
  panel = new_panel(win);
  set_panel_userptr(panel, this);
}

void VizThing::check() {
  PThing obj = thing();
  assert(hp == obj->state.hp);
  assert(max_hp == obj->state.max_hp);
  assert(atq == obj->state.atq);
  assert(max_atq == obj->state.max_atq);
}

VizThing::~VizThing() {
  float t = 0;
  while (wait && t < 5) {
    Sleep(0.1);
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
  int max_val = *(val + sizeof(int));
  int max_card_val = *(val + 2 * sizeof(int));
  if (*val < max_val)
    res = highlight ? WHITE_on_RED : RED_on_BLACK;
  else if (*val < max_card_val)
    res = highlight ? WHITE_on_GREEN : GREEN_on_BLACK;
  else
    res = highlight ? WHITE_on_BLACK : 0;
  return res | (standout ? A_STANDOUT : 0);
}

WIN* VizThing::draw(ArgMap& args) {
  DEFARG(int, x, 0);
  DEFARG(int, y, 0);
  DEFARG(int, bkgd, 0);
  DEFARG(int, highlight, 0);
  if (!win) return win;
  get_win_pos(win, px, py);
  if (x && y && (x != px || y != py))
    move_panel(panel, y, x);
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
  mvwaddstr(win, ty - 1, tx - 1 - len(thp), thp, highlight | buff_color(&hp));
  return win;
}

bool VizThing::update_stats(const Msg_Status& msg, bool show_hp = true) {
  if (status_id != msg.status_id) return false;
  status_id++;
  const bool anim = obj->engine->board->viz->animated;
  // update attribute
  int oldval = getattr(msg.attr);
  int newval = getattr(msg.attr) = msg.val;
  // animation if hp changed
  if (anim && show_hp && msg.attr == &VizThing::hp) {
    diff = newval - oldval;
    if (diff)
      temp_panel(string_format("%+d", diff), buff_color(diff, true, false), 1.5);
  }
  draw();
  return true;
}




//### Board --------

VizBoard::VizBoard(Board* board, bool switch_heroes = false, bool animated = true) :
  board(board), switch_heroes(switch_heroes), animated(animated),
  end_turn(11, getmaxy(stdscr), "End turn", right) {
  for (int i = 0; i < 2; i++) {
    Player* pl = engine->players[i];
    pl->viz = NEWP(VizPlayer, engine->players[i]);
    pl->hero->viz = NEWP(VizHero, pl->hero, get_hero_pos(pl));
    hero_power_buttons[pl] = pl->hero->viz->create_hero_power_button();
  }
}



// ----- Human player interface -------

void HumanPlayerCurses::mulligan(ListCard cards) {
  engine->board.viz->draw();

  int NR, NC; getmaxyx(stdscr,NR,NC);
  int nc = len(cards);
  VizButton end_button(25, (NC - 6) / 2, "  OK  ");
  /*
  showlist = [(end_button, dict(highlight = black_on_blue))]
  for i, card in enumerate(cards) :
    showlist.append((card, dict(pos = (6, int((NC - 6)*(i + 0.5) / nc - 7)),
    small = False, highlight = black_on_green)))

      discarded = []
      while True :
        for card, kwargs in showlist :
  card.draw(**kwargs)
    show_panels()
    ch = getch()
    if ch == KEY_MOUSE :
      mouse_state = getmouse()
      if mouse_state == ERR : continue
        id, x, y, z, bstate = mouse_state

        which = None
        for card, kwargs in showlist :
  if self.mouse_in_win(card.win, y, x) :
    which = card, kwargs
    break

    if bstate & BUTTON1_PRESSED :
      if not which : continue
        if card is end_button :
  kwargs['bkgd'] = black_on_blue
        else :
        if card in discarded :
  kwargs['bkgd'] = 0
    discarded.remove(card)
        else :
        kwargs['bkgd'] = black_on_red
        discarded.append(card)

        elif bstate & BUTTON1_RELEASED :
        if card is end_button :
  break
        else :
        # reset end button
        kwargs = showlist[0][1]
        kwargs['bkgd'] = 0

        elif ch in(ord(' '), ord('\n')) :
        break

        # clean up
        end_button.delete()
        for card in cards :
  hide_panel(card.panel)
    show_panels()

    return discarded;
    */
}

Action* HumanPlayerCurses::choose_actions(ListAction actions) {
  //# split actions
  //showlist = [] # [(action, object, draw_kwargs)]
  //remaining_cards = set(self.cards)
  //for a in actions :
  //if issubclass(type(a), Act_HeroPower) :
  //showlist.append((a, self.engine.board.viz.hero_power_buttons[a.caster], {}))
  //elif issubclass(type(a), Act_PlayCard) :
  //showlist.append((a, a.card, { 'small':True, 'cost' : a.cost }))
  //if a.card in remaining_cards : # choice_of_cards
  //remaining_cards.remove(a.card)
  //elif issubclass(type(a), Act_MinionAttack) :
  //showlist.append((a, a.caster, {}))
  //elif issubclass(type(a), Act_WeaponAttack) :
  //showlist.append((a, a.caster.hero, {}))
  //elif issubclass(type(a), Act_HeroAttack) :
  //showlist.append((a, a.caster, {}))
  //else:
  //end_turn_action = a
  //showlist.append((a, self.engine.board.viz.end_turn, {}))
  //# we can also inspect non - playable cards
  //for card in remaining_cards :
  //showlist.append((None, card, { 'small':True }))
  //
  //def erase_elems(showlist) :
  //for a, obj, kwargs in showlist :
  //obj.draw(small = True)  # erase everything
  //
  //init_showlist = showlist
  //self.engine.board.draw()
  //last_sel = None
  //active = None
  //while True:
  //mapping = {}  # obj->action, draw_kwargs
  //for a, obj, kwargs in showlist :
  //highlight = black_on_green if a else 0
  //assert hasattr(obj, 'draw'), debug()
  //obj.draw(highlight = highlight, **kwargs)
  //mapping[obj] = (a, kwargs)
  //if active :
  //active.draw(bkgd = black_on_white)
  //show_panels()
  //
  //ch = getch()
  //
  //if ch == KEY_MOUSE:
  //mouse_state = getmouse()
  //if mouse_state == ERR : continue
  //id, x, y, z, bstate = mouse_state
  //#mvaddstr(5, 0, "mouse %d %d %s" % (y, x, bin(bstate)))
  //
  //sel = None
  //cur = top_panel()
  //while cur:
  //obj = get_panel_userptr(cur)
  //if issubclass(type(obj), Card) or obj in mapping :
  //if self.mouse_in_win(panel_window(cur), y, x) :
  //sel = obj
  //break
  //cur = panel_below(cur)
  //
  //# reset everybody
  //for a, kwargs in mapping.values() :
  //kwargs['bkgd'] = 0
  //kwargs['small'] = True
  //
  //if bstate & BUTTON1_PRESSED :
  //# set current all green
  //if sel in mapping :
  //a, kwargs = mapping[sel]
  //kwargs['bkgd'] = black_on_green if a else 0
  //kwargs['small'] = False
  //last_sel = sel
  //
  //elif bstate & BUTTON1_RELEASED:
  //if sel != last_sel : continue
  //if sel not in mapping : continue
  //act, kwargs = mapping[sel]
  //if not act : continue
  //
  //if issubclass(type(act), Action) :
  //action = act
  //active = sel
  //choices = []  # reset choices
  //else:
  //choices.append(act)
  //erase_elems(showlist)
  //while True :
  //if len(choices) >= len(action.choices) :
  //return action.select(choices)
  //elif not action.choices[len(choices)] : # no choices!
  //choices.append(None)
  //else :
  //# propose new choices
  //showlist = [(obj, obj, {}) for obj in action.choices[len(choices)]]
  //break
  //
  //else:
  //erase_elems(showlist)
  //showlist = init_showlist
  //active = None
  //
  //elif ch == ord('\n') :
  //if active == None :
  //return end_turn_action.select(())
  //
  //elif ch == 27 : # escape
  //erase_elems(showlist)
  //showlist = init_showlist
  //active = None
  //elif ch in(CCHAR('d'), CCHAR('p')) :
  //debug()
  //else:
  //endwin()
  //print self.engine.log
  //sys.exit()
  NI; return nullptr;
}