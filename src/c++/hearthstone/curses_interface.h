#ifndef __CURSES_INTERFACE_H__
#define __CURSES_INTERFACE_H__
#include <stdlib.h>
#include <curses.h>
#include <panel.h>
#include "common.h"


void addwch(chtype ch, int attr = 0, WINDOW* win = nullptr, int y = -1, int x = -1, int nb = 1) {
  if (!win) win = stdscr;
  // display unicode character
  if (x < 0)
    wmove(win, y, x);
  else
    getyx(win, y, x);
  whline(win, ch | attr, nb);
  wmove(win, y, x + nb);
}

void init_screen();

#define SETWATTR(win,attr) \
  int oldattr = 0; \
  if (attr) {\
    oldattr = getattrs(win);\
    wattrset(win, attr);\
  }
#define UNSETWATTR(win,attr) \
  if (attr) wattrset(win, oldattr);

int print_middle(WINDOW* win, int y, int x, int width, string text, int attr = 0) {
  if (!win) win = stdscr;
  x = max(x, x + (width - len(text)) / 2);
  if (text.size() > width)  text = text.substr(0, width);

  SETWATTR(win, attr);
  int ret = mvwaddstr(win, y, x, text.data());
  UNSETWATTR(win, attr);
  return ret;
}

int print_longtext(WINDOW* win, int y, int x, int endy, int endx, string text, int attr = 0) {
  if (win) win = stdscr;
  
  vector<string> words = split(text);

  int width = endx - x;
  while (!words.empty() && y < endy) {
    // pick enough text to fill one line
    string line = pop_front(words);

    while (!words.empty() && (line + words[0]).size() + 1<width)
      line += " " + pop_front(words);

    if (line.size() > width) {
      words.insert(words.begin(), "-" + line.substr(width,999));
      line = line.substr(0,width);
    }
    print_middle(win, y, x, width, line, attr);
    y++;
  }
}

void delete_panel(PANEL* panel) {
  WINDOW* win = panel_window(panel);
  del_panel(panel);
  delwin(win);
}

//def top_panel() :
//''' due to a bug in unicurses we recode it here '''
//if os.name == 'nt' :
//return panel_below(None) # get top panel
//else :
//return curses.panel.top_panel()

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
    addwch(i);
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


//def rounded_box(win) :
//ty, tx = getmaxyx(win)
//box(win)
//addwch(2320, win = win, x = 0, y = 0)
//addwch(2321, win = win, x = tx - 1, y = 0)
//
//def strong_box(win, attr = 0) :
//#    wborder(win, *((ACS_BLOCK, ) * 8))
//if code == 'UTF-8':
//h, w = getmaxyx(win)
//w -= 1
//h -= 1
//addwch(9556, attr, win = win, x = 0, y = 0)
//addwch(9552, attr, win = win, nb = w - 1)
//addwch(9559, attr, win = win)
//for i in range(1, h) :
//addwch(9553, attr, win = win, x = 0, y = i)
//addwch(9553, attr, win = win, x = w, y = i)
//addwch(9562, attr, win = win, x = 0, y = h)
//addwch(9552, attr, win = win, nb = w - 1)
//addwch(9565, attr, win = win)
//else:
//wborder(win, 9553, 9553, 9552, 9552, 9556, 9559, 9562, 9565)
//
//def weak_box(win, attr = 0) :
//wborder(win, ord(':'), ord(':'), ord('-'), ord('-'))
//
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
//
//
//panel_to_obj = []
//def set_panel_userptr(panel, obj) :
//set_panel_userptr(panel, len(panel_to_obj))
//panel_to_obj.append(obj)
//def get_panel_userptr(panel) :
//return panel_to_obj[panel_userptr(panel)]
//
//
//show_panel_lock = Lock()
//def show_panels() :
//show_panel_lock.acquire()
//update_panels()
//doupdate()
//show_panel_lock.release()
//
//# Viz classes = copy object specs
//
//
//### General thing (minion, weapon, hero...)------
//
//class VizThing(object) :
//  def __init__(self, obj, size, pos) :
//  self.obj = obj
//  self.hp = obj.hp
//  self.max_hp = obj.max_hp
//  self.atq = obj.atq
//  self.max_atq = obj.max_atq
//  self.effects = copy(obj.effects)
//  obj.draw = self.draw
//  self.wait = 0
//  self.status_id = 0
//  # create panel
//  self.win = newwin(size[0], size[1], pos[0], pos[1])
//  self.panel = new_panel(self.win)
//  set_panel_userptr(self.panel, obj)
//
//  def check(self) :
//  obj = self.obj
//  assert self.hp == obj.hp, debug()
//  assert self.max_hp == obj.max_hp, debug()
//  assert self.atq == obj.atq, debug()
//  assert self.max_atq == obj.max_atq, debug()
//
//  def delete(self) :
//  t = 0
//  while self.wait and t<5 :
//    time.sleep(0.1)
//    t += 0.1
//    self.wait = 0
//    if t >= 5 : debug()
//      if hasattr(self, 'panel') :
//        del_panel(self.panel)
//        del self.panel
//        del self.win
//
//        def buff_color(self, val, highlight = False, standout = False) :
//        if type(val) == int :
//          if val>0:
//res = white_on_green if highlight else green_on_black
//elif val == 0 :
//res = white_on_black if highlight else 0
//          else :
//          res = white_on_red if highlight else red_on_black
//          elif type(val) == str :
//          if getattr(self, val)<getattr(self, 'max_' + val) :
//            res = white_on_red if highlight else red_on_black
//            elif getattr(self, val)>getattr(self.obj.card, val) :
//            res = white_on_green if highlight else green_on_black
//          else :
//          res = white_on_black if highlight else 0
//        else:
//assert False, debug()
//return res + (A_STANDOUT if standout else 0)
//
//def draw(self, pos = None, y = None, bkgd = 0, highlight = 0, **kwargs) :
//if not hasattr(self, 'win') : return None # thing was already deleted
//win, panel = self.win, self.panel
//wpos = getbegyx(win)
//if pos and pos != wpos:
//move_panel(panel, *pos)
//elif y and y != wpos[0] :
//move_panel(panel, y, wpos[1])
//
//if 'frozen' in self.effects or 'insensible' in self.effects :
//bkgd = black_on_cyan
//elif 'divine_shield' in self.effects :
//bkgd = black_on_yellow
//
//wbkgd(win, bkgd)
//wattron(win, highlight)
//if 'stealth' in self.effects :
//weak_box(win, highlight)
//elif 'taunt' in self.effects :
//strong_box(win, highlight)
//else:
//box(win)
//wattroff(win, highlight)
//# show just HP
//ty, tx = getmaxyx(win)
//thp = " %d "%self.hp
//mvwaddstr(win, ty - 1, tx - 1 - len(thp), thp, highlight | self.buff_color('hp'))
//return win
//
//def update_stats(self, msg, show_hp = True) :
//if self.status_id != msg.status_id : return False
//self.status_id += 1
//anim = self.obj.engine.board.viz.animated
//for attr in msg.attrs :
//oldval = getattr(self, attr)
//newval = getattr(msg, attr)
//setattr(self, attr, newval)
//if anim and show_hp and msg.attrs == ['hp'] :
//diff = newval - oldval
//if diff :
//temp_panel(self, "%+d"%diff, self.buff_color(diff, highlight = 1), duration = 1.5)
//self.draw()
//return True
//
//
//### Hero -----------
//
//class VizHero(VizThing) :
//  size = (4, 13)
//  def __init__(self, hero, pos) :
//  VizThing.__init__(self, hero, self.size, pos)
//  self.armor = hero.armor
//
//  def check(self) :
//  assert self.armor == obj.armor, debug()
//
//  def draw(self, **kwargs) :
//  win = VizThing.draw(self, **kwargs)
//  if not win : return
//    highlight = kwargs.get('highlight', 0)
//    ty, tx = getmaxyx(win)
//    hero = self.obj
//    print_middle(win, 1, 1, tx - 2, hero.owner.name)
//    hero_name = hero.card.name[:(hero.card.name + ' ').find(' ')]
//    print_middle(win, 2, 1, tx - 2, "(%s)"%hero_name, blue_on_black)
//    if self.armor:
//tar = "[%d]"%self.armor
//mvwaddstr(win, ty - 2, tx - 1 - len(tar), tar)
//pl = hero.owner.viz
//atq = pl.weapon.atq if pl.weapon else self.atq
//if atq :
//hh = pl.weapon.viz.buff_color('atq') if pl.weapon else self.buff_color('atq')
//mvwaddstr(win, ty - 1, 1, " %d "%atq, highlight | hh)
//
//def create_hero_power_button(self) :
//card = self.obj.card
//y, x = getbegyx(self.win)
//name = card.ability.name.split()
//if len(name) == 1 :
//name = name[0]
//name = name[:4], [name[4:]]
//up, down = name[0], ' '.join(name[1])
//return HeroPowerButton(y, x + 24, up, down, card.ability.cost, tx = 9, ty = 4)
//
//
//### Minion -----------
//
//class VizMinion(VizThing) :
//  size = 5, 11
//  def __init__(self, minion) :
//  pos = minion.engine.board.viz.get_minion_pos(minion)
//  VizThing.__init__(self, minion, pos = pos, size = self.size)
//
//  def draw(self, pos = None, **kwargs) :
//  minion = self.obj
//  if pos == None : pos = minion.engine.board.viz.get_minion_pos(minion)
//    win = VizThing.draw(self, pos = pos, **kwargs)
//    if not win : return
//      highlight = kwargs.get('highlight', 0)
//      ty, tx = getmaxyx(win)
//      name = minion.card.name_fr or minion.card.name
//      print_longtext(win, 1, 1, ty - 1, tx - 1, name, yellow_on_black)
//      mvwaddstr(win, ty - 1, 1, " %d "%self.atq, highlight | self.buff_color('atq'))
//      if 'death_rattle' in self.effects:
//mvwaddstr(win, ty - 1, tx / 2, 'D', highlight)
//elif 'effect' in self.effects :
//mvwaddstr(win, ty - 1, tx / 2, 'Z', highlight)
//if 'silence' in self.effects :
//line = derwin(win, 1, tx, ty / 2, 0)
//wbkgd(line, 0, black_on_red)
//mvwchgat(line, 0, 1, tx - 2, black_on_red, 0)
//delwin(line)
//
//
//class VizWeapon(VizThing) :
//  size = 4, 11
//  def __init__(self, weapon) :
//  y, x = getbegyx(weapon.hero.viz.win)
//  VizThing.__init__(self, weapon, pos = (y, x - 18), size = self.size)
//
//  def draw(self, **kwargs) :
//  weapon = self.obj
//  win = VizThing.draw(self, **kwargs)
//  if not win : return
//    highlight = kwargs.get('highlight', 0)
//    ty, tx = getmaxyx(win)
//    name = weapon.card.name_fr or weapon.card.name
//    print_longtext(win, 1, 1, ty - 1, tx - 1, name, green_on_black)
//    ref_atq = min(self.obj.card.atq, self.max_atq)
//    mvwaddstr(win, ty - 1, 1, " %d "%self.atq, highlight | self.buff_color('atq'))
//
//    def update_stats(self, msg) :
//    return VizThing.update_stats(self, msg, show_hp = False)
//
//
//### Player -----------
//
//class VizPlayer :
//  def __init__(self, player) :
//  self.player = player
//  self.cards = []
//  self.minions = []
//  self.weapon = None
//  self.secrets = []
//
//  def check(self) :
//  pl = self.player
//  for card in self.cards :
//    assert card in pl.cards, debug()
//    if pl.weapon or self.weapon :
//      self.weapon.viz.check()
//      assert len(self.secrets) == len(pl.secrets), debug()
//      assert len(self.minions) == len(pl.minions), debug()
//      for i, m in enumerate(self.minions) :
//        assert m is pl.minions[i], debug()
//        m.viz.check()
//
//        def set_weapon(self, weapon) :
//        self.unset_weapon(self.weapon)
//        weapon.viz = VizWeapon(weapon)
//        self.weapon = weapon
//
//        def unset_weapon(self, weapon) :
//        assert weapon is self.weapon
//        if self.weapon :
//          self.weapon.viz.delete()
//          self.weapon = None
//
//
//### Button -----------
//
//class Button :
//  def __init__(self, y, x, text, align = 'center', tx = 0, ty = 3) :
//  self.text = text
//  if tx == 0 : tx = len(text) + 2
//    if align == 'left' : pass
//      elif align == 'center' : x -= tx / 2
//      elif align == 'right' : x -= tx
//      win = self.win = newwin(ty, tx, y, x)
//      self.panel = new_panel(win)
//      set_panel_userptr(self.panel, self)
//
//      def delete(self) :
//      del_panel(self.panel)
//      del self.panel
//      del self.win
//
//      def draw(self, y = None, highlight = 0, bkgd = 0, box = True, ytext = None, coltext = 0, **kwargs) :
//      wpos = getbegyx(self.win)
//      if y and y != wpos[1] : move_panel(self.panel, y, wpos[1])
//        wbkgd(self.win, bkgd)
//        if box :
//          wattron(self.win, highlight)
//          box(self.win)
//          wattroff(self.win, highlight)
//          ty, tx = getmaxyx(self.win)
//          print_middle(self.win, ytext or ty / 2, 1, tx - 2, self.text, coltext)
//
//class HeroPowerButton(Button) :
//  def __init__(self, y, x, text, subtext, cost = 2, **kwargs) :
//  Button.__init__(self, y, x, text, **kwargs)
//  self.subtext = subtext
//  self.cost = cost
//  self.used = False
//
//  def draw(self, blink = 0, **kwargs) :
//  coltext = yellow_on_black
//  if self.used : kwargs['bkgd'] = black_on_yellow
//    Button.draw(self, ytext = 1, coltext = coltext, **kwargs)
//    if blink :
//      for i in range(int(10 * blink)) :
//        kwargs['bkgd'] = [black_on_yellow, yellow_on_black][i % 2]
//        Button.draw(self, ytext = 1, coltext = coltext, **kwargs)
//        show_panels()
//        time.sleep(0.1)
//        ty, tx = getmaxyx(self.win)
//        mvwaddstr(self.win, 0, tx / 2 - 1, "(%d)"%self.cost, cyan_on_black)
//        print_longtext(self.win, 2, 1, ty - 1, tx - 1, self.subtext, coltext)
//
//
//
//        def temp_panel(viz, text, color, duration = 2) :
//        assert issubclass(type(viz), VizThing), debug()
//        viz.wait += 1
//        y, x = getbegyx(viz.win)
//        ty, tx = getmaxyx(viz.win)
//        button = Button(y + ty / 2 - 1, x + tx / 2, text)
//        button.draw(box = False, bkgd = color)
//        def wait_delete(duration, button, viz) :
//        t = 0
//        while t<duration :
//          show_panel_lock.acquire()
//          touchwin(button.win)
//          top_panel(button.panel)  # remains at top
//          show_panel_lock.release()
//          show_panels()
//          time.sleep(0.1)
//          t += 0.1
//          show_panel_lock.acquire()
//          button.delete()
//          show_panel_lock.release()
//          show_panels()
//          viz.wait -= 1
//          Thread(target = wait_delete, args = (duration, button, viz)).start()
//
//
//
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
//del_panel(self.panel)
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
//del_panel(panel)
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
//        print_longtext(win, y + 1, x + 1, y + h - 1, x + w - 1, name, yellow_on_black)
//        mvwaddstr(win, y + h - 1, x + 1, "%2d "%self.atq)
//        mvwaddstr(win, y + h - 1, x + w - 4, "%2d "%self.hp)
//        print_longtext(win, mid, 2, ty - 1, tx - 2, desc)
//      else:
//r = 4 if issubclass(type(self), Card_Weapon) else 3
//mvwaddch(win, r, 0, ACS_LTEE, highlight)
//mvwhline(win, r, 1, ACS_HLINE, tx - 2)
//mvwaddch(win, r, tx - 1, ACS_RTEE, highlight)
//if issubclass(type(self), Card_Weapon) :
//name_color = green_on_black
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
//mvwaddstr(win, 0, 0, "(%d)"%cost, black_on_cyan)
//elif cost<self.cost :
//mvwaddstr(win, 0, 0, "(%d)"%cost, white_on_green)
//else:
//mvwaddstr(win, 0, 0, "(%d)"%cost, white_on_red)
//
//
//def card_delete(self) :
//if hasattr(self, 'panel') :
//del_panel(self.panel)
//del self.win
//del self.panel
//if hasattr(self, 'small_panel') :
//del_panel(self.small_panel)
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
//button.draw(highlight = black_on_yellow)
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
//card.draw(highlight = black_on_yellow, pos = (y, x), small = 0 if h >= ty else h)
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
//self.card.draw(pos = (0, sx), highlight = black_on_yellow, **kwargs)
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
//anim_magic_burst(self.engine, get_center(button), get_center(self.target.viz), ord('*'), black_on_red, erase = True)
//
//
//
//### Board --------
//
//class VizBoard :
//  def __init__(self, board, switch = False, animated = True) :
//  self.board = board
//  self.engine = board.engine
//  board.draw = self.draw
//  self.switch = switch # switch heroes or not
//  self.animated = animated  # show animation or not
//  NR, NC = getmaxyx(stdscr)
//  self.end_turn = Button(11, NC, "End turn", align = 'right')
//  self.hero_power_buttons = {}
//  for pl in engine.players:
//pl.viz = VizPlayer(pl)
//pl.hero.viz = VizHero(pl.hero, self.get_hero_pos(pl))
//self.hero_power_buttons[pl] = pl.hero.viz.create_hero_power_button()
//
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
//mvaddstr(i, NC - 11 - len(text), text, black_on_cyan)
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
//
//
//# Overload human interface
//
//
//class HumanPlayerAscii(HumanPlayer) :
//  ''' human player : ask the player what to do'''
//  @staticmethod
//  def mouse_in_win(win, y, x) :
//  wy, wx = getbegyx(win)
//  height, width = getmaxyx(win)
//  return wx <= x<wx + width and wy <= y<wy + height
//
//  def mulligan(self, cards) :
//  engine.board.viz.draw()
//  NR, NC = getmaxyx(stdscr)
//  nc = len(cards)
//  end_button = Button(25, (NC - 6) / 2, '  OK  ')
//  showlist = [(end_button, dict(highlight = black_on_blue))]
//  for i, card in enumerate(cards) :
//    showlist.append((card, dict(pos = (6, int((NC - 6)*(i + 0.5) / nc - 7)),
//    small = False, highlight = black_on_green)))
//
//    discarded = []
//    while True :
//      for card, kwargs in showlist :
//card.draw(**kwargs)
//show_panels()
//ch = getch()
//if ch == KEY_MOUSE :
//mouse_state = getmouse()
//if mouse_state == ERR : continue
//id, x, y, z, bstate = mouse_state
//
//which = None
//for card, kwargs in showlist :
//if self.mouse_in_win(card.win, y, x) :
//which = card, kwargs
//break
//
//if bstate & BUTTON1_PRESSED :
//if not which : continue
//if card is end_button :
//kwargs['bkgd'] = black_on_blue
//else :
//if card in discarded :
//kwargs['bkgd'] = 0
//discarded.remove(card)
//else :
//kwargs['bkgd'] = black_on_red
//discarded.append(card)
//
//elif bstate & BUTTON1_RELEASED :
//if card is end_button :
//break
//else :
//# reset end button
//kwargs = showlist[0][1]
//kwargs['bkgd'] = 0
//
//elif ch in(ord(' '), ord('\n')) :
//break
//
//# clean up
//end_button.delete()
//for card in cards :
//hide_panel(card.panel)
//show_panels()
//
//return discarded
//
//def choose_actions(self, actions) :
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
//
//
//
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
//
//    if __name__ == "__main__":
//args = sys.argv[1:]
//mana = 10 if "mana" in args else 0
//anim = 'anim' in args
//dbg = 'debug' in args
//setup = 'setup' in args
//
//from collection import build_cardbook
//from decks import fake_deck
//cardbook = build_cardbook()
//
//cards = ["Shattered Sun Cleric"]
//deck1 = fake_deck(cardbook, dbg, cards)
//hero1 = Hero(cardbook["Anduin Wrynn"])
//player1 = HumanPlayerAscii(hero1, 'jerome', deck1)
//
//deck2 = fake_deck(cardbook, dbg, cards)
//hero2 = Hero(cardbook["Jaina Proudmoore"])
//if 0:
//player2 = HumanPlayerAscii(hero2, 'mattis', deck2)
//elif 1 :
//from ai import SimpleAI
//player2 = SimpleAI(hero2, 'simpleAI', deck2)
//elif 1 :
//from ai import VerySimpleAI
//player2 = VerySimpleAI(hero2, 'simpleAI', deck2)
//else:
//player2 = RandomPlayer(hero2, 'IA', deck2)
//
//stdscr = init_screen()
//engine = CursesHSEngine(player1, player2)
//engine.board.viz = VizBoard(engine.board, switch = type(player2) == HumanPlayerAscii, animated = anim)
//
//if mana:
//player1.add_mana_crystal(mana)
//player2.add_mana_crystal(mana)
//
//if setup :
//#dbg_add_minion(player1, cardbook["injured blademaster"])
//dbg_add_minion(player2, cardbook["war golem"])
//
//# start playing
//#show_ACS()
//#show_unicode()
//engine.start_game()
//while not engine.is_game_ended() :
//engine.play_turn()
//
//t = engine.turn
//winner = engine.get_winner()
//NC = getmaxyx(stdscr)[1]
//button = Button(10, NC / 2 - 3, '  %s wins after %d turns!  ' % (winner.name, (t + 1) / 2), ty = 5)
//button.draw(highlight = black_on_yellow)
//show_panels()
//getch()
//endwin()
//endwin()
//



#endif
































