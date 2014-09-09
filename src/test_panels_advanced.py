from unicurses import *

def win_show(win, label, label_color):
    starty, startx = getbegyx(win)
    height, width = getmaxyx(win)
    box(win, 0, 0)
    mvwaddch(win, 2, 0, ACS_LTEE)
    mvwhline(win, 2, 1, ACS_HLINE, width - 2)
    mvwaddch(win, 2, width - 1, ACS_RTEE)
    print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color))
    print_in_middle(win, 5, 0, width, "this text will be remembered", COLOR_PAIR(label_color))

def print_in_middle(win, starty, startx, width, string, color):
    if (win == None): win = stdscr
    y, x = getyx(win)
    if (startx != 0): x = startx
    if (starty != 0): y = starty
    if (width == 0): width = 80
    length = len(string)
    temp = (width - length) / 2
    x = startx + int(temp)
    wattron(win, color)
    mvwaddstr(win, y, x, string)
    wattroff(win, color)

def init_wins(wins, n):
    y = 2
    x = 10
    for i in range(0, n):
        wins[i] = newwin(10, 40, y, x)
        label = str.format("Window number {0}", i + 1)
        win_show(wins[i], label, i + 1)
        y += 3
        x += 7

def debug():
  endwin()
  import pdb
  pdb.set_trace()


NLINES = 10
NCOLS = 40
my_wins = [0] * 3
my_panels = [0] * 3

stdscr = initscr()
start_color()
cbreak()
curs_set(0)
noecho()
keypad(stdscr, True)
mouseinterval(0)
mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION)

init_pair(1, COLOR_RED, COLOR_BLACK)
init_pair(2, COLOR_GREEN, COLOR_BLACK)
init_pair(3, COLOR_BLUE, COLOR_BLACK)
init_pair(4, COLOR_CYAN, COLOR_BLACK)

mvaddstr(15, 15, "---------- This text is background text ---------")
init_wins(my_wins, 3)

my_panels[0] = new_panel(my_wins[0])
my_panels[1] = new_panel(my_wins[1])
my_panels[2] = new_panel(my_wins[2])

set_panel_userptr(my_panels[0], 0)
set_panel_userptr(my_panels[1], 1)
set_panel_userptr(my_panels[2], 2)

update_panels()

attron(COLOR_PAIR(4))
mvaddstr(0, int(NCOLS / 2) - 2, "Use tab to browse through the windows (Q to Exit)")
attroff(COLOR_PAIR(4))
noutrefresh(stdscr)

doupdate()

top = my_panels[2]
def next(n):
  global top
  num = panel_userptr(top)
  top = my_panels[(num+n)%3]
  top_panel(top)

BUTTON5_PRESSED = (1<<21) | REPORT_MOUSE_POSITION

ch = -1
while ( (ch != CCHAR('q')) and (ch != CCHAR('Q')) ):
    mvaddstr(22,0,"current panel = %d" % (1+panel_userptr(top)))
    update_panels()
    doupdate()
    ch = getch()
    move(19,0);  clrtoeol()
    move(20,0);  clrtoeol()
    move(21,0);  clrtoeol()
    
    if ch == ord('\t'):
        next(1)
        mvaddstr(16,20,"top panel = %d" % (1+panel_userptr(top)))
    
    elif ch == KEY_MOUSE:
        mouse_state = getmouse()
        if mouse_state==ERR: continue
        id, x, y, z, bstate = mouse_state
        mvaddstr(19,0,"mouse position = %d,%d  state={%s}" % (x,y,''.join([(bstate&(1<<i)) and '%d,'%i or '' for i in range(32)])))
        
        if bstate & BUTTON5_PRESSED:
          next(1)
          continue
        elif bstate & BUTTON4_PRESSED:
          next(-1)
          continue
        
        cur = top #panel_below(None)
        n = len("panel stack = ")
        mvaddstr(21,0,"panel stack = ")
        while cur:
          mvaddstr(21,n,"%1d," % (1+panel_userptr(cur)))
          n+=3
          win = panel_window(cur)
          wy,wx = getbegyx(win)
          height, width = getmaxyx(win)
          if wx<=x<wx+width and wy<=y<wy+height:
            break
          cur = panel_below(cur)
        if not cur: continue
        
        if bstate & BUTTON1_PRESSED:
          if top!=cur:
            top_panel(cur)
            top=cur
          else:
            if panel_hidden(cur):
              show_panel(cur)
              top=cur
            elif panel_below(cur):
              top = panel_below(cur)
              hide_panel(cur)

endwin()


























