#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "repl.h"
#include "stb_ds.h"

static char *read_line(FILE *in) {
  char c;
  char *string = malloc(sizeof(char));

  string[0] = '\0';

  for (int i = 0; (c = getc(in)) != '\n'; i++) {
    string = realloc(string, (i + 2) * sizeof(char));
    string[i] = c;
    string[i + 1] = '\0';
  }

  return string;
}

static char *_lower_str(char *str) {
  for (size_t i = 0; i < strlen(str); i++)
    str[i] = tolower(str[i]);
  return str;
}

void bfi_run_repl(bfi_memory_layout_t *mem) {
  char *repl_input = NULL;
  bfi_token_t *tokens = NULL;
  bfi_instruction_t *instructions = NULL;

  fprintf(stdout, "Brainfuck Interpreter written by <thinkofher> @ 2021\n");
  fprintf(stdout, "You're using it at your own risk.\n");
  fprintf(stdout, "See LICENSE file for more information.\n");
  fprintf(stdout,
          "Enter brainfuck code or type \"exit\" to leave the program.\n\n");

  for (;;) {
    fprintf(stdout, "[0x%04x] $ ", mem->mem_ptr);

    repl_input = read_line(stdin);

    if (strcmp("exit", _lower_str(repl_input)) == 0) {
      free(repl_input);

      if (tokens != NULL)
        stbds_arrfree(tokens);

      if (instructions != NULL)
        stbds_arrfree(instructions);

      return;
    }

    tokens = bfi_tokenize_str(repl_input);

    instructions = bfi_instr_lexing(tokens);

    bfi_run_instrs(mem, instructions);

    stbds_arrfree(tokens);
    tokens = NULL;

    stbds_arrfree(instructions);
    instructions = NULL;

    free(repl_input);
    repl_input = NULL;
  }
}
