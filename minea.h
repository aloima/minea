#include <ncurses.h>
#include <ncurses/menu.h>

#include <stdint.h>
#include <string.h> // IWYU pragma: keep
#include <stdbool.h>

#ifndef MINEA_H_
  #define MINEA_H_

  #define streq(s1, s2) (strcmp((s1), (s2)) == 0)

  typedef struct {
    uint32_t x;
    uint32_t y;
  } pos_t;

  typedef struct {
    bool flagged;
    bool mine;
    pos_t position;
    bool opened;
    uint32_t value;
  } tile_t;

  struct Tiles {
    tile_t *data;
    uint32_t size;
    uint32_t len; // (len * len) = size, represents row and column length
  };

  struct Offset {
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;
  };

  struct Options {
    uint32_t minefield_len;
    uint32_t mine_count;
  };

  tile_t *get_tile(struct Tiles tiles, uint32_t x, uint32_t y);
  void free_tiles(struct Tiles tiles);
  struct Tiles generate_empty_tiles(uint32_t len);
  void place_mines(struct Tiles tiles, uint32_t count, pos_t center, struct Offset offset);
  uint32_t get_mine_count(struct Tiles tiles, pos_t pos);
  void open_area(struct Tiles tiles, tile_t *tile, bool direct);

  void init_app();
  void init_game();
  void generate_board(struct Options options, uint32_t y, uint32_t x);

  void init_menu();
  void clear_menu(ITEM **items, MENU *menu, WINDOW *menu_win, WINDOW *menu_derwin);
  void handle_menu_enter(int index, ITEM **items, MENU *menu, WINDOW *menu_win, WINDOW *menu_derwin);

  void about_page();
  void options_page();

  struct Options get_options();
  void set_options(struct Options input);
  void read_options();
#endif
