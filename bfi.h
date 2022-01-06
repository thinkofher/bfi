#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef _BFI_H_
#define _BFI_H_

typedef enum {
  /*
   * Any character other than the rest of tokens.
   * Can be part of a comment.
   */
  bfi_token_none = 0,
  bfi_token_incr_ptr = '>',
  bfi_token_decr_ptr = '<',
  bfi_token_incr_data = '+',
  bfi_token_decr_data = '-',
  bfi_token_output_data = '.',
  bfi_token_input_data = ',',
  bfi_token_jump_fwr = '[',
  bfi_token_jump_bck = ']'
} bfi_token_t;

/*
 * bfi_tokenize translates given ASCII character into brainfuck
 * token.
 */
bfi_token_t bfi_tokenize(char);

/*
 * bfi_tokenize_file returns stbds array with tokens
 * read from given file.
 */
bfi_token_t *bfi_tokenize_file(FILE *f);

#define BFI_MEMORY_SIZE 30000

typedef struct {
  uint8_t memory[BFI_MEMORY_SIZE];
  uint32_t mem_ptr;
} bfi_memory_layout_t;

/*
 * bfi_new_memory returns fresh memory with safe zero values.
 */
bfi_memory_layout_t bfi_new_memory();

typedef enum {
  bfi_instr_unknown = 0,
  bfi_instr_diff_ptr = 1,
  bfi_instr_diff_data = 2,
  bfi_instr_out = 3,
  bfi_instr_in = 4,
  bfi_instr_jump_fwr = 5,
  bfi_instr_jump_bck = 6,
} bfi_instruction_type_t;

typedef struct {
  bfi_instruction_type_t type;
  union {
    int32_t diff;
    struct {
      uint32_t begin_instr_ptr;
      uint32_t end_instr_ptr;
    } jump;
  } data;
} bfi_instruction_t;

bfi_instruction_t bfi_new_diff_ptr_instr(int32_t diff);

bfi_instruction_t bfi_new_diff_data_instr(int32_t diff);

bfi_instruction_t bfi_new_instr_out(void);

bfi_instruction_t bfi_new_instr_in(void);

bfi_instruction_t bfi_new_instr_jump_fwr(uint32_t begin, uint32_t end);

bfi_instruction_t bfi_new_instr_jump_bck(uint32_t begin, uint32_t end);

bfi_instruction_t *bfi_instr_lexing(bfi_token_t *stb_arr);

/*
 * Run set of instructions against memory layout.
 */
void bfi_run_instrs(bfi_memory_layout_t *mem, bfi_instruction_t *program);

/*
 * Outputs information about given instruction to provided file handle.
 */
void bfi_instr_show(FILE *f, bfi_instruction_t *instr);

#endif
