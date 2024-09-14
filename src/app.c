#include "../minea.h"

#include <stdint.h>
#include <string.h>

#include <ncurses.h>

void redraw_board(struct Tiles tiles, uint32_t y, uint32_t x, uint32_t remaining_flags) {
  mvprintw(y - 1, x, "Remaining flags: %-3u", remaining_flags);

  for (uint32_t i = 0; i < tiles.size; ++i) {
    const uint32_t mx = i % 9;
    const uint32_t my = i / 9;

    tile_t *tile = get_tile(tiles, mx + 1, my + 1);

    if (tile->flagged) {
      attron(COLOR_PAIR(1));
      mvaddch(my * 2 + 1 + y, mx * 4 + 2 + x, 'F');
      attroff(COLOR_PAIR(1));
    } else if (tile->mine) {
      mvaddch(my * 2 + 1 + y, mx * 4 + 2 + x, 'M');
    } else if (tile->opened) {
      if (tile->value != 0) {
        attron(COLOR_PAIR(2));
        mvaddch(my * 2 + 1 + y, mx * 4 + 2 + x, tile->value + 48);
        attroff(COLOR_PAIR(2));
      } else {
        mvaddch(my * 2 + 1 + y, mx * 4 + 2 + x, 'X');
      }
    } else {
      mvaddch(my * 2 + 1 + y, mx * 4 + 2 + x, ' ');
    }
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

  uint32_t board_height = (options.minefield_len * 2 + 1);
  uint32_t board_width = (options.minefield_len * 4 + 1);
  uint32_t window_height = getmaxy(stdscr);
  uint32_t window_width = getmaxx(stdscr);

  if ((board_height + 1) > window_height || board_width > window_width) {
    mvaddstr(window_height / 2, (window_width - 26) / 2, "Window size is not enough.");
    refresh();

    while (true) {
      if (getch() == KEY_RESIZE) {
        clear();

        window_height = getmaxy(stdscr);
        window_width = getmaxx(stdscr);

        if ((board_height + 1) > window_height || board_width > window_width) {
          mvaddstr(window_height / 2, (window_width - 26) / 2, "Window size is not enough.");
          refresh();
        } else {
          init_game();
          break;
        }
      }
    }
  } else {
    uint32_t start_y = (window_height - board_height) / 2;
    uint32_t start_x = (window_width - board_width) / 2;

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

          window_height = getmaxy(stdscr);
          window_width = getmaxx(stdscr);

          if ((board_height + 1) > window_height || board_width > window_width) {
            mvaddstr(window_height / 2, (window_width - 26) / 2, "Window size is not enough.");
            refresh();
          } else {
            start_y = (window_height - board_height) / 2;
            start_x = (window_width - board_width) / 2;
            generate_board(options, start_y, start_x);

            if (placed_mines) {
              redraw_board(tiles, start_y, start_x, remaining_flags);
            }
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
                if (click_at.x != 0 && click_at.y != 0 && click_at.x <= tiles.len && click_at.y <= tiles.len) {
                  if (!placed_mines) {
                    struct Offset offset = {
                      .bottom = 1,
                      .top = 1,
                      .left = 1,
                      .right = 1
                    };

                    placed_mines = true;
                    place_mines(tiles, options.mine_count, click_at, offset);
                    redraw_board(tiles, start_y, start_x, remaining_flags);
                  } else {
                    tile_t *tile = get_tile(tiles, click_at.x, click_at.y);

                    if (!open_area(tiles, tile, true)) {
                      lose_page(tiles);
                      return;
                    } else {
                      redraw_board(tiles, start_y, start_x, remaining_flags);
                    }
                  }
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

                    if (get_fully_flagged(tiles)) {
                      win_page(tiles);
                      return;
                    } else {
                      redraw_board(tiles, start_y, start_x, remaining_flags);
                    }
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
}

void init_app() {
  initscr();
  cbreak();
  noecho();
  notimeout(stdscr, true);
  curs_set(0);

  start_color();
  use_default_colors();
  init_pair(1, COLOR_RED, -1);
  init_pair(2, COLOR_BLUE, -1);

  mousemask(ALL_MOUSE_EVENTS, NULL);
  init_menu();

  endwin();
}
