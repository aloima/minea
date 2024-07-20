#include "../minea.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <ncurses.h>
#include <ncurses/menu.h>

void clear_menu(ITEM **items, MENU *menu, WINDOW *menu_win, WINDOW *menu_derwin) {
  unpost_menu(menu);
  free_menu(menu);

  free_item(items[0]);
  free_item(items[1]);
  free_item(items[2]);
  free_item(items[3]);
  free(items);

  delwin(menu_derwin);
  delwin(menu_win);

  clear();
  refresh();
}

void handle_menu_enter(int32_t index, ITEM **items, MENU *menu, WINDOW *menu_win, WINDOW *menu_derwin) {
  switch (index) {
    case 0:
      clear_menu(items, menu, menu_win, menu_derwin);
      init_game();
      return;

    case 1:
      clear_menu(items, menu, menu_win, menu_derwin);
      about_page();
      return;

    case 2:
      clear_menu(items, menu, menu_win, menu_derwin);
      options_page();
      return;

    case 3:
      clear_menu(items, menu, menu_win, menu_derwin);
      return;

    default:
      return;
  }
}

void init_menu() {
  ITEM **items = malloc(5 * sizeof(ITEM *));
  items[0] = new_item("Play", NULL);
  items[1] = new_item("About", NULL);
  items[2] = new_item("Options", NULL);
  items[3] = new_item("Exit", NULL);
  items[4] = NULL;

  const uint32_t total_lines = getmaxy(stdscr);
  const uint32_t total_cols = getmaxx(stdscr);

  MENU *menu = new_menu(items);
  WINDOW *menu_win = newwin(8, 30, (total_lines - 7) / 2, (total_cols - 30) / 2);
  keypad(menu_win, true);

  set_menu_mark(menu, " ");
  set_menu_win(menu, menu_win);

  WINDOW *menu_derwin = derwin(menu_win, 4, 8, 2, 11);
  set_menu_sub(menu, menu_derwin);

  box(menu_win, 0, 0);
  refresh();

  const int32_t menubegy = getbegy(menu_derwin);

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

      case KEY_MOUSE: {
        MEVENT event;

        if (getmouse(&event) == OK) {
          switch (event.bstate) {
            case BUTTON1_CLICKED: {
              const int32_t mitem_count = item_count(menu);

              if (menubegy <= event.y && event.y <= (menubegy + mitem_count - 1)) {
                set_current_item(menu, items[event.y - menubegy]);
              }

              break;
            }

            case BUTTON1_DOUBLE_CLICKED: {
              const int32_t mitem_count = item_count(menu);

              if (menubegy <= event.y && event.y <= (menubegy + mitem_count - 1)) {
                handle_menu_enter(event.y - menubegy, items, menu, menu_win, menu_derwin);
                return;
              }

              break;
            }
          }
        }

        break;
      }

      case 10: // enter key
        handle_menu_enter(item_index(current_item(menu)), items, menu, menu_win, menu_derwin);
        return;

      case KEY_RESIZE:
        clear_menu(items, menu, menu_win, menu_derwin);
        init_menu();
        return;
    }
  }

  clear_menu(items, menu, menu_win, menu_derwin);
}
