#include <stdio.h>
#include <string.h>

#include "bfi.h"

#define STBDS_NO_SHORT_NAMES
#include "stb_ds.h"

#define __last_element(arr) ((arr) + stbds_arrlenu((arr)) - (1))

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

bfi_token_t *bfi_tokenize_str(const char *str) {
  bfi_token_t *res = NULL;

  bfi_token_t t = bfi_token_none;
  for (size_t i = 0; i < strlen(str); i++) {
    t = bfi_tokenize(str[i]);
    if (t != bfi_token_none)
      stbds_arrput(res, t);
  }

  return res;
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

bfi_instruction_t bfi_new_diff_data_instr(int32_t diff) {
  return (bfi_instruction_t){
      .type = bfi_instr_diff_data,
      .data.diff = diff,
  };
}

bfi_instruction_t bfi_new_instr_out(void) {
  return (bfi_instruction_t){
      .type = bfi_instr_out,
  };
}

bfi_instruction_t bfi_new_instr_in(void) {
  return (bfi_instruction_t){
      .type = bfi_instr_in,
  };
}

bfi_instruction_t bfi_new_instr_jump_fwr(uint32_t begin, uint32_t end) {
  return (bfi_instruction_t){
      .type = bfi_instr_jump_fwr,
      .data.jump.begin_instr_ptr = begin,
      .data.jump.end_instr_ptr = end,
  };
}

bfi_instruction_t bfi_new_instr_jump_bck(uint32_t begin, uint32_t end) {
  return (bfi_instruction_t){
      .type = bfi_instr_jump_bck,
      .data.jump.begin_instr_ptr = begin,
      .data.jump.end_instr_ptr = end,
  };
}

bfi_instruction_t *bfi_instr_lexing(bfi_token_t *tokens) {
  bfi_instruction_t *instructions = NULL;
  bfi_instruction_t *jumps_stack = NULL;

  bfi_instruction_t *previous_instr = NULL;

  /*
   * For new jumps.
   */
  bfi_instruction_t new_instr;

  /*
   * For previous jumps from stack.
   */
  bfi_instruction_t last_jump;

  bfi_token_t curr_token = bfi_token_none;
  for (size_t i = 0; i < stbds_arrlenu(tokens); i++) {
    curr_token = tokens[i];

    switch (curr_token) {
    case bfi_token_none:
      /*
       * This scenario shouldn't happen, because non-token
       * characters are being ignored during tokenize process.
       */
      continue;
    case bfi_token_incr_ptr:
      if (instructions == NULL) {
        stbds_arrput(instructions, bfi_new_diff_ptr_instr(1));

        /*
         * First instruction, we can continue to the next token.
         */
        continue;
      }

      previous_instr = __last_element(instructions);

      if (previous_instr->type == bfi_instr_diff_ptr) {
        /*
         * Last instruction is the same, so we can just change data diff
         * field.
         */
        previous_instr->data.diff += 1;
      } else {
        /*
         * Last instruction is different, so we can push new to the array.
         */
        stbds_arrput(instructions, bfi_new_diff_ptr_instr(1));
      }
      break;
    case bfi_token_decr_ptr:
      if (instructions == NULL) {
        stbds_arrput(instructions, bfi_new_diff_ptr_instr(-1));
        continue;
      }

      previous_instr = __last_element(instructions);

      if (previous_instr->type == bfi_instr_diff_ptr) {
        previous_instr->data.diff -= 1;
      } else {
        stbds_arrput(instructions, bfi_new_diff_ptr_instr(-1));
      }

      break;
    case bfi_token_incr_data:
      if (instructions == NULL) {
        stbds_arrput(instructions, bfi_new_diff_data_instr(1));
        continue;
      }

      previous_instr = __last_element(instructions);

      if (previous_instr->type == bfi_instr_diff_data) {
        previous_instr->data.diff += 1;
      } else {
        stbds_arrput(instructions, bfi_new_diff_data_instr(1));
      }

      break;
    case bfi_token_decr_data:
      if (instructions == NULL) {
        stbds_arrput(instructions, bfi_new_diff_data_instr(-1));
        continue;
      }

      previous_instr = __last_element(instructions);

      if (previous_instr->type == bfi_instr_diff_data) {
        previous_instr->data.diff -= 1;
      } else {
        stbds_arrput(instructions, bfi_new_diff_data_instr(-1));
      }

      break;
    case bfi_token_input_data:
      stbds_arrput(instructions, bfi_new_instr_in());
      break;
    case bfi_token_output_data:
      stbds_arrput(instructions, bfi_new_instr_out());
      break;
    case bfi_token_jump_fwr:
      new_instr = bfi_new_instr_jump_fwr(stbds_arrlenu(instructions), 0);
      stbds_arrput(jumps_stack, new_instr);
      stbds_arrput(instructions, new_instr);
      break;
    case bfi_token_jump_bck:
      new_instr = bfi_new_instr_jump_bck(0, stbds_arrlenu(instructions));
      last_jump = stbds_arrpop(jumps_stack);

      if (last_jump.type != bfi_instr_jump_fwr) {
        /*
         * Invalid jump instruction. jmp_bck can happen only if
         * there was any jmp_fwr declared previously.
         */
        stbds_arrfree(instructions);
        stbds_arrfree(jumps_stack);
        return NULL;
      }

      /*
       * Update jump values for both instructions.
       */
      instructions[last_jump.data.jump.begin_instr_ptr]
          .data.jump.end_instr_ptr = new_instr.data.jump.end_instr_ptr;
      new_instr.data.jump.begin_instr_ptr = last_jump.data.jump.begin_instr_ptr;

      /*
       * Push updated instruction to array.
       */
      stbds_arrput(instructions, new_instr);
      break;
    }
  }

  if (stbds_arrlenu(jumps_stack) > 0) {
    /*
     * Jumps stack has to be empty at the end of the program.
     * If it is not, it means that there are some extra jump
     * instructions declared.
     */
    stbds_arrfree(jumps_stack);
    stbds_arrfree(instructions);
    return NULL;
  }

  stbds_arrfree(jumps_stack);
  return instructions;
}

void bfi_run_instrs(bfi_memory_layout_t *mem, bfi_instruction_t *program) {
  if (program == NULL || stbds_arrlenu(program) == 0) {
    return;
  }

  uint32_t instr_ptr = 0;
  for (;;) {
    if (instr_ptr >= stbds_arrlenu(program)) {
      return;
    }

    switch (program[instr_ptr].type) {
    case bfi_instr_unknown:
      /*
       * This instruction shouldn't be in the program.
       */
      return;
    case bfi_instr_diff_ptr:
      mem->mem_ptr += program[instr_ptr].data.diff;
      break;
    case bfi_instr_diff_data:
      mem->memory[mem->mem_ptr] += program[instr_ptr].data.diff;
      break;
    case bfi_instr_out:
      putchar(mem->memory[mem->mem_ptr]);
      break;
    case bfi_instr_in:
      mem->memory[mem->mem_ptr] = getchar();
      break;
    case bfi_instr_jump_fwr:
      if (mem->memory[mem->mem_ptr] == 0) {
        instr_ptr = program[instr_ptr].data.jump.end_instr_ptr + 1;
        continue;
      }
      break;
    case bfi_instr_jump_bck:
      if (mem->memory[mem->mem_ptr] != 0) {
        instr_ptr = program[instr_ptr].data.jump.begin_instr_ptr + 1;
        continue;
      }
      break;
    }

    instr_ptr++;
  }
}

void bfi_instr_show(FILE *f, bfi_instruction_t *instr) {
  if (instr == NULL || f == NULL) {
    return;
  }

  switch (instr->type) {
  case bfi_instr_unknown:
    fprintf(f, "bfi_instr_unknown");
    break;
  case bfi_instr_diff_ptr:
    fprintf(f, "bfi_instr_diff_ptr {diff=%" PRId32 "}", instr->data.diff);
    break;
  case bfi_instr_diff_data:
    fprintf(f, "bfi_instr_diff_data {diff=%" PRId32 "}", instr->data.diff);
    break;
  case bfi_instr_out:
    fprintf(f, "bfi_instr_out");
    break;
  case bfi_instr_in:
    fprintf(f, "bfi_instr_in");
    break;
  case bfi_instr_jump_fwr:
    fprintf(f, "bfi_instr_jump_fwr {begin=%" PRIu32 " end=%" PRIu32 "}",
            instr->data.jump.begin_instr_ptr, instr->data.jump.end_instr_ptr);
    break;
  case bfi_instr_jump_bck:
    fprintf(f, "bfi_instr_jump_bck {begin=%" PRIu32 " end=%" PRIu32 "}",
            instr->data.jump.begin_instr_ptr, instr->data.jump.end_instr_ptr);
    break;
  }

  return;
}
