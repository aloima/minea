#include <stdint.h>
#include <string.h>
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

  tile_t *get_tile(struct Tiles tiles, uint32_t x, uint32_t y);
  struct Tiles generate_empty_tiles(uint32_t size);
  void place_mines(struct Tiles tiles, uint32_t count, pos_t center, struct Offset offset);
#endif
