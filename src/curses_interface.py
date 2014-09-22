import sys, os, time, pdb
from board import Board, Slot
from players import *
from messages import *
from actions import *
from heroes import *
from cards import Card
import unicurses as uc

import locale
locale.setlocale(locale.LC_ALL, '')
code = locale.getpreferredencoding()
os.environ['ESCDELAY'] = '25'

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
  create_color_pair(uc.COLOR_WHITE, uc.COLOR_RED)
  create_color_pair(uc.COLOR_WHITE, uc.COLOR_GREEN)
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
      print_middle(win,y,x,width,line,attr)
      y += 1

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
      #uc.addch(i)
      #for byte in unichr(i).encode(code):
      #  uc.addch(byte)
      uc.addstr(unichr(i).encode(code))
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
    uc.mvwaddch(win,0,0,ord(u'\u2320'))
    uc.mvwaddch(win,ty-1,tx-1,ord(u'\u2321'))

def strong_box(win):
    ty,tx = uc.getmaxyx(win)
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

### Hero -----------

def draw_Hero(self, pos=None, bkgd=0, highlight=0, **kwargs):
    #if not hasattr()
    NR,NC = uc.getmaxyx(stdscr)
    ty,tx = 4,12
    if not hasattr(self,"win"):
      self.win = uc.newwin(ty,tx,pos[0],pos[1])
      self.panel = uc.new_panel(self.win)
      set_panel_userptr(self.panel, self)
    win, panel = self.win, self.panel
    
    if pos and pos!=uc.getbegyx(win):
      uc.move_panel(panel,*pos)    
    
    uc.wbkgd(win,bkgd)
    uc.wattron(win,highlight)
    uc.box(win)
    uc.wattroff(win,highlight)
    print_middle(win,1,1,tx-2,self.owner.name)
    print_middle(win,2,1,tx-2,"(%s)"%self.card.name,uc.magenta_on_black)
    uc.mvwaddstr(win,ty-1,tx-3,"%d"%self.hp)
    uc.touchwin(win)

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
      uc.del_panel(self.panel)
      self.panel = self.win = None

  def draw(self, highlight=0, bkgd=0, **kwargs):
      uc.wbkgd(self.win,bkgd)
      
      uc.wattron(self.win,highlight)
      uc.box(self.win)
      uc.wattroff(self.win,highlight)
      
      ty, tx = uc.getmaxyx(self.win)
      print_middle(self.win,ty/2,1,tx-2,self.text)

### Slot -----------

def get_screen_pos(self):
    """ return position, space """
    NR,NC = uc.getmaxyx(stdscr)
    n = len(self.owner.minions)
    sp = ([3]*5+[2,1,0])[n] # spacement between minions
    return int(NC-3-(11+sp)*n)/2+(11+sp)*self.pos, sp

Slot.get_screen_pos = get_screen_pos

def draw_Slot(self, highlight=0, bkgd=0, **kwargs):
    if not hasattr(self,"win"):
      x, sp = self.get_screen_pos()
      win = self.win = uc.newwin(5,sp,14,x-sp)
      self.panel = uc.new_panel(win)
      set_panel_userptr(self.panel, self)
    
    if bkgd or highlight:
      uc.top_panel(self.panel)
      uc.wbkgd(self.win,bkgd or highlight)
    else:
      uc.del_panel(self.panel)
      del self.panel
      del self.win

### Minion -----------

def draw_Minion(self, highlight=0, bkgd=0, y=-1, **kwargs):
    slot = self.engine.board.get_minion_pos(self)
    x = slot.get_screen_pos()[0]
    
    if not hasattr(self,"win"):
      assert y>=0
      win = self.win = uc.newwin(5,11,y,x)
      self.panel = uc.new_panel(win)
      set_panel_userptr(self.panel, self)
    elif y>=0:
      uc.move_panel(self.panel,y,x)
    
    win = self.win
    ty, tx = uc.getmaxyx(win)
    uc.wbkgd(win,bkgd)
    
    name = self.card.name_fr or self.card.name
    
    if highlight:  uc.wattron(win,highlight)
    uc.box(win)
    print_longtext(win,1,1,ty-1,tx-1,name,uc.magenta_on_black)
    uc.mvwaddstr(win,ty-1,1,"%2d "%self.atq)
    uc.mvwaddstr(win,ty-1,tx-4,"%2d "%self.hp)
    if highlight:  uc.wattroff(win,highlight)

### Card -----------
card_size = 14,15

def draw_Card(self, pos=None, highlight=0, cost=None, small=True, bkgd=0, **kwargs):
    name = self.name_fr or self.name
    desc = self.desc_fr or self.desc

    if not small:
      ty,tx = card_size
      if not pos and hasattr(self,"small_panel"):
        self.ty = ty
        self.tx = tx
        y,x = uc.getbegyx(self.small_win)
        NR,NC = uc.getmaxyx(stdscr)
        pos = NR-ty, x
      if not hasattr(self,"win"):
        self.win = uc.newwin(ty,tx,pos[0],pos[1])
        self.panel = uc.new_panel(self.win)
        set_panel_userptr(self.panel, self)
      win, panel = self.win, self.panel
      uc.top_panel(panel)
    else: # small card version
      if hasattr(self,"panel"):
        uc.hide_panel(self.panel)
      if not hasattr(self,"small_win"):
        ty, tx = small, card_size[1]
        if pos==None: debug()
        self.small_win = uc.newwin(ty,tx,pos[0],pos[1])
        self.small_panel = uc.new_panel(self.small_win)
        set_panel_userptr(self.small_panel, self)
      win, panel = self.small_win, self.small_panel
      ty, tx = uc.getmaxyx(win)
      if type(small)==int and small!=ty: # redo
        uc.del_panel(panel)
        del self.small_win
        del self.small_panel
        return self.draw(pos=pos, highlight=highlight, cost=cost, small=small, bkgd=bkgd)
    
    ty, tx = uc.getmaxyx(win)
    if pos and pos!=uc.getbegyx(win):
      uc.move_panel(panel,*pos)

    uc.wbkgd(win,bkgd)
    if highlight:  uc.wattron(win,highlight)
    uc.box(win)
    if small:
      uc.mvwaddch(win,ty-1,0,uc.ACS_VLINE)
      uc.mvwaddch(win,ty-1,tx-1,uc.ACS_VLINE)
    if highlight:  uc.wattroff(win,highlight)
    if small:
      uc.mvwaddstr(win,ty-1,1,' '*(tx-2))

    if issubclass(type(self),Card_Minion):
      mid = card_size[0]/2
      y,x,h,w = 1,2,mid-2,tx-4
      manual_box(win,y,x,h,w)
      print_longtext(win,y+1,x+1,y+h-1,x+w-1,name,uc.magenta_on_black)
      uc.mvwaddstr(win,y+h-1,x+1,"%2d "%self.atq)
      uc.mvwaddstr(win,y+h-1,x+w-4,"%2d "%self.hp)
      print_longtext(win,mid,2,ty-1,tx-2,desc)
    else:
      uc.mvwaddch(win, 3, 0, uc.ACS_LTEE, highlight)
      uc.mvwhline(win, 3, 1, uc.ACS_HLINE, tx-2)
      uc.mvwaddch(win, 3, tx-1, uc.ACS_RTEE, highlight)
      print_longtext(win,1,1,3,tx-1,name,uc.yellow_on_black)
      print_longtext(win,4,2,ty,tx-2,desc)

    # print cost
    if cost==None:
      cost=self.cost
    if cost==self.cost:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.black_on_cyan)
    elif cost<self.cost:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.white_on_green)
    else:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.white_on_red)

# Messages ---------

def draw_Message(self):
    pass

def draw_Action(self):
    pass

def draw_Msg_StartTurn(self):
    button = Button(10,37," %s's turn! "%self.caster.name,tx=20,ty=5)
    button.draw(highlight=uc.black_on_yellow)
    uc.update_panels()
    uc.doupdate()
    time.sleep(1)
    button.delete()
    self.engine.board.draw()

def draw_Msg_DrawCard(self):
    bottom_player = self.engine.board.get_top_bottom_players()[1]
    if bottom_player==self.caster:
      self.engine.board.draw('cards',which=self.caster,last_card=False)
      card = self.card
      NR,NC = uc.getmaxyx(stdscr)
      ty,tx = card_size
      sy,sx = 12, NC-tx
      ey,ex = self.engine.board.get_card_pos(card)
      for y in range(sy,ey+1):
        x = int(0.5+sx+(ex-sx)*(y-sy)/float(ey-sy))
        h = max(0,NR-y)
        card.draw(highlight=uc.black_on_yellow,pos=(y,x),small=0 if h>=ty else h)
        uc.update_panels()
        uc.doupdate()
        time.sleep(0.05 + 0.6*(y==sy))
    self.engine.board.draw('cards',which=self.caster)

def draw_Msg_EndTurn(self):
    for card in self.caster.cards:
      if hasattr('card','small_panel'):
        uc.hide_panel(card.small_panel)

def draw_Msg_UseMana(self):
    self.engine.board.draw('mana',self.caster)

def draw_Msg_ThrowCard(self):
    card = self.card
    if hasattr(card,'panel'):
      uc.del_panel(card.panel)
      del card.win
      del card.panel
    if hasattr(card,'small_panel'):
      uc.del_panel(card.small_panel)
      del card.small_win
      del card.small_panel
    self.engine.board.draw('cards',which=self.caster)

def draw_Msg_AddMinion(self):
    self.engine.board.draw('minions',which=self.caster)

def draw_Msg_DeadMinion(self):
    uc.del_panel(self.caster.panel)
    del self.caster.panel
    del self.caster.win
    self.engine.board.draw('minions',which=self.caster.owner)


"""

def draw_Msg_Popup(self):
    print "Minion '%s' pops up" %(self.caster)

def draw_Msg_Dying(self):
    print "Death of "+str(self.caster)

def draw_Msg_StartAttack(self):
    print "%s attacks %s !" % (self.caster, )

def draw_Msg_EndAttack(self):
    pass

def draw_Msg_Damage (TargetedMessage):
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        self.target.hurt(self.damage)


def draw_Msg_SpellDamage (Msg_Damage):
    ''' same but launched
     by a spell'''
    pass

def draw_Msg_Heal (TargetedMessage):
    def __init__(self, caster, target, heal):
        Message.__init__(self, caster, target)
        self.heal = heal
    def execute(self):
        self.target.heal(self.heal)

# card/spell messages

def draw_Msg_StartCard(self):
    pass
def draw_Msg_EndCard(self):
    pass

def draw_Msg_StartSpell (Msg_StartCard):
    pass
def draw_Msg_EndSpell (Msg_EndCard):
    pass
"""

### Board --------
def get_card_pos(self, card):
  NR,NC = uc.getmaxyx(stdscr)
  nc = len(card.owner.cards)
  ty,tx = card_size
  actual_NC = min(NC,tx*nc)
  startx = (NC - actual_NC)/2
  i = card.owner.cards.index(card)
  return 24, startx + int((actual_NC-tx)*i/(nc-1))

Board.get_card_pos = get_card_pos

def get_top_bottom_players(self):
  player = self.engine.get_current_player()
  adv = self.engine.get_other_player()
  if self.switch==False and self.engine.turn%2: 
    player,adv = adv,player # prevent top/down switching
  return adv, player

Board.get_top_bottom_players = get_top_bottom_players


def draw_Board(self, what='bkgd decks hero cards mana minions', which=None, last_card=True):
    NR,NC = uc.getmaxyx(stdscr)
    
    # clear screen
    adv, player = self.get_top_bottom_players()
    which = {player,adv} if not which else {which}
    
    if 'bkgd' in what:
      # background
      uc.erase()
      uc.mvhline(2,0,uc.ACS_CKBOARD,NC)
      uc.mvhline(3,0,uc.ACS_CKBOARD,NC)
      uc.mvhline(12,0,ord('-'),NC)
      uc.mvhline(21,0,uc.ACS_CKBOARD,NC)
      uc.mvhline(22,0,uc.ACS_CKBOARD,NC)
      if not hasattr(self,"end_turn"):
        self.end_turn = Button(11,NC,"End turn",align='right')
      self.end_turn.draw()
    
    # draw decks on the right side
    if 'decks' in what:
      for i in range(3):
        uc.mvvline(2,NC-1-i,uc.ACS_CKBOARD,20)
      for i in [5,15]:
        uc.mvaddch(i,NC-2,uc.ACS_HLINE)
        uc.addstr(unichr(9558).encode(code))
        text = ' %2d'%len(i<12 and adv.deck or player.deck)
        for j,ch in enumerate(text):
          uc.mvaddch(i+1+j,NC-2,ord(ch))
          uc.addstr(unichr(9553).encode(code))
        uc.mvaddch(i+4,NC-2,uc.ACS_HLINE)
        uc.addstr(unichr(9564).encode(code))
    
    # draw heroes
    if 'hero' in what:
      if adv in which:
        adv.hero.draw(pos=(1,(NC-18)/2))
      if player in which:
        player.hero.draw(pos=(20,(NC-18)/2))
    
    # draw cards
    if 'cards' in what:
      if adv in which:
        print_middle(stdscr, 0,0, NC, " Adversary has %d cards. "%len(adv.cards))
      if player in which:
        cards = player.cards if last_card else player.cards[:-1]
        for card in cards:
          card.draw(pos=self.get_card_pos(card), small=NR-24)
    
    # draw mana
    if 'mana' in what:
      for who,i in [(adv,2),(player,22)]:
        if who not in which:  continue
        p = i<12 and adv or player
        text = "%d/%d "%(p.mana,p.max_mana)
        uc.mvaddstr(i,NC-11-len(text), text, uc.black_on_cyan)
        uc.mvaddstr(i,NC-11, (unichr(9826)*p.max_mana).encode(code), uc.cyan_on_black)
        uc.mvaddstr(i,NC-11, (unichr(9830)*p.mana).encode(code), uc.cyan_on_black)
    
    # draw minions
    if 'minions' in what:
      if player in which:
        for m in player.minions:
          m.draw(y=14)
      if adv in which:
        for m in adv.minions:
          m.draw(y=5)
    
    uc.update_panels()
    uc.doupdate()


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
      engine.board.draw()
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
        uc.update_panels()
        uc.doupdate()
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
      uc.update_panels()
      uc.doupdate()
      
      return discarded

  def choose_actions(self, actions):
      # split actions
      showlist = [] # [(action, object, draw_kwargs)]
      remaining_cards = set(self.cards)
      for a in actions:
        if issubclass(type(a),Act_HeroPower):
          pass #act_hero.append(a)
        elif issubclass(type(a),Act_PlayCard):
          showlist.append((a,a.card,{'small':True}))
          if a.card in remaining_cards: # choice_of_cards
            remaining_cards.remove(a.card)
        elif issubclass(type(a),Act_Attack):
          pass  #act_atq.append(a)
        else:
          showlist.append((a,self.engine.board.end_turn,{}))
      # we can also inspect non-playable cards
      for card in remaining_cards:
        showlist.append((None,card,{'small':True}))
      
      def erase_elems(showlist):
        for a,obj,kwargs in showlist:
          obj.draw(small=True)  # erase everything
      
      init_showlist = showlist
      self.engine.board.draw()
      last_sel = None
      while True:
        mapping = {}  # obj -> action,draw_kwargs
        for a,obj,kwargs in showlist:
          highlight = uc.black_on_green if a else 0
          obj.draw(highlight=highlight,**kwargs)
          mapping[obj] = (a,kwargs)
        
        uc.update_panels()
        uc.doupdate()
        
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
              # put in last position = front
              #showlist.remove((a,sel,kwargs))
              #showlist.append((a,sel,kwargs))
            last_sel = sel
          
          elif bstate & uc.BUTTON1_RELEASED:
            if sel!=last_sel: continue 
            if sel not in mapping: continue
            act = mapping[sel][0]
            if not act: continue
            
            if issubclass(type(act), Action):
              action = act
              choices = []  # reset choices
            else:
              choices.append(act)
            if len(choices)>=len(action.choices):
              erase_elems(showlist)
              return action.select(choices)
            erase_elems(showlist)
            showlist = [(obj,obj,{}) for obj in action.choices[len(choices)]]
          
          else:
            erase_elems(showlist)
            showlist = init_showlist
        
        elif ch == 27: # escape
          erase_elems(showlist)
          showlist = init_showlist
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

  def display_msg(self, msg):
    self.display.append(msg)

  def wait_for_display(self):
    while self.display:
      msg = self.display.pop(0)
      self.log += '%s\n' % msg
      msg.draw()




if __name__=="__main__":
    from cards import fake_deck
    

    deck1 = fake_deck()
    hero1 = Hero(Card_Mage())
    player1 = HumanPlayerAscii(hero1, 'jerome', deck1)

    deck2 = fake_deck()
    hero2 = Hero(Card_Priest())
    player2 = RandomPlayer(hero2, 'IA', deck2)

    engine = CursesHSEngine( player1, player2 )
    engine.board.switch = False
    # initialize global variables

    # start playing
    stdscr = init_screen()
    #show_ACS()
    #show_unicode()
    engine.start_game()
    while not engine.is_game_ended():
      engine.play_turn()

    uc.endwin()
    t = engine.turn
    print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)


















