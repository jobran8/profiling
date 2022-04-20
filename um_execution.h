/*
 * um_execution.h
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Provides an interface for the declaration of a function to execute the
 * final program. Contains pointer to an incomplete struct to keep track of
 * individual functions.
*/

#ifndef UM_EXECUTION_
#define UM_EXECUTION_

#include <stdint.h>
#include "um_segments.h"

#define T Instruction_T
typedef struct T *T; /* pointer to an incomplete struct */

/*
 * Takes in inputted program memory, active registers, and a program counter
 *      to execute a chain of instructions.
 */
extern void execute(Memory_T program, uint32_t *registers, int *prog_counter);

#undef T
#endif
