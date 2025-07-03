#include <minea.h>

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
  const uint32_t total_cols = getmaxx(stdscr);
  const uint32_t total_lines = getmaxy(stdscr);

  const uint32_t lines = 7;
  const uint32_t cols = 30;

  const uint32_t begin_y = (total_lines - lines) / 2;
  const uint32_t begin_x = (total_cols - cols) / 2;

  WINDOW *win = newwin(lines, cols, begin_y, begin_x);
  keypad(win, true);
  box(win, 0, 0);

  struct Options options = get_options();
  char minefield_len[8], mine_count[8];
  sprintf(minefield_len, "%d", options.minefield_len);
  sprintf(mine_count, "%d", options.mine_count);

  mvwaddstr(win, 1, 2, "Mines count:");
  mvwaddstr(win, 1, 15, mine_count);

  mvwaddstr(win, 2, 2, "Minefield size:");
  mvwaddstr(win, 2, 18, minefield_len);

  mvwaddstr(win, 5, 2, "Press 'b' to back menu.");
  curs_set(2);
  wrefresh(win);

  refresh();
  wmove(win, 1, 15);

  while (true) {
    int32_t c = wgetch(win);

    switch (c) {
      case 'b':
        curs_set(0);
        delwin(win);
        clear();
        refresh();
        init_menu();
        return;

      case KEY_MOUSE: {
        MEVENT event;

        if (getmouse(&event) == OK) {
          switch (event.bstate) {
            case BUTTON1_CLICKED: {
              const int32_t x = event.x - begin_x;
              const int32_t y = event.y - begin_y;

              if (x > 0 && x < cols && y > 0 && y < lines) {
                switch (y) {
                  case 1:
                    wmove(win, 1, 15);
                    break;

                  case 2:
                    wmove(win, 2, 18);
                    break;

                  default:
                    break;
                }
              }
            }
          }
        }

        break;
      }

      case KEY_RESIZE:
        delwin(win);
        clear();
        refresh();
        options_page();
        return;
    }
  }
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

void win_page(struct Tiles tiles) {
  clear();
  refresh();

  const uint32_t total_lines = getmaxy(stdscr);
  const uint32_t total_cols = getmaxx(stdscr);

  WINDOW *win = newwin(6, 30, (total_lines - 6) / 2, (total_cols - 30) / 2);
  box(win, 0, 0);

  mvwaddstr(win, 2, 7, "You win the game.");
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
