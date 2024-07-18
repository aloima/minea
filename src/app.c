#include "../minea.h"

#include <ncurses.h>

void init_app() {
  initscr();
  cbreak();
  noecho();

  init_menu();

  endwin();
}
