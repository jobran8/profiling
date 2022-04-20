/*
 * um_instructions.h
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Provides an interface for the declaration of all functions dealing with
 * instructions executed by the registers.
*/

#ifndef UM_INSTRUCTIONS_
#define UM_INSTRUCTIONS_

#include "um_segments.h"

/* Takes in inputted memory and stops all computations associated with it */
void halt(Memory_T memory);

/* Takes in inputted array of registers and a value to load into register A */
void load_value(uint32_t *registers, unsigned A, unsigned B,
                            unsigned C, uint32_t value);

/*
 * Takes in inputted array of registers and moves contents of register A to B
 *      if register C is empty.
*/
void conditional_move(uint32_t *registers, unsigned A,
                                    unsigned B, unsigned C);

/*
 * Takes in inputted array of registers and memory to duplicate a segment in
 *      memory and return it.
*/
int load_program(uint32_t *registers, unsigned A, unsigned B, unsigned C,
                 Memory_T memory);

/* Takes in inputted array of registers and memory to load words from memory*/
void segmented_load(uint32_t *registers, unsigned A, unsigned B,
                                  unsigned C, Memory_T memory);

/* Takes in inputted array of registers and memory to store words in memory*/
void segmented_store(uint32_t *registers, unsigned A, unsigned B,
                                   unsigned C, Memory_T memory);

/* Takes in inputted array of registers and memory to activate new segment */
void map_segment(uint32_t *registers, unsigned A, unsigned B,
                               unsigned C, Memory_T memory);

/* Takes in inputted array of registers and memory to deactivate a segment */
void unmap_segment(uint32_t *registers, unsigned A, unsigned B,
                                unsigned C, Memory_T memory);

/* Takes in inputted array of registers and adds two values */
void addition(uint32_t *registers, unsigned A, unsigned B,
                            unsigned C);

/* Takes in inputted array of registers and multiplies two values */
void multiplication(uint32_t *registers, unsigned A, unsigned B,
                                  unsigned C);

/* Takes in inputted array of registers and divides two values */
void division(uint32_t *registers, unsigned A, unsigned B,
                            unsigned C);

/* Takes in inputted array of registers and nands two values */
void bitwise_nand(uint32_t *registers, unsigned A, unsigned B,
                                unsigned C);

/* Takes in inputted array of registers and outputs value in register C */
void output(uint32_t *registers, unsigned A, unsigned B,
                          unsigned C);

/* Takes in inputted array of registers and loads inputted value in C */
void input(uint32_t *registers, unsigned A, unsigned B,
                         unsigned C);

#undef T
#endif
