#include <inttypes.h>
#include <stdio.h>

#include "bfi.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES
#include "stb_ds.h"

#define PROJECT_NAME "bfi"

int main(void) {
  bfi_token_t *tokens = bfi_tokenize_file(stdin);

  if (tokens == NULL) {
    fprintf(stderr, "failed to tokenize file\n");
    return 0;
  }

  bfi_instruction_t *instrs = bfi_instr_lexing(tokens);
  if (instrs == NULL) {
    fprintf(stderr, "failed to perform lexical analysis on file\n");
    stbds_arrfree(tokens);
    return 0;
  }

  for (size_t i = 0; i < stbds_arrlenu(instrs); i++) {
    printf("%lu ", i);
    bfi_instr_show(stdout, instrs + i);
    putchar('\n');
  }

  stbds_arrfree(instrs);
  stbds_arrfree(tokens);
  return 0;
}
