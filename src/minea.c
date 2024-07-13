#include "../minea.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int32_t argc, char *argv[]) {
  if (argc == 2) {
    char *option = argv[1];

    if (streq(option, "--version") || streq(option, "-v")) {
      puts("minea v1, https://github.com/aloima/minea");
      exit(EXIT_SUCCESS);
    } else if (streq(option, "--help") || streq(option, "-h")) {
      printf((
        "%s [<option>]\n\n"
        "-h, --help    - Writes out this page\n"
        "-v, --version - Writes out version\n"
      ), argv[0]);

      exit(EXIT_SUCCESS);
    } else {
      puts("minea: invalid option, use --help/-h");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}
