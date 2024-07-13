#include "../minea.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

tile_t *get_tile(struct Tiles tiles, uint32_t size, uint32_t x, uint32_t y) {
  return &tiles.data[x - 1 + (y - 1) * ((uint32_t) sqrt(size))];
}

struct Tiles generate_empty_tiles(uint32_t size) {
  struct Tiles tiles = {
    .data = malloc(size * sizeof(tile_t)),
    .size = size,
    .len = sqrt(size)
  };

  for (uint32_t i = 0; i < size; ++i) {
    const uint32_t j = (i + 1);

    tiles.data[i] = (tile_t) {
      .position = {
        .x = (j % tiles.len),
        .y = (j / tiles.len)
      },
      .flagged = false,
      .mine = false
    };
  }

  return tiles;
}
