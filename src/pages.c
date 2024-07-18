#include "../minea.h"

#include <stdbool.h>
#include <stdint.h>

#include <ncurses.h>

void about_page() {
  int32_t total_cols, total_lines;
  getmaxyx(stdscr, total_lines, total_cols);

  int32_t lines = total_lines * 0.8;
  int32_t cols = total_cols * 0.6;
  WINDOW *win = newwin(lines, cols, total_lines * 0.1, total_cols * 0.2);
  box(win, 0, 0);

  mvwaddstr(win, 1, 2, "minea is a ncurses based minesweeper game made for hobby.");
  mvwaddstr(win, 3, 2, "Maintainer: aloima (https://github.com/aloima)");
  mvwaddstr(win, 4, 2, "Repository: https://github.com/aloima/minea");
  mvwaddstr(win, 5, 2, "License: BSD-3 Clause Clear License");
  mvwaddstr(win, 6, 2, "Version: 0.1");
  mvwaddstr(win, lines - 2, 2, "Press 'b' to back menu.");
  wrefresh(win);

  refresh();

  while (true) {
    int32_t c = wgetch(win);

    switch (c) {
      case 'b':
        delwin(win);
        clear();
        refresh();
        init_menu();
        return;

      case KEY_RESIZE:
        delwin(win);
        clear();
        refresh();
        about_page();
        return;
    }
  }
}

void options_page() {
  
}
