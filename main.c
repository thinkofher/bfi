#include <inttypes.h>
#include <stdio.h>

#include "bfi.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES
#include "stb_ds.h"

#define PROJECT_NAME "bfi"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "provide filename with brainfuck source code\n");
    return 1;
  }

  FILE *src = fopen(argv[1], "r");

  /*
   * Read tokens from file.
   */
  bfi_token_t *tokens = bfi_tokenize_file(src);

  if (tokens == NULL) {
    fprintf(stderr, "failed to tokenize file\n");
    return 1;
  }

  /*
   * Perform lexical analysis on read tokens.
   */
  bfi_instruction_t *instrs = bfi_instr_lexing(tokens);
  if (instrs == NULL) {
    fprintf(stderr, "failed to perform lexical analysis on file\n");
    stbds_arrfree(tokens);
    return 1;
  }

  /*
   * TODO(thinkofher): Allow user show results of lexical analysis of source
   * code.
   *
   * for (size_t i = 0; i < stbds_arrlenu(instrs); i++) {
   *   printf("%lu ", i);
   *   bfi_instr_show(stdout, instrs + i);
   *   putchar('\n');
   * }
   */

  /*
   * Allocate fixed memory layout for running programs.
   */
  bfi_memory_layout_t mem = bfi_new_memory();

  /*
   * Run tokenized program against freshly allocated memory.
   */
  bfi_run_instrs(&mem, instrs);

  stbds_arrfree(instrs);
  stbds_arrfree(tokens);
  return 0;
}
