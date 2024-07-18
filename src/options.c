#include "../minea.h"

#include <stdio.h>
#include <stdlib.h>

static struct Options options;

struct Options get_options() {
  return options;
}

void set_options(struct Options input) {
  memcpy(&options, &input, sizeof(struct Options));
}

void read_options() {
  const char *home_dir = getenv("HOME");
  char path[1024];
  sprintf(path, "%s/.config/minea.conf", home_dir);

  FILE *file = fopen(path, "r");

  if (file == NULL) {
    options = (struct Options) {
      .minefield_len = 9,
      .mine_count = 16
    };
  }
}
