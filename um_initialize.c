/*
 * um_initialize.c
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Implementation of the um_initialize.h interface. Contains function that
 * creates new instance of a Memory_T and maps that to the zero segment. Then,
 * gets each individual instruction and stores in the zero segment.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bitpack.h"
#include "um_segments.h"
#include "um_initialize.h"

/* initialize
 *
 *      Purpose: Initializes program and loads instructions in segment 0.
 *
 *   Parameters: File holding a list of instruction, number of instructions.
 *
 *      Returns: Filled program memory (Memory_T).
 *
 * Expectations: None.
*/
extern Memory_T initialize(FILE *fp, uint32_t num_instructions)
{
    /* create new segment of length equal to the number of instructions */
    Memory_T memory = segment_new();
    uint32_t segment_0 = segment_map(memory, num_instructions);

    /* loops through the number of instructions in segment 0 */
    for (int i = 0; i < (int)num_instructions; i++) {
        uint32_t instruction = 0;

        /* loops for every first character of each set of 4 */
        for (int j = sizeof(uint32_t) - 1; j >= 0; j--) {
            int first_byte = getc(fp);
            instruction = Bitpack_newu(instruction, 8, (j * 8),
                                       (uint64_t)first_byte);
        }

        /* store each instruction in correct placement of segment 0 sequence */
        segment_store(memory, segment_0, i, instruction);
    }

    return memory;
}
