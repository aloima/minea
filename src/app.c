#include "../minea.h"

#include <ncurses.h>
#include <stdint.h>

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

  const uint32_t lines = getmaxy(stdscr);
  const uint32_t cols = getmaxx(stdscr);

  const uint32_t start_y = (lines - (options.minefield_len * 2 + 1)) / 2;
  const uint32_t start_x = (cols - (options.minefield_len * 4 + 1)) / 2;

  generate_board(options, start_y, start_x);

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
        refresh();
        init_game();
        return;
    }
  }
}

void init_app() {
  initscr();
  cbreak();
  noecho();

  mousemask(ALL_MOUSE_EVENTS, NULL);
  init_menu();

  endwin();
}
