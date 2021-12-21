#include <inttypes.h>
#include <stdio.h>

#include "bfi.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES
#include "stb_ds.h"

#define PROJECT_NAME "bfi"

int main(void) {
  bfi_token_t *tokens = bfi_tokenize_file(stdin);

  for (size_t i = 0; i < stbds_arrlenu(tokens); i++) {
    putchar(tokens[i]);
  }

  stbds_arrfree(tokens);
  return 0;
}
