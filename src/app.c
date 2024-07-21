#include "../minea.h"

#include <ncurses.h>
#include <stdint.h>
#include <string.h>

void redraw_board(struct Tiles tiles, uint32_t y, uint32_t x) {
  for (uint32_t i = 0; i < tiles.size; ++i) {
    const uint32_t mx = i % 9;
    const uint32_t my = i / 9;

    tile_t *tile = get_tile(tiles, mx + 1, my + 1);
    char s[12];

    if (tile->flagged) {
      strcpy(s, "F");
    } else if (tile->mine) {
      strcpy(s, "M");
    } else if (tile->opened) {
      strcpy(s, "O");
    } else {
      strcpy(s, "A");
    }

    mvaddstr(my * 2 + 1 + y, mx * 4 + 2 + x, s);
  }
}

void generate_board(struct Options options, uint32_t y, uint32_t x) {
  /*
    fl = first line
    l1 = vertical line
    l2 = line with spaces
    ll = last line
  */
  const uint32_t ml4 = options.minefield_len * 4;
  chtype fl[ml4 + 2], ll[ml4 + 2];
  chtype l1[ml4 + 2], l2[ml4 + 2];

  for (uint32_t i = 0; i < options.minefield_len; ++i) {
    const uint32_t i4 = (i * 4);

    if (i == 0) {
      fl[i4] = ACS_ULCORNER;
      l1[i4] = ACS_LTEE;
      ll[i4] = ACS_LLCORNER;
    } else {
      fl[i4] = ACS_TTEE;
      l1[i4] = ACS_PLUS;
      ll[i4] = ACS_BTEE;
    }

    fl[i4 + 1] = ACS_HLINE;
    fl[i4 + 2] = ACS_HLINE;
    fl[i4 + 3] = ACS_HLINE;

    l1[i4 + 1] = ACS_HLINE;
    l1[i4 + 2] = ACS_HLINE;
    l1[i4 + 3] = ACS_HLINE;

    ll[i4 + 1] = ACS_HLINE;
    ll[i4 + 2] = ACS_HLINE;
    ll[i4 + 3] = ACS_HLINE;

    l2[i4] = ACS_VLINE;
    l2[i4 + 1] = ' ';
    l2[i4 + 2] = ' ';
    l2[i4 + 3] = ' ';
  }

  fl[ml4] = ACS_URCORNER;
  fl[ml4 + 1] = 0;

  l1[ml4] = ACS_RTEE;
  l1[ml4 + 1] = 0;

  l2[ml4] = ACS_VLINE;
  l2[ml4 + 1] = 0;

  ll[ml4] = ACS_LRCORNER;
  ll[ml4 + 1] = 0;

  mvaddchstr(y, x, fl);
  mvaddchstr(y + 1, x, l2);

  for (uint32_t i = 1; i < options.minefield_len; ++i) {
    mvaddchstr(y + i * 2, x, l1);
    mvaddchstr(y + i * 2 + 1, x, l2);
  }

  mvaddchstr(y + options.minefield_len * 2, x, ll);

  refresh();
}

void init_game() {
  read_options();
  struct Options options = get_options();

  bool placed_mines = false;

  uint32_t start_y = (getmaxy(stdscr) - (options.minefield_len * 2 + 1)) / 2;
  uint32_t start_x = (getmaxx(stdscr) - (options.minefield_len * 4 + 1)) / 2;

  generate_board(options, start_y, start_x);
  keypad(stdscr, true);

  struct Tiles tiles = generate_empty_tiles(options.minefield_len);

  while (true) {
    const int32_t c = getch();

    switch (c) {
      case 'b':
        clear();
        refresh();
        init_menu();
        return;

      case KEY_RESIZE:
        clear();
        start_y = (getmaxy(stdscr) - (options.minefield_len * 2 + 1)) / 2;
        start_x = (getmaxx(stdscr) - (options.minefield_len * 4 + 1)) / 2;
        generate_board(options, start_y, start_x);

        if (placed_mines) {
          redraw_board(tiles, start_y, start_x);
        }

        break;
      
      case KEY_MOUSE: {
        MEVENT event;

        if (getmouse(&event) == OK) {
          switch (event.bstate) {
            case BUTTON1_CLICKED: {
              const uint32_t _y = event.y - start_y;
              const uint32_t _x = event.x - start_x;

              const pos_t click_at = {
                .y = (_y % 2 == 0) ? 0 : ((_y / 2) + 1),
                .x = (_x % 4 == 0) ? 0 : ((_x / 4) + 1)
              };

              struct Offset offset = {
                .bottom = 1,
                .top = 1,
                .left = 1,
                .right = 1
              };

              placed_mines = place_mines(tiles, options.mine_count, click_at, offset);
              redraw_board(tiles, start_y, start_x);

              break;
            }
          }
        }
      }
    }
  }
}

void init_app() {
  initscr();
  cbreak();
  noecho();
  notimeout(stdscr, true);
  curs_set(0);

  mousemask(ALL_MOUSE_EVENTS, NULL);
  init_menu();

  endwin();
}
