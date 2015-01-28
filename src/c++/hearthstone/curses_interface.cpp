#include "curses_interface.h"

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


void init_screen() {
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
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION);
  
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