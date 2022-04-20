/*
 * um_initialize.h
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Provides an interface for the declaration of all functions dealing with
 * initializing and loading the program.
*/

#ifndef UM_INITIALIZE_
#define UM_INITIALIZE_

#include <stdint.h>
#include "um_segments.h"

/*
 * Takes in inputted file and the number of instructions to execute, and
 *      returns main memory with stored instructions.
 */
extern Memory_T initialize(FILE *fp, uint32_t num_instructions);

#endif
