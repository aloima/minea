#include "../minea.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

tile_t *get_tile(struct Tiles tiles, uint32_t x, uint32_t y) {
  return &tiles.data[x - 1 + (y - 1) * tiles.len];
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

void place_mines(struct Tiles tiles, uint32_t count, pos_t center, struct Offset offset) {
  const uint32_t area_left_at = center.x - offset.left;
  const uint32_t area_right_at = center.x + offset.right;
  const uint32_t area_top_at = center.y - offset.top;
  const uint32_t area_bottom_at = center.x + offset.bottom;

  pos_t *filled_tiles = malloc(count * sizeof(pos_t));

  for (uint32_t i = 0; i < count; ++i) {
    while (true) {
      bool set = false; 
      const uint32_t x = (rand() % tiles.len) + 1;
      const uint32_t y = (rand() % tiles.len) + 1;

      if ((x < area_left_at || x > area_right_at) && (y < area_top_at || y > area_bottom_at)) {
        if (i != 0) {
          for (uint32_t j = 0; j < i; ++j) {
            if (x == filled_tiles[j].x && y == filled_tiles[j].y) {
              set = true;
              break;
            }
          }

          if (!set) {
            filled_tiles[i] = (pos_t) {
              .x = x,
              .y = y
            };

            break;
          }
        } else {
          filled_tiles[i] = (pos_t) {
            .x = x,
            .y = y
          };

          break;
        }
      }
    }
  }

  for (uint32_t i = 0; i < count; ++i) {
    const pos_t position = filled_tiles[i];
    tile_t *tile = get_tile(tiles, position.x, position.y);
    tile->mine = true;
  }

  free(filled_tiles);
}
