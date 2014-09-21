import pdb
from board import Board, Slot
from players import *
from messages import *
from actions import *
from heroes import *
from cards import Card
import unicurses as uc

"""
def show_Msg_StartTurn(self):
    print "Start of turn for hero "+self.caster.show()

def show_Msg_EndTurn(self):
    print "End of turn for hero "+self.caster.show()

def show_Msg_Nothing(self):
    pass

def show_Msg_AddMinion(self):
    print "New minion '%s' appears for Hero %s" %(self.minion, self.caster)

def show_Msg_Popup(self):
    print "Minion '%s' pops up" %(self.caster)

def show_Msg_Dying(self):
    print "Death of "+str(self.caster)

def show_Msg_StartAttack(self):
    print "%s attacks %s !" % (self.caster, )

def show_Msg_EndAttack(self):
    pass

def show_Msg_Damage (TargetedMessage):
    def __init__(self, caster, target, damage):
        Message.__init__(self, caster, target)
        self.damage = damage
    def execute(self):
        self.target.hurt(self.damage)


def show_Msg_SpellDamage (Msg_Damage):
    ''' same but launched
     by a spell'''
    pass


def show_Msg_Heal (TargetedMessage):
    def __init__(self, caster, target, heal):
        Message.__init__(self, caster, target)
        self.heal = heal
    def execute(self):
        self.target.heal(self.heal)


# card/spell messages

def show_Msg_StartCard(self):
    pass
def show_Msg_EndCard(self):
    pass

def show_Msg_StartSpell (Msg_StartCard):
    pass
def show_Msg_EndSpell (Msg_EndCard):
    pass

# attach each show function to a message
show_funcs = [key for key in globals() if key.startswith("show_")]
for key in show_funcs:
    if key[5:] in globals():
      setattr(globals()[key[5:]], "show", globals()[key])

pdb.set_trace()



def weapon_string(weapon):
    return weapon.name + " %d/%d"%(weapon.attack,weapon.durability)

def creature_string(c):
    return c.name+"(%d/%d)\t"%(c.atq,c.hp)

def hand_card_string(c):
    return c.name + "(%d)"%c.card.cost
"""

def init_screen():
  stdscr = uc.initscr()
  uc.start_color()
  uc.cbreak()
  uc.curs_set(0)
  uc.noecho()
  uc.keypad(stdscr, True)
  uc.mouseinterval(0)
  uc.mousemask(uc.ALL_MOUSE_EVENTS | uc.REPORT_MOUSE_POSITION)
  # init color pairs
  color_pairs = []
  def create_color_pair( col_fg, col_bg ):
    name={0:'black',1:'blue',2:'green',3:'cyan',4:'red',5:'magenta',6:'yellow',7:'white'}
    colname = '%s_on_%s' % (name[col_fg],name[col_bg])
    color_pairs.append((col_fg,col_bg))
    ncol = len(color_pairs)
#    assert ncol<7, 'error: too many color pairs'
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
  create_color_pair(uc.COLOR_CYAN, uc.COLOR_BLACK)
  return stdscr

def print_middle(win,y,x,width,text,attr=None):
    if win==None: win = stdscr
    x = max(x, x+(width - len(text)) / 2)
    uc.mvwaddstr(win, y, x, text[:width], attr)

def print_longtext(win,y,x,endy,endx,text,attr=None):
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
    for i in range(65536):
      uc.addch(i)
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

def draw_Hero(self, pos=None, bkgd=0, highlight=0, **kwargs):
    #if not hasattr()
    NR,NC = uc.getmaxyx(stdscr)
    ty,tx = 4,12
    if not hasattr(self,"win"):
      self.win = uc.newwin(ty,tx,pos[0],pos[1])
      self.panel = uc.new_panel(self.win)
      set_panel_userptr(self.panel, self)
    win = self.win
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

class Button:
  def __init__(self, y,x, text, align='center'):
      self.text = text
      if align=='left': pass
      elif align=='center': x -= len(text)/2+1
      elif align=='right': x -= len(text)+2
      win = self.win = uc.newwin(3,len(text)+2,y,x)
      self.panel = uc.new_panel(win)
      set_panel_userptr(self.panel, self)

  def draw(self, highlight=0, bkgd=0, **kwargs):
      uc.wbkgd(self.win,bkgd)
      uc.wattron(self.win,highlight)
      uc.box(self.win)
      uc.wattroff(self.win,highlight)
      uc.mvwaddstr(self.win,1,1,self.text)
      return self.win

def get_minion_pos(self):
    """ return position, space """
    NR,NC = uc.getmaxyx(stdscr)
    n = len(self.owner.minions)
    sp = ([3]*5+[2,1,0])[n] # spacement between minions
    y = 14 if self.owner==self.engine.get_current_player() else 5
    return y, int(NC-3-(11+sp)*n)/2+(11+sp)*self.pos, sp

Slot.get_minion_pos = get_minion_pos

def draw_Slot(self, highlight=0, bkgd=0, **kwargs):
    if not hasattr(self,"win"):
      y, x, sp = self.get_minion_pos()
      win = self.win = uc.newwin(5,sp,y,x-sp)
      self.panel = uc.new_panel(win)
      set_panel_userptr(self.panel, self)
    
    if bkgd or highlight:
      uc.top_panel(self.panel)
      uc.wbkgd(self.win,bkgd or highlight)
    else:
      uc.del_panel(self.panel)
      del self.panel
      del self.win

def draw_Minion(self, highlight=0, bkgd=0, **kwargs):
    slot = self.engine.board.get_minion_pos(self)
    y,x = slot.get_minion_pos()[:2]
    
    if not hasattr(self,"win"):
      win = self.win = uc.newwin(5,11,y,x)
      self.panel = uc.new_panel(win)
      set_panel_userptr(self.panel, self)
    else:
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


#Draws board and cards
def draw_Board(self):
    # clear screen
    player = self.engine.get_current_player()
    adv = self.engine.get_other_player()

    NR,NC = uc.getmaxyx(stdscr)
    uc.erase()
    uc.mvhline(2,0,uc.ACS_CKBOARD,NC)
    uc.mvhline(3,0,uc.ACS_CKBOARD,NC)

    uc.mvhline(12,0,ord('-'),NC)
    uc.mvhline(21,0,uc.ACS_CKBOARD,NC)
    uc.mvhline(22,0,uc.ACS_CKBOARD,NC)

    # draw right side
    for i in range(3):
      uc.mvvline(2,NC-1-i,uc.ACS_CKBOARD,20)
    for i in [5,15]:
      uc.mvaddch(i,NC-2,uc.ACS_HLINE)
      uc.addch(9558)
      uc.mvvline(i+1,NC-1,9553,3)
      text = ' %2d'%len(i<12 and adv.deck or player.deck)
      for j,ch in enumerate(text):
        uc.mvaddch(i+1+j,NC-2,ord(ch))
      uc.mvaddch(i+4,NC-2,uc.ACS_HLINE)
      uc.addch(9564)

    print_middle(stdscr, 0,0, NC, " Adversary has %d cards. "%len(adv.cards))
    adv.hero.draw(pos=(1,(NC-18)/2))
    player.hero.draw(pos=(20,(NC-18)/2))

    # draw mana
    for i in [2,22]:
      p = i<12 and adv or player
      text = "%d/%d "%(p.mana,p.max_mana)
      uc.mvaddstr(i,NC-11-len(text), text, uc.black_on_cyan)
      uc.mvhline(i,NC-11,9826 | uc.cyan_on_black, p.max_mana)
      uc.mvhline(i,NC-11,9830 | uc.cyan_on_black, p.mana)

    # draw minions
    for m in player.minions:
      m.draw()
    for m in adv.minions:
      m.draw()

    if not hasattr(self,"end_turn"):
      self.end_turn = Button(11,NC,"End turn",align='right')
    self.end_turn.draw()

    # draw cards
    nc = len(player.cards)
    actual_NC = min(NC,15*nc)
    startx = (NC - actual_NC)/2
    for i,card in enumerate(player.cards):
      x = startx + int(actual_NC*i/nc)
      card.draw(pos=(24,x), small=NR-24)

    #uc.touchwin(self.end_turn.win)
    uc.update_panels()
    uc.doupdate()

def draw_Card(self, pos=None, highlight=0, cost=None, small=True, bkgd=0, **kwargs):
    name = self.name_fr or self.name
    desc = self.desc_fr or self.desc

    if not small:
      ty,tx = 14,15
      if not pos and hasattr(self,"small_panel"):
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
        ty, tx = small, 15
        self.small_win = uc.newwin(ty,tx,pos[0],pos[1])
        self.small_panel = uc.new_panel(self.small_win)
        set_panel_userptr(self.small_panel, self)
      win, panel = self.small_win, self.small_panel

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
      if small:
        mid = ty
        y,x,h,w = 1,2,ty-1,tx-4
      else:
        mid = ty/2
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
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.black_on_cyan) #uc.col_white_blue)
    elif cost<self.cost:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.white_on_green) #uc.col_white_blue)
    else:
      uc.mvwaddstr(win, 0,0, "(%d)"%cost, uc.white_on_red) #uc.col_white_blue)
    
  

# attach each show function to a message
all_globs = globals().keys()
draw_funcs = [key for key in all_globs if key.startswith("draw_")]
for key in draw_funcs:
    if key[5:] in all_globs:
      setattr(globals()[key[5:]], "draw", globals()[key])

def mouse_in_win(win,y,x):
  wy,wx = uc.getbegyx(win)
  height, width = uc.getmaxyx(win)
  return wx<=x<wx+width and wy<=y<wy+height


class HumanPlayerAscii (HumanPlayer):
  ''' human player : ask the player what to do'''
  def mulligan(self, cards):
      engine.board.draw()
      NR,NC = uc.getmaxyx(stdscr)
      nc = len(cards)
      for i,card in enumerate(cards):
          card.draw(pos=(6,int(NC*(i+0.5)/nc-7)), small=False, highlight=uc.black_on_green)

      discarded = []
      while True:
        uc.update_panels()
        uc.doupdate()
        ch = uc.getch()
        if ch == uc.KEY_MOUSE:
          mouse_state = uc.getmouse()
          if mouse_state==uc.ERR: continue
          id, x, y, z, bstate = mouse_state
          if not(bstate & uc.BUTTON1_PRESSED): continue

          which = None
          for i,card in enumerate(cards):
            if mouse_in_win(card.win,y,x):
              which = card
              break
          if not which: continue

          if which in discarded:
            uc.wbkgd(card.win,uc.COLOR_PAIR(0))
            discarded.remove(which)
          else:
            uc.wbkgd(card.win,uc.black_on_red)
            discarded.append(which)
        elif ch in (ord(' '), ord('\n')):
          break

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
      while True:
        mapping = {}  # obj -> action,draw_kwargs
        for a,obj,kwargs in showlist:
          highlight = uc.black_on_green if a else 0
          obj.draw(highlight=highlight,**kwargs)
          mapping[obj] = (a,kwargs)
        
        uc.mvaddstr(5,0,"%d choices"%(len(showlist)))
        uc.update_panels()
        uc.doupdate()
        ch = uc.getch()
        
        if ch==uc.KEY_MOUSE:
          mouse_state = uc.getmouse()
          if mouse_state==uc.ERR: continue
          id, x, y, z, bstate = mouse_state
          #uc.mvaddstr(5,0,"mouse %d %d %s"%(y,x,bin(bstate)))

          sel = None
          cur = uc.panel_below(None) # get top panel
          while cur:
            obj = get_panel_userptr(cur)
            if issubclass(type(obj),Card) or obj in mapping:
              if mouse_in_win(uc.panel_window(cur),y,x):
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

          elif bstate & uc.BUTTON1_RELEASED:
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
          assert 0


if __name__=="__main__":
    from cards import fake_deck
    from hs_engine import HSEngine

    deck1 = fake_deck()
    hero1 = Hero(Card_Mage())
    player1 = HumanPlayerAscii(hero1, 'jerome', deck1)

    deck2 = fake_deck()
    hero2 = Hero(Card_Priest())
    player2 = RandomPlayer(hero2, 'IA', deck2)

    engine = HSEngine( player1, player2 )
    # initialize global variables

    # start playing
    stdscr = init_screen()
#    show_ACS()
#    show_unicode()
    engine.start_game()
    while not engine.is_game_ended():
      engine.board.draw()
      engine.play_turn()
      assert 0

    uc.endwin()
    t = engine.turn
    print 'end of game: player %d won after %d turn' % (t%2, (t+1)/2)


















