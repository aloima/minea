#include "../minea.h"

#include <stdbool.h>
#include <stdlib.h>

#include <ncurses.h>
#include <ncurses/menu.h>

void init_menu();
void about_page();
void clear_menu(ITEM **items, MENU *menu, WINDOW *menu_win, WINDOW *menu_derwin);

void about_page() {
  int total_cols, total_lines;
  getmaxyx(stdscr, total_lines, total_cols);

  int lines = total_lines * 0.8;
  int cols = total_cols * 0.6;
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
    int c = wgetch(win);

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

void clear_menu(ITEM **items, MENU *menu, WINDOW *menu_win, WINDOW *menu_derwin) {
  unpost_menu(menu);
  free_item(items[0]);
  free_item(items[1]);
  free_item(items[2]);
  free_menu(menu);
  free(items);
  delwin(menu_derwin);
  delwin(menu_win);
  clear();
  refresh();
}

void init_menu() {
  ITEM **items = malloc(4 * sizeof(ITEM *));
  items[0] = new_item("Play", NULL);
  items[1] = new_item("About", NULL);
  items[2] = new_item("Exit", NULL);
  items[3] = NULL;

  int total_cols, total_lines;
  getmaxyx(stdscr, total_lines, total_cols);

  MENU *menu = new_menu(items);
  WINDOW *menu_win = newwin(7, 30, (total_lines - 7) / 2, (total_cols - 30) / 2);
  keypad(menu_win, true);

  set_menu_mark(menu, " ");
  set_menu_win(menu, menu_win);

  WINDOW *menu_derwin = derwin(menu_win, 3, 8, 2, 11);
  set_menu_sub(menu, menu_derwin);

  box(menu_win, 0, 0);
  refresh();

  post_menu(menu);
  wrefresh(menu_win);

  int c;

  while ((c = wgetch(menu_win)) != 'q') {
    switch (c) {
      case KEY_DOWN:
        menu_driver(menu, REQ_DOWN_ITEM);
        break;

      case KEY_UP:
        menu_driver(menu, REQ_UP_ITEM);
        break;

      case 10: { // enter key
        switch (item_index(current_item(menu))) {
          case 1:
            clear_menu(items, menu, menu_win, menu_derwin);
            about_page();
            return;

          case 2:
            clear_menu(items, menu, menu_win, menu_derwin);
            return;
        }

        break;
      }

      case KEY_RESIZE: {
        clear_menu(items, menu, menu_win, menu_derwin);
        init_menu();
        return;
      }
    }
  }

  clear_menu(items, menu, menu_win, menu_derwin);
}

void init_app() {
  initscr();
  cbreak();
  noecho();

  init_menu();

  endwin();
}
