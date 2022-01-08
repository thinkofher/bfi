#include <stdio.h>

#include "bfi.h"

#ifndef _REPL_H_
#define _REPL_H

/*
 * Runs REPL session until the user provides EOF character.
 */
void bfi_run_repl(bfi_memory_layout_t *mem);

#endif
