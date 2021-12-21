#include "bfi.h"
#include "stb_ds.h"
#include <stdio.h>

bfi_token_t bfi_tokenize(char c) {
  switch (c) {
  case bfi_token_incr_ptr:
  case bfi_token_decr_ptr:
  case bfi_token_incr_data:
  case bfi_token_decr_data:
  case bfi_token_output_data:
  case bfi_token_input_data:
  case bfi_token_jump_fwr:
  case bfi_token_jump_bck:
    return c;
  default:
    return bfi_token_none;
  }
}

bfi_token_t *bfi_tokenize_file(FILE *f) {
  bfi_token_t *res = NULL;

  char c = 0;
  bfi_token_t t = bfi_token_none;
  while ((c = fgetc(f)) != EOF) {
    t = bfi_tokenize(c);
    if (t != bfi_token_none) {
      stbds_arrput(res, t);
    }
  }

  return res;
}

bfi_memory_layout_t bfi_new_memory() {
  return (bfi_memory_layout_t){
      .memory = {0},
      .mem_ptr = 0,
  };
}

bfi_instruction_t bfi_new_diff_ptr_instr(int32_t diff) {
  return (bfi_instruction_t){
      .type = bfi_instr_diff_ptr,
      .data.diff = diff,
  };
}
