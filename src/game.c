#include "../minea.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

tile_t *get_tile(struct Tiles tiles, uint32_t x, uint32_t y) {
  return &tiles.data[x - 1 + (y - 1) * tiles.len];
}

void free_tiles(struct Tiles tiles) {
  free(tiles.data);
}

struct Tiles generate_empty_tiles(uint32_t len) {
  const uint32_t size = len * len;

  struct Tiles tiles = {
    .data = malloc(size * sizeof(tile_t)),
    .size = size,
    .len = len
  };

  for (uint32_t i = 0; i < size; ++i) {
    const uint32_t j = (i + 1);
    const uint32_t _x = (j % len);
    const uint32_t _y = (j / len);

    tiles.data[i] = (tile_t) {
      .position = {
        .x = _x == 0 ? len : _x,
        .y = _x == 0 ? _y : _y + 1
      },
      .flagged = false,
      .mine = false,
      .opened = false,
      .value = 0
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
            filled_tiles[i] = (pos_t) {x, y};
            break;
          }
        } else {
          filled_tiles[i] = (pos_t) {x, y};
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

bool open_area(struct Tiles tiles, tile_t *tile, bool direct) {
  uint32_t count = get_mine_count(tiles, tile->position);

  if (!tile->opened && !tile->flagged) {
    if (!tile->mine) {
      tile->opened = true;

      if (count != 0) {
        tile->value = count;
        return true;
      }
    } else if (direct) {
      return false;
    }
  }

  if (tile->position.x > 1) {
    tile_t *look = get_tile(tiles, tile->position.x - 1, tile->position.y);
    if (!look->opened) open_area(tiles, look, false);
  }

  if (tile->position.y > 1) {
    tile_t *look = get_tile(tiles, tile->position.x, tile->position.y - 1);
    if (!look->opened) open_area(tiles, look, false);
  }

  if (tile->position.x < tiles.len) {
    tile_t *look = get_tile(tiles, tile->position.x + 1, tile->position.y);
    if (!look->opened) open_area(tiles, look, false);
  }

  if (tile->position.y < tiles.len) {
    tile_t *look = get_tile(tiles, tile->position.x, tile->position.y + 1);
    if (!look->opened) open_area(tiles, look, false);
  }

  return true;
}

uint32_t get_mine_count(struct Tiles tiles, pos_t pos) {
  uint32_t count = 0;

  // row 1
  if (pos.y > 1) {
    if (get_tile(tiles, pos.x, pos.y - 1)->mine) count += 1;
    if (pos.x > 1 && get_tile(tiles, pos.x - 1, pos.y - 1)->mine) count += 1;
    if (pos.x < tiles.len && get_tile(tiles, pos.x + 1, pos.y - 1)->mine) count += 1;
  }

  // row 2
  if (pos.x > 1 && get_tile(tiles, pos.x - 1, pos.y)->mine) count += 1;
  if (pos.x < tiles.len && get_tile(tiles, pos.x + 1, pos.y)->mine) count += 1;

  // row 3
  if (pos.y < tiles.len) {
    if (get_tile(tiles, pos.x, pos.y + 1)->mine) count += 1;
    if (pos.x > 1 && get_tile(tiles, pos.x - 1, pos.y + 1)->mine) count += 1;
    if (pos.x < tiles.len && get_tile(tiles, pos.x + 1, pos.y + 1)->mine) count += 1;
  }

  return count;
}
