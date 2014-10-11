import sys, os, time, pdb
from threading import Thread, Lock
from board import Board, Slot
from players import *
from messages import *
from actions import *
from heroes import *
from cards import Card, Card_Minion, Card_Weapon
import unicurses as uc

import locale
locale.setlocale(locale.LC_ALL, '') #en_US.utf8') #UTF-8')
code = locale.getpreferredencoding()
os.environ['ESCDELAY'] = '25'

def addwch(ch,attr=0,win=None,y=None,x=None,nb=1):
  if win==None: win=stdscr
  # display unicode character
  if code=='UTF-8':
    if x==None:
      uc.waddstr(win,unichr(ch).encode(code)*nb,attr)
    else:
      uc.mvwaddstr(win,y,x,unichr(ch).encode(code)*nb,attr)
  else:
    if x!=None: 
      uc.wmove(win,y,x)
    else:
      y,x = uc.getyx(win)
    uc.whline(win,ch|attr,nb)
    uc.wmove(win,y,x+nb)

def init_screen():
  stdscr = uc.initscr()
  NR,NC = uc.getmaxyx(stdscr)
  if NR<30: 
    uc.endwin()
    print >>sys.stderr, 'error: screen not high enough (min 30 lines)'
    sys.exit()
  if NC<80: 
    uc.endwin()
    print >>sys.stderr, 'error: screen not wide enough (min 80 columns)'
    sys.exit()
  uc.start_color()
  uc.cbreak()
  uc.curs_set(0)
  uc.noecho()
  uc.keypad(stdscr, True)
  uc.mouseinterval(0)
  uc.mousemask(uc.ALL_MOUSE_EVENTS | uc.REPORT_MOUSE_POSITION)
  # init color pairs
  color_pairs = []
  name={getattr(uc,name):name[6:].lower() for name in dir(uc) if name.startswith('COLOR_') and type(getattr(uc,name))==int}
  def create_color_pair( col_fg, col_bg ):
    colname = '%s_on_%s' % (name[col_fg],name[col_bg])
    color_pairs.append((col_fg,col_bg))
    ncol = len(color_pairs)
    #assert ncol<7, 'error: too many color pairs'
    uc.init_pair(ncol, col_fg, col_bg)
    setattr(uc,colname,uc.COLOR_PAIR(ncol))
  create_color_pair(uc.COLOR_CYAN, uc.COLOR_BLACK)
  create_color_pair(uc.COLOR_YELLOW, uc.COLOR_BLACK)
  create_color_pair(uc.COLOR_MAGENTA, uc.COLOR_BLACK)
  create_color_pair(uc.COLOR_BLUE, uc.COLOR_BLACK)
  create_color_pair(uc.COLOR_RED, uc.COLOR_BLACK)
  create_color_pair(uc.COLOR_GREEN, uc.COLOR_BLACK)
  create_color_pair(uc.COLOR_WHITE, uc.COLOR_BLACK)
  create_color_pair(uc.COLOR_WHITE, uc.COLOR_RED)
  create_color_pair(uc.COLOR_WHITE, uc.COLOR_GREEN)
  create_color_pair(uc.COLOR_BLACK, uc.COLOR_WHITE)
  create_color_pair(uc.COLOR_BLACK, uc.COLOR_GREEN)
  create_color_pair(uc.COLOR_BLACK, uc.COLOR_RED)
  create_color_pair(uc.COLOR_BLACK, uc.COLOR_CYAN)
  create_color_pair(uc.COLOR_BLACK, uc.COLOR_BLUE)
  create_color_pair(uc.COLOR_BLACK, uc.COLOR_YELLOW)
  return stdscr

def print_middle(win,y,x,width,text,attr=0):
    if win==None: win = stdscr
    x = max(x, x+(width - len(text)) / 2)
    uc.mvwaddstr(win, y, x, text[:width], attr)

def print_longtext(win,y,x,endy,endx,text,attr=0):
    if win==None: win = stdscr
    text = text.split()
    width = endx-x
    while text and y<endy:
      # pick enough text to fill one line
      line = text.pop(0)
      while text and len(line+text[0])+1<width:
        line += ' '+text.pop(0)
      if len(line)>width:
          text.insert(0,'-'+line[width:])
          line = line[:width]
      print_middle(win,y,x,width,line,attr)
      y += 1

def del_panel(panel):
  win = uc.panel_window(panel)
  uc.del_panel(panel)  
  uc.delwin(win)
  
def top_panel():
  ''' due to a bug in unicurses we recode it here '''
  if os.name=='nt':
    return uc.panel_below(None) # get top panel
  else:
    return uc.curses.panel.top_panel()

def debug():
    uc.endwin()
    pdb.set_trace()
    global stdscr
    stdscr = init_screen()
    uc.touchwin(stdscr)
    show_panels()

def show_ACS():
    NR,NC = uc.getmaxyx(stdscr)
    acs = [ch for ch in dir(uc) if ch.startswith('ACS_')]
    i = 0
    for ch in acs:
      uc.mvhline(i,0,eval('uc.'+ch),NC)
      uc.mvaddstr(i,0,ch)
      i+=1
      if i==NR:
        uc.refresh()
        uc.getch()
        uc.clear()
        i = 0
    uc.refresh()
    uc.getch()
    uc.clear()

def show_unicode():
    uc.clear()
    NR,NC = uc.getmaxyx(stdscr)
    for i in range(1,65536):
      addwch(i)
      if (i+1)%(NR*NC)==0:
        uc.refresh()
        uc.getch()
        uc.clear()
    uc.refresh()
    uc.getch()
    uc.clear()


def rounded_box(win):
    ty,tx = uc.getmaxyx(win)
    uc.box(win)
    addwch(2320,win=win,x=0,y=0)
    addwch(2321,win=win,x=tx-1,y=0)

def strong_box(win):
    if code=='UTF-8':
      h,w = uc.getmaxyx(win)
      w -= 1
      h -= 1
      addwch(9556,win=win,x=0,y=0)
      addwch(9552,win=win,nb=w-1)
      addwch(9559,win=win)
      for i in range(1,h):
        addwch(9553,win=win,x=0,y=i)
        addwch(9553,win=win,x=w,y=i)
      addwch(9562,win=win,x=0,y=h)
      addwch(9552,win=win,nb=w-1)
      addwch(9565,win=win)
    else:
      uc.wborder(win,9553,9553,9552,9552,9556,9559,9562,9565)

def manual_box(win,y,x,h,w):
    w -= 1
    h -= 1
    uc.mvwaddch(win,y,x,uc.ACS_ULCORNER)
    uc.whline(win,uc.ACS_HLINE,w-1)
    uc.mvwaddch(win,y,x+w,uc.ACS_URCORNER)
    uc.mvwvline(win,y+1,x,uc.ACS_VLINE,h-1)
    uc.mvwvline(win,y+1,x+w,uc.ACS_VLINE,h-1)
    uc.mvwaddch(win,y+h,x,uc.ACS_LLCORNER)
    uc.whline(win,uc.ACS_HLINE,w-1)
    uc.mvwaddch(win,y+h,x+w,uc.ACS_LRCORNER)


panel_to_obj = []
def set_panel_userptr(panel, obj):
    uc.set_panel_userptr(panel,len(panel_to_obj))
    panel_to_obj.append(obj)
def get_panel_userptr(panel):
    return panel_to_obj[uc.panel_userptr(panel)]


show_panel_lock = Lock()
def show_panels():
  show_panel_lock.acquire()
  uc.update_panels()
  uc.doupdate()
  show_panel_lock.release()

# Viz classes = copy object specs


### General thing (minion, weapon, hero...)------

class VizThing (object):
  def __init__(self, obj, size, pos ):
    self.obj = obj
    self.hp =     obj.hp
    self.max_hp = obj.max_hp
    self.atq =    obj.atq
    self.max_atq =obj.max_atq
    self.effects = copy(obj.card.effects)
    obj.draw = self.draw
    self.wait = 0
    # create panel
    self.win = uc.newwin(size[0],size[1],pos[0],pos[1])
    self.panel = uc.new_panel(self.win)
    set_panel_userptr(self.panel, obj)

  def check(self):
    obj = self.obj
    assert self.hp == obj.hp, debug()
    assert self.max_hp == obj.max_hp, debug()
    assert self.atq == obj.atq, debug()
    assert self.max_atq == obj.max_atq, debug()

  def delete(self):
    t = 0
    while self.wait and t<5:
      time.sleep(0.1)
      t+=0.1
    self.wait = 0
    if t>=5: debug()
    if hasattr(self,'panel'):
      del_panel(self.panel)
      del self.panel
      del self.win

  def buff_color(self,val,highlight=False,standout=False):
    if type(val)==int:
      if val>0:
        res = uc.white_on_green if highlight else uc.green_on_black
      elif val==0:
        res = uc.white_on_black if highlight else 0
      else:
        res = uc.white_on_red if highlight else uc.red_on_black
    elif type(val)==str:
      if getattr(self,val)<getattr(self,'max_'+val):
        res = uc.white_on_red if highlight else uc.red_on_black
      elif getattr(self,val)>getattr(self.obj.card,val):
        res = uc.white_on_green if highlight else uc.green_on_black
      else:
        res = uc.white_on_black if highlight else 0
    else:
      assert False, debug()
    return res + (uc.A_STANDOUT if standout else 0)

  def draw(self, pos=None, bkgd=0, highlight=0, **kwargs):
    if not hasattr(self,'win'): return None # thing was already deleted
    win, panel = self.win, self.panel
    if pos and pos!=uc.getbegyx(win):
      uc.move_panel(panel,*pos)    
    
    uc.wbkgd(win,bkgd)
    uc.wattron(win,highlight)
    if 'taunt' in self.effects:
      strong_box(win)    
    else:
      uc.box(win)    
    uc.wattroff(win,highlight)
    # show just HP
    ty,tx = uc.getmaxyx(win)
    thp = " %d "%self.hp
    uc.mvwaddstr(win,ty-1,tx-1-len(thp),thp,highlight|self.buff_color('hp'))
    return win

  def update_stats(self, msg, show_hp=True):
    anim = self.obj.engine.board.viz.animated
    for attr in msg.attrs:
      oldval = getattr(self,attr)
      newval = getattr(msg,attr)
      setattr(self,attr,newval)
      if anim and show_hp and msg.attrs==['hp']:
        diff = newval-oldval
        if diff:
          temp_panel(self,"%+d"%diff,self.buff_color(diff,highlight=1),duration=1.5)
    self.draw()


### Hero -----------

class VizHero (VizThing):
  size = (4,13)
  def __init__(self, hero, pos ):
    VizThing.__init__(self,hero,self.size,pos)
    self.armor = hero.armor

  def check(self):
    assert self.armor == obj.armor, debug()

  def draw(self, **kwargs):
    win = VizThing.draw(self,**kwargs)
    if not win: return
    highlight = kwargs.get('highlight',0)
    ty,tx = uc.getmaxyx(win)
    hero = self.obj
    print_middle(win,1,1,tx-2,hero.owner.name)
    print_middle(win,2,1,tx-2,"(%s)"%hero.card.name,uc.blue_on_black)
    if self.armor:
      tar = "[%d]"%self.armor
      uc.mvwaddstr(win,ty-2,tx-1-len(tar),tar)
    pl = hero.owner.viz
    if pl.weapon:
      atq = pl.weapon.atq
      uc.mvwaddstr(win,ty-1,1," %d "%atq,highlight|pl.weapon.viz.buff_color('atq'))

  def create_hero_power_button(self):
    card = self.obj.card
    y,x = uc.getbegyx(self.win)
    up, down = card.power_text.split()
    return HeroPowerButton(y,x+24,up,down,card.power_cost,tx=9,ty=4)
    

### Minion -----------

class VizMinion (VizThing):
  size = 5,11
  def __init__(self, minion):
      pos = minion.engine.board.viz.get_minion_pos(minion)
      VizThing.__init__(self,minion,pos=pos,size=self.size)

  def draw(self, pos=None, **kwargs):
      minion = self.obj
      if pos==None: pos = minion.engine.board.viz.get_minion_pos(minion)
      win = VizThing.draw(self,pos=pos,**kwargs)
      highlight = kwargs.get('highlight',0)
      ty,tx = uc.getmaxyx(win)
      name = minion.card.name_fr or minion.card.name
      print_longtext(win,1,1,ty-1,tx-1,name,uc.yellow_on_black)
      ref_atq = min(self.obj.card.atq, self.max_atq)
      uc.mvwaddstr(win,ty-1,1," %d "%self.atq, highlight|self.buff_color('atq'))
      if 'death_rattle' in self.effects:
        uc.mvwaddstr(win,ty-1,tx/2,'D',highlight)
      if 'silence' in self.effects:
        uc.mvwchgat(win,ty/2,1,tx-2,uc.black_on_red,12)


class VizWeapon (VizThing):
  size = 4,11
  def __init__(self, weapon):
      y,x = uc.getbegyx(weapon.hero.viz.win)
      VizThing.__init__(self,weapon,pos=(y,x-18),size=self.size)

  def draw(self, **kwargs):
      weapon = self.obj
      win = VizThing.draw(self,**kwargs)
      highlight = kwargs.get('highlight',0)
      ty,tx = uc.getmaxyx(win)
      name = weapon.card.name_fr or weapon.card.name
      print_longtext(win,1,1,ty-1,tx-1,name,uc.green_on_black)
      ref_atq = min(self.obj.card.atq, self.max_atq)
      uc.mvwaddstr(win,ty-1,1," %d "%self.atq, highlight|self.buff_color('atq'))
  
  def update_stats(self, msg):
    VizThing.update_stats(self, msg, show_hp=False)


### Player -----------

class VizPlayer:
  def __init__(self, player ):
    self.player = player
    self.cards = []
    self.minions = []
    self.weapon = None
    self.secrets = []
    
  def check(self):
    pl = self.player
    for card in self.cards:
      assert card in pl.cards, debug()
    if pl.weapon or self.weapon: 
      self.weapon.viz.check()
    assert len(self.secrets)==len(pl.secrets), debug()
    assert len(self.minions)==len(pl.minions), debug()
    for i,m in enumerate(self.minions):
      assert m is pl.minions[i], debug()
      m.viz.check()

  def set_weapon(self, weapon):
    self.unset_weapon(self.weapon)
    weapon.viz = VizWeapon(weapon)
    self.weapon = weapon

  def unset_weapon(self, weapon):
    assert weapon is self.weapon
    if self.weapon:
      self.weapon.viz.delete()
      self.weapon = None


### Button -----------

class Button:
  def __init__(self, y,x, text, align='center', tx=0,ty=3):
      self.text = text
      if tx==0: tx = len(text)+2
      if align=='left': pass
      elif align=='center': x -= tx/2
      elif align=='right': x -= tx
      win = self.win = uc.newwin(ty,tx,y,x)
      self.panel = uc.new_panel(win)
      set_panel_userptr(self.panel, self)

  def delete(self):
      del_panel(self.panel)
      del self.panel
      del self.win

  def draw(self, highlight=0, bkgd=0, box=True, ytext=None, coltext=0, **kwargs):
      uc.wbkgd(self.win,bkgd)
      if box:
        uc.wattron(self.win,highlight)
        uc.box(self.win)
        uc.wattroff(self.win,highlight)
      ty, tx = uc.getmaxyx(self.win)
      print_middle(self.win,ytext or ty/2,1,tx-2,self.text,coltext)

class HeroPowerButton (Button):
  def __init__(self, y,x, text, subtext, cost=2, **kwargs):
      Button.__init__(self,y,x,text,**kwargs)
      self.subtext = subtext
      self.cost = cost

  def draw(self,**kwargs):
      coltext = uc.yellow_on_black
      Button.draw(self,ytext=1,coltext=coltext,**kwargs)
      ty, tx = uc.getmaxyx(self.win)
      uc.mvwaddstr(self.win,0,tx/2-1,"(%d)"%self.cost,uc.cyan_on_black)
      print_longtext(self.win,2,1,ty-1,tx-1,self.subtext,coltext)
      

def temp_panel(viz,text,color,duration=2):
    assert issubclass(type(viz),VizThing), debug()
    viz.wait += 1
    y,x = uc.getbegyx(viz.win)
    ty,tx = uc.getmaxyx(viz.win)
    button = Button(y+ty/2-1,x+tx/2,text)
    button.draw(box=False,bkgd=color)
    def wait_delete(duration,button,viz):
      t=0
      while t<duration:
        show_panel_lock.acquire()
        uc.touchwin(button.win)
        uc.top_panel(button.panel)  # remains at top
        show_panel_lock.release()
        show_panels()
        time.sleep(0.1)
        t+=0.1
      show_panel_lock.acquire()
      button.delete()
      show_panel_lock.release()
      show_panels()
      viz.wait -= 1
    Thread(target=wait_delete,args=(duration,button,viz)).start()
    
    

### Slot -----------

def get_screen_pos(self):
    """ return position, space """
    NR,NC = uc.getmaxyx(stdscr)
    n = len(self.owner.viz.minions)
    top,bot = self.engine.board.viz.get_top_bottom_players()
    y = 6 if self.owner is top else 14
    sp = ([3]*5+[2,1,0])[n] # spacement between minions
    return (y,int(NC-3-(11+sp)*n)/2+(11+sp)*self.index), sp

Slot.get_screen_pos = get_screen_pos

def draw_Slot(self, highlight=0, bkgd=0, **kwargs):
    if not hasattr(self,"win"):
      (y,x), sp = self.get_screen_pos()
      win = self.win = uc.newwin(5,sp,y,x-sp)
      self.panel = uc.new_panel(win)
      set_panel_userptr(self.panel, self)
    
    if bkgd or highlight:
      uc.top_panel(self.panel)
      uc.wbkgd(self.win,bkgd or highlight)
    else:
      del_panel(self.panel)
      del self.panel
      del self.win



### Card -----------
card_size = (14,15)

def draw_Card(self, pos=None, highlight=0, cost=None, small=True, bkgd=0, **kwargs):
    name = self.name_fr or self.name
    desc = self.desc_fr or self.desc

    if not small:
      ty,tx = card_size
      if hasattr(self,"small_panel"):
        y,x = uc.getbegyx(self.small_win)
        if not pos:
          self.ty = ty
          self.tx = tx
          NR,NC = uc.getmaxyx(stdscr)
          pos = NR-ty, x
        if pos[0]>y:  pos = y,pos[1]  # cannot be below small panel
      if not hasattr(self,"win"):
        self.win = uc.newwin(ty,tx,pos[0],pos[1])
        self.panel = uc.new_panel(self.win)
        set_panel_userptr(self.panel, self)
      win, panel = self.win, self.panel
      uc.top_panel(panel)
    else: # small card version
      small = min(small,card_size[0])
      if hasattr(self,"panel"):
        uc.hide_panel(self.panel)
      if not hasattr(self,"small_win"):
        ty, tx = small, card_size[1]
        self.small_win = uc.newwin(ty,tx,pos[0],pos[1])
        self.small_panel = uc.new_panel(self.small_win)
        set_panel_userptr(self.small_panel, self)
      win, panel = self.small_win, self.small_panel
      ty, tx = uc.getmaxyx(win)
      if type(small)==int and small!=ty: # redo
        del_panel(panel)
        del self.small_win
        del self.small_panel
        return self.draw(pos=pos, highlight=highlight, cost=cost, small=small, bkgd=bkgd)
      small = uc.getmaxyx(win)[0]
    
    ty, tx = uc.getmaxyx(win)
    if pos and pos!=uc.getbegyx(win):
      uc.move_panel(panel,*pos)

    uc.wbkgd(win,bkgd)
    if highlight:  uc.wattron(win,highlight)
    uc.box(win)
    if 0<small<card_size[0]:
      uc.mvwaddch(win,ty-1,0,uc.ACS_VLINE)
      uc.mvwaddch(win,ty-1,tx-1,uc.ACS_VLINE)
    if highlight:  uc.wattroff(win,highlight)
    if 0<small<card_size[0]:
      uc.mvwaddstr(win,ty-1,1,' '*(tx-2))

    if issubclass(type(self),Card_Minion):
      mid = card_size[0]/2
      y,x,h,w = 1,2,mid-2,tx-4
      manual_box(win,y,x,h,w)
      print_longtext(win,y+1,x+1,y+h-1,x+w-1,name,uc.yellow_on_black)
      uc.mvwaddstr(win,y+h-1,x+1,"%2d "%self.atq)
      uc.mvwaddstr(win,y+h-1,x+w-4,"%2d "%self.hp)
      print_longtext(win,mid,2,ty-1,tx-2,desc)
    else:
      r = 4 if issubclass(type(self),Card_Weapon) else 3
      uc.mvwaddch(win, r, 0, uc.ACS_LTEE, highlight)
      uc.mvwhline(win, r, 1, uc.ACS_HLINE, tx-2)
      uc.mvwaddch(win, r, tx-1, uc.ACS_RTEE, highlight)
      if issubclass(type(self),Card_Weapon):
        name_color = uc.green_on_black
        uc.mvwaddstr(win,r,2, " %d "%self.atq)
        hpt = " %d "%self.hp
        uc.mvwaddstr(win,r,tx-2-len(hpt),hpt)
      else:
        name_color = uc.magenta_on_black
      print_longtext(win,1,1,r,tx-1,name,name_color)
      print_longtext(win,r+1,2,ty,tx-2,desc)

    # print cost
    if cost==None:
      cost=self.cost
    if cost==self.cost:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.black_on_cyan)
    elif cost<self.cost:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.white_on_green)
    else:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.white_on_red)


def card_delete(self):
    if hasattr(self,'panel'):
      del_panel(self.panel)
      del self.win
      del self.panel
    if hasattr(self,'small_panel'):
      del_panel(self.small_panel)
      del self.small_win
      del self.small_panel

Card.delete = card_delete

# Messages ---------

def interp(i,max,start,end):
  """ func to interpolate, i varies in [0,m-1] """
  assert 0<=i<max, debug()
  return start + (end-start)*i/(max-1)

def draw_Message(self):
    pass

def draw_Action(self):
    pass

def draw_Msg_StartTurn(self):
    player = self.caster
    player.viz.check()  # check consistency with real data
    NC = uc.getmaxyx(stdscr)[1]
    button = Button(10,NC/2-3," %s's turn! "%player.name,tx=20,ty=5)
    button.draw(highlight=uc.black_on_yellow)
    show_panels()
    time.sleep(1 if self.engine.board.viz.animated else 0.1)
    button.delete()
    self.engine.board.draw()

def draw_Msg_DrawCard(self):
    card = self.card
    self.caster.viz.cards.append(card)
    bottom_player = self.engine.board.viz.get_top_bottom_players()[1]
    if self.engine.board.viz.animated and bottom_player==self.caster:
      self.engine.board.draw('cards',which=self.caster,last_card=False)
      NR,NC = uc.getmaxyx(stdscr)
      ty,tx = card_size
      sy,sx = 12, NC-tx
      ey,ex = self.engine.board.viz.get_card_pos(card)
      for y in range(sy,ey+1):
        x = int(0.5+sx+(ex-sx)*(y-sy)/float(ey-sy))
        h = max(0,NR-y)
        card.draw(highlight=uc.black_on_yellow,pos=(y,x),small=0 if h>=ty else h)
        show_panels()
        time.sleep(0.05 + 0.6*(y==sy))
    self.engine.board.draw('cards',which=self.caster)

def draw_Msg_EndTurn(self):
    pass

def draw_Msg_UseMana(self):
    self.engine.board.draw('mana',self.caster)

def draw_Msg_ThrowCard(self):
    card = self.card
    card.owner.viz.cards.remove(card)
    card.delete()
    self.engine.board.draw('cards',which=self.caster)

def draw_Msg_PlayCard(self):
    top, bot = self.engine.board.viz.get_top_bottom_players()    
    if self.caster is top:
      sx = (uc.getmaxyx(stdscr)[1]-card_size[1])/2
      kwargs = dict(small=False,cost=self.cost)
      self.card.draw(pos=(0,sx),highlight=uc.black_on_yellow,**kwargs)
      show_panels()
      time.sleep(1)
      if self.engine.board.viz.animated:
        for i in range(sx-1,-1,-2):
          self.card.draw(pos=(0,i),**kwargs)
          show_panels()
          time.sleep(0.05*(i)/sx)
        time.sleep(0.2)
      self.card.delete()
      show_panels()

def draw_Msg_MinionPopup(self):
    new_minion = self.caster
    owner = new_minion.owner
    if self.engine.board.viz.animated:
      old_pos = {}
      for i,m in enumerate(owner.viz.minions):
        old_pos[m] = Slot(owner,i).get_screen_pos()[0]
    owner.viz.minions.insert(self.pos,new_minion)
    new_minion.viz = VizMinion(new_minion)
    if self.engine.board.viz.animated and old_pos:
      new_pos = {}
      for i,m in enumerate(owner.viz.minions):
        new_pos[m] = Slot(owner,i).get_screen_pos()[0]
      r = VizMinion.size[1]/2+1
      uc.hide_panel(new_minion.viz.panel)
      for i in range(1,r):
        for m, (oy,ox) in old_pos.items():
          ny,nx = new_pos[m]
          m.draw(pos=(interp(i,r,oy,ny),interp(i,r,ox,nx)))
        show_panels()
        time.sleep(0.1)
      uc.show_panel(new_minion.viz.panel)
    self.engine.board.draw('minions',which=owner)

def draw_Msg_WeaponPopup(self):
    weapon = self.caster
    weapon.owner.viz.set_weapon(weapon)
    self.engine.board.draw('heroes',which=weapon.owner)

def draw_Msg_SecretPopup(self):
    secret = self.caster
    weapon.owner.viz.secrets.append(secret)
    self.engine.board.draw('secrets',which=secret.owner)

def draw_Msg_DeadMinion(self):
    dead_minion = self.caster
    dead_minion.viz.delete()
    if self.engine.board.viz.animated:
      pl = dead_minion.owner
      old_pos = {}
      for i,m in enumerate(pl.viz.minions):
        old_pos[m] = Slot(pl,i).get_screen_pos()[0]
    dead_minion.owner.viz.minions.remove(dead_minion)
    if self.engine.board.viz.animated:
      new_pos = {}
      for i,m in enumerate(pl.viz.minions):
        new_pos[m] = Slot(pl,i).get_screen_pos()[0]
      r = VizMinion.size[1]/2+1
      for i in range(1,r):
        for m, (ny,nx) in new_pos.items():
          oy,ox = old_pos[m]
          m.draw(pos=(interp(i,r,oy,ny),interp(i,r,ox,nx)))
        show_panels()
        time.sleep(0.1)
    self.engine.board.draw('minions',which=dead_minion.owner)

def draw_Msg_DeadWeapon(self):
    dead_weapon = self.caster
    dead_weapon.owner.viz.unset_weapon(dead_weapon)
    dead_weapon.viz.delete()
    self.engine.board.draw('heroes',which=dead_weapon.owner)

def draw_Msg_Status(self):
    self.caster.viz.update_stats(self)
    show_panels()

def draw_Msg_StartAttack(self):
    if self.engine.board.viz.animated:
      if issubclass(type(self.caster),Weapon):
        caster = self.caster.hero
      else:
        caster = self.caster
      oy,ox =   uc.getbegyx(caster.viz.win)
      oty,otx = uc.getmaxyx(caster.viz.win)
      uc.top_panel(caster.viz.panel) # set assailant as top panel
      ny,nx =   uc.getbegyx(self.target.viz.win)
      nty,ntx = uc.getmaxyx(self.target.viz.win)
      nx += (ntx-otx)/2
      m = abs(oy-ny)
      t = 0.5/(m+2)
      for i in range(1,m-(nty+1)/2)+range(m-(nty+1)/2,-1,-1):
        caster.draw(pos=(interp(i,m,oy,ny),interp(i,m,ox,nx)))
        show_panels()
        time.sleep(t)
  

### Board --------

class VizBoard:
  def __init__(self, board, switch=False, animated=True):
      self.board = board
      self.engine = board.engine
      board.draw = self.draw
      self.switch = switch # switch heroes or not
      self.animated = animated  # show animation or not
      NR,NC = uc.getmaxyx(stdscr)
      self.end_turn = Button(11,NC,"End turn",align='right')
      self.hero_power_buttons = {}
      for pl in engine.players:
        pl.viz = VizPlayer(pl)
        pl.hero.viz = VizHero(pl.hero, self.get_hero_pos(pl))
        self.hero_power_buttons[pl] = pl.hero.viz.create_hero_power_button()

  def get_top_bottom_players(self):
      player = self.engine.get_current_player()
      adv = self.engine.get_other_player()
      if self.switch==False and self.engine.turn%2: 
        player,adv = adv,player # prevent top/down switching
      return adv, player

  def get_minion_pos(self, minion):
      try:
        index = minion.owner.viz.minions.index(minion)
      except ValueError:
        return None
      slot = Slot(minion.owner, index, None)
      return slot.get_screen_pos()[0]

  def get_card_pos(self,card):
      NR,NC = uc.getmaxyx(stdscr)
      nc = len(card.owner.viz.cards)+1e-8
      ty,tx = card_size
      actual_NC = min(NC,tx*nc)
      startx = int((NC - actual_NC)/2)
      i = card.owner.viz.cards.index(card)
      return 24, startx + int((actual_NC-tx)*i/(nc-1))

  def get_hero_pos(self,player):
      NR,NC = uc.getmaxyx(stdscr)
      top,bot = self.get_top_bottom_players()
      if player==top: return (1,(NC-18)/2)
      if player==bot: return (20,(NC-18)/2)
      assert False

  def draw(self, what='bkgd decks hero cards mana minions', which=None, last_card=True):
      NR,NC = uc.getmaxyx(stdscr)
      
      # clear screen
      adv, player = self.get_top_bottom_players()
      which = {player,adv} if not which else {which}
      assert all([issubclass(type(pl),Player) for pl in which]), debug()
      
      if 'bkgd' in what:
        # background
        uc.erase()
        uc.mvhline(2,0,uc.ACS_CKBOARD,NC)
        uc.mvhline(3,0,uc.ACS_CKBOARD,NC)
        uc.mvhline(12,0,ord('-'),NC)
        uc.mvhline(21,0,uc.ACS_CKBOARD,NC)
        uc.mvhline(22,0,uc.ACS_CKBOARD,NC)
        self.end_turn.draw()
        # draw hero power
        for pl in which:
          self.hero_power_buttons[pl].draw()
      
      # draw decks on the right side
      if 'decks' in what:
        for i in range(3):
          uc.mvvline(2,NC-1-i,uc.ACS_CKBOARD,20)
        for i in [5,15]:
          uc.mvaddch(i,NC-2,uc.ACS_HLINE)
          addwch(9558)
          text = ' %2d'%len(i<12 and adv.deck or player.deck)
          for j,ch in enumerate(text):
            uc.mvaddch(i+1+j,NC-2,ord(ch))
            addwch(9553)
          uc.mvaddch(i+4,NC-2,uc.ACS_HLINE)
          addwch(9564)
      
      # draw heroes
      if 'hero' in what:
        for pl in which:
          pl.hero.draw()
          if pl.viz.weapon:
            pl.viz.weapon.draw()

      # draw cards
      if 'cards' in what:
        if adv in which:
          print_middle(stdscr, 0,0, NC, " Adversary has %d cards. "%len(adv.cards))
        if player in which:
          for card in player.viz.cards[:None if last_card else -1]:
            card.draw(pos=self.get_card_pos(card), small=NR-24)
      
      # draw mana
      if 'mana' in what:
        for who,i in [(adv,2),(player,22)]:
          if who not in which:  continue
          p = i<12 and adv or player
          text = "%2d/%d "%(p.mana,p.max_mana)
          uc.mvaddstr(i,NC-11-len(text), text, uc.black_on_cyan)
          addwch(9830, uc.cyan_on_black, nb=p.mana)
          addwch(9826, uc.cyan_on_black, nb=p.max_mana-p.mana)
      
      # draw minions
      if 'minions' in what:
        if player in which:
          for m in player.viz.minions:
            m.draw(y=14)
        if adv in which:
          for m in adv.viz.minions:
            m.draw(y=5)
      
      show_panels()


# attach each show function to a message
all_globs = globals().keys()
draw_funcs = [key for key in all_globs if key.startswith("draw_")]
for key in draw_funcs:
    if key[5:] in all_globs:
      setattr(globals()[key[5:]], "draw", globals()[key])


# Overload human interface


class HumanPlayerAscii (HumanPlayer):
  ''' human player : ask the player what to do'''
  @staticmethod
  def mouse_in_win(win,y,x):
    wy,wx = uc.getbegyx(win)
    height, width = uc.getmaxyx(win)
    return wx<=x<wx+width and wy<=y<wy+height

  def mulligan(self, cards):
      engine.board.viz.draw()
      NR,NC = uc.getmaxyx(stdscr)
      nc = len(cards)
      end_button = Button(25,(NC-6)/2,'  OK  ')
      showlist = [(end_button,dict(highlight=uc.black_on_blue))]
      for i,card in enumerate(cards):
          showlist.append((card,dict(pos=(6,int((NC-6)*(i+0.5)/nc-7)), small=False, highlight=uc.black_on_green)))
      
      discarded = []
      while True:
        for card,kwargs in showlist:
          card.draw(**kwargs)
        show_panels()
        ch = uc.getch()
        if ch == uc.KEY_MOUSE:
          mouse_state = uc.getmouse()
          if mouse_state==uc.ERR: continue
          id, x, y, z, bstate = mouse_state
          
          which = None
          for card,kwargs in showlist:
            if self.mouse_in_win(card.win,y,x):
              which = card,kwargs
              break
          
          if bstate & uc.BUTTON1_PRESSED: 
            if not which: continue
            if card is end_button:
              kwargs['bkgd'] = uc.black_on_blue
            else:
              if card in discarded:
                kwargs['bkgd'] = 0
                discarded.remove(card)
              else:
                kwargs['bkgd'] = uc.black_on_red
                discarded.append(card)
          
          elif bstate & uc.BUTTON1_RELEASED: 
            if card is end_button:
              break
            else:
              # reset end button
              kwargs = showlist[0][1]
              kwargs['bkgd'] = 0
        
        elif ch in (ord(' '), ord('\n')):
          break
      
      # clean up
      end_button.delete()
      for card in cards:
        uc.hide_panel(card.panel)
      show_panels()
      
      return discarded

  def choose_actions(self, actions):
      # split actions
      showlist = [] # [(action, object, draw_kwargs)]
      remaining_cards = set(self.cards)
      for a in actions:
        if issubclass(type(a),Act_HeroPower):
          showlist.append((a,self.engine.board.viz.hero_power_buttons[a.caster],{}))
        elif issubclass(type(a),Act_PlayCard):
          showlist.append((a,a.card,{'small':True}))
          if a.card in remaining_cards: # choice_of_cards
            remaining_cards.remove(a.card)
        elif issubclass(type(a),Act_MinionAttack):
          showlist.append((a,a.caster,{}))
        elif issubclass(type(a),Act_WeaponAttack):
          showlist.append((a,a.caster.hero,{}))
        else:
          end_turn_action = a
          showlist.append((a,self.engine.board.viz.end_turn,{}))
      # we can also inspect non-playable cards
      for card in remaining_cards:
        showlist.append((None,card,{'small':True}))
      
      def erase_elems(showlist):
        for a,obj,kwargs in showlist:
          obj.draw(small=True)  # erase everything
      
      init_showlist = showlist
      self.engine.board.draw()
      last_sel = None
      active = None
      while True:
        mapping = {}  # obj -> action,draw_kwargs
        for a,obj,kwargs in showlist:
          highlight = uc.black_on_green if a else 0
          assert hasattr(obj,'draw'), debug()
          obj.draw(highlight=highlight,**kwargs)
          mapping[obj] = (a,kwargs)
        if active:
          active.draw(bkgd=uc.black_on_white)
        show_panels()
        
        ch = uc.getch()
        
        if ch==uc.KEY_MOUSE:
          mouse_state = uc.getmouse()
          if mouse_state==uc.ERR: continue
          id, x, y, z, bstate = mouse_state
          #uc.mvaddstr(5,0,"mouse %d %d %s"%(y,x,bin(bstate)))

          sel = None
          cur = top_panel()  
          while cur:
            obj = get_panel_userptr(cur)
            if issubclass(type(obj),Card) or obj in mapping:
              if self.mouse_in_win(uc.panel_window(cur),y,x):
                sel = obj
                break
            cur = uc.panel_below(cur)
          
          # reset everybody 
          for a,kwargs in mapping.values():
            kwargs['bkgd'] = 0
            kwargs['small'] = True
          
          if bstate & uc.BUTTON1_PRESSED:
            # set current all green
            if sel in mapping:
              a, kwargs = mapping[sel]
              kwargs['bkgd'] = uc.black_on_green if a else 0
              kwargs['small'] = False
            last_sel = sel
          
          elif bstate & uc.BUTTON1_RELEASED:
            if sel!=last_sel: continue 
            if sel not in mapping: continue
            act,kwargs = mapping[sel]
            if not act: continue
            
            if issubclass(type(act), Action):
              action = act
              active = sel
              choices = []  # reset choices
            else:
              choices.append(act)
            erase_elems(showlist)
            while True:
              if len(choices)>=len(action.choices):
                return action.select(choices)
              elif not action.choices[len(choices)]:  # no choices!
                choices.append(None)
              else:
                # propose new choices
                showlist = [(obj,obj,{}) for obj in action.choices[len(choices)]]
                break
              
          else:
            erase_elems(showlist)
            showlist = init_showlist
            active = None
        
        elif ch == ord('\n'):
          if active==None:
            return end_turn_action.select([])
        
        elif ch == 27: # escape
          erase_elems(showlist)
          showlist = init_showlist
          active = None
        elif ch in (uc.CCHAR('d'), uc.CCHAR('p')):
          debug()
        else:
          uc.endwin()
          print self.engine.log
          sys.exit()



# overloaded HS engine
from hs_engine import HSEngine

class CursesHSEngine (HSEngine):
  ''' overload display functions '''
  def __init__(self, *args ):
    HSEngine.__init__(self, *args)
    self.display = []
    self.log = ''
    self.logfile = open('log.txt','w')

  def display_msg(self, msg):
    self.display.append(msg)

  def wait_for_display(self):
    while self.display:
      msg = self.display.pop(0)
      line = "[%s] %s\n" %(type(msg).__name__,msg)
      self.logfile.write(line)
      self.log += line
      msg.draw()




if __name__=="__main__":
    args = sys.argv[1:]
    mana = 10 if "mana" in args else 0
    anim = 'anim' in args
    from cards import fake_deck

    deck1 = fake_deck()
    hero1 = Hero(Card_Mage())
    player1 = HumanPlayerAscii(hero1, 'jerome', deck1)

    deck2 = fake_deck()
    hero2 = Hero(Card_Priest())
    player2 = RandomPlayer(hero2, 'IA', deck2)

    stdscr = init_screen()
    engine = CursesHSEngine( player1, player2 )
    engine.board.viz = VizBoard(engine.board, switch=False, animated=anim)

    if mana:
      engine.players[0].add_mana_crystal(mana)
      engine.players[1].add_mana_crystal(mana)  
    
    # start playing
    #show_ACS()
    #show_unicode()
    engine.start_game()
    while not engine.is_game_ended():
      engine.play_turn()
    
    t = engine.turn
    winner = engine.get_winner()
    NC = uc.getmaxyx(stdscr)[1]
    button = Button(10,NC/2-3,'  %s wins after %d turns!  ' % (winner.name, (t+1)/2),ty=5)
    button.draw(highlight=uc.black_on_yellow)
    show_panels()
    uc.getch()    
    uc.endwin()





































