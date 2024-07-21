#include "../minea.h"

#include <ncurses.h>
#include <stdint.h>
#include <string.h>

void redraw_board(struct Tiles tiles, uint32_t y, uint32_t x, uint32_t remaining_flags) {
  mvprintw(y - 1, x, "Remaining flags: %-3u", remaining_flags);

  for (uint32_t i = 0; i < tiles.size; ++i) {
    const uint32_t mx = i % 9;
    const uint32_t my = i / 9;

    tile_t *tile = get_tile(tiles, mx + 1, my + 1);
    char c;

    if (tile->flagged) {
      c = 'F';
    } else if (tile->mine) {
      c = 'M';
    } else if (tile->opened) {
      c = 'O';
    } else {
      c = 'A';
    }

    mvaddch(my * 2 + 1 + y, mx * 4 + 2 + x, c);
  }
}

void generate_board(struct Options options, uint32_t y, uint32_t x) {
  /*
    fl = first line
    l1 = vertical line
    l2 = line with spaces
    ll = last line
  */
  mvprintw(y - 1, x, "Remaining flags: %-3u", options.mine_count);

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
  uint32_t remaining_flags = options.mine_count;

  while (true) {
    const int32_t c = getch();

    switch (c) {
      case 'b':
        clear();
        free_tiles(tiles);
        init_menu();
        return;

      case KEY_RESIZE:
        clear();
        start_y = (getmaxy(stdscr) - (options.minefield_len * 2 + 1)) / 2;
        start_x = (getmaxx(stdscr) - (options.minefield_len * 4 + 1)) / 2;
        generate_board(options, start_y, start_x);

        if (placed_mines) {
          redraw_board(tiles, start_y, start_x, remaining_flags);
        }

        break;
      
      case KEY_MOUSE: {
        MEVENT event;

        if (getmouse(&event) == OK) {
          const uint32_t _y = event.y - start_y;
          const uint32_t _x = event.x - start_x;

          const pos_t click_at = {
            .x = (_x % 4 == 0) ? 0 : ((_x / 4) + 1),
            .y = (_y % 2 == 0) ? 0 : ((_y / 2) + 1)
          };

          switch (event.bstate) {
            case BUTTON1_CLICKED:
              if (!placed_mines) {
                struct Offset offset = {
                  .bottom = 1,
                  .top = 1,
                  .left = 1,
                  .right = 1
                };

                placed_mines = place_mines(tiles, options.mine_count, click_at, offset);
                redraw_board(tiles, start_y, start_x, remaining_flags);
              }

              break;

            case BUTTON3_CLICKED: {
              if (click_at.x != 0 && click_at.y != 0) {
                tile_t *tile = get_tile(tiles, click_at.x, click_at.y);

                if (tile->flagged) {
                  tile->flagged = false;
                  ++remaining_flags;
                  redraw_board(tiles, start_y, start_x, remaining_flags);
                } else if (!tile->opened && remaining_flags != 0) {
                  tile->flagged = true;
                  --remaining_flags;
                  redraw_board(tiles, start_y, start_x, remaining_flags);
                }
              }

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
