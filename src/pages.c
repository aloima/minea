#include "../minea.h"

#include <stdbool.h>
#include <stdint.h>

#include <ncurses.h>

void about_page() {
  const uint32_t total_cols = getmaxx(stdscr);
  const uint32_t total_lines = getmaxy(stdscr);

  const uint32_t lines = total_lines * 0.8;
  const uint32_t cols = total_cols * 0.6;
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

void lose_page(struct Tiles tiles) {
  clear();
  refresh();

  const uint32_t total_lines = getmaxy(stdscr);
  const uint32_t total_cols = getmaxx(stdscr);

  WINDOW *win = newwin(6, 30, (total_lines - 6) / 2, (total_cols - 30) / 2);
  box(win, 0, 0);

  mvwaddstr(win, 2, 7, "You lost the game.");
  mvwaddstr(win, 3, 4, "Press 'b' to back menu.");
  wrefresh(win);

  refresh();

  while (true) {
    int32_t c = wgetch(win);

    switch (c) {
      case 'b':
        free_tiles(tiles);
        delwin(win);
        clear();
        refresh();
        init_menu();
        clear();
        return;

      case KEY_RESIZE:
        delwin(win);
        clear();
        refresh();
        lose_page(tiles);
        return;
    }
  }
}
