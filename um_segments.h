/*
 * um_segments.h
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Provides an interface for the creation of "main memory" used in the UM
 * emulator. Contains a pointer to an incomplete Memory_T struct and declares
 * all functions relaing to the use of this segmented memory.
*/

#ifndef UM_SEGMENTS_
#define UM_SEGMENTS_

#include <stdint.h>

#define T Memory_T
typedef struct T *T; /* pointer to an incomplete struct */

/* Outputs a newly created instance of main memory */
extern T segment_new();

/* Takes in inputted memory and frees all associated memory */
extern void segment_free(T memory);

/*
 * Takes in inputted memory, an offset, and an id and returns a word at that
 *      spot in memory.
 */
extern uint32_t segment_load(T memory, int id, int offset);

/*
 * Takes in inputted memory, an offset, an id, and a word to store in that spot
 *      in main memory.
 */
extern void segment_store(T memory, int id, int offset, uint32_t word);

/*
 * Takes in inputted memory and an index to load a new program into the
 *      specified segment index.
 */
extern void segment_load_program(T memory, int id);

/*
 * Takes in inputted memory and a size and returns a new segment in memory
 *      with size equal to that passed in.
 */
extern uint32_t segment_map(T memory, int size);

/*
 * Takes in inputted memory and an id and unmaps and frees a segment in main
 *      memory.
 */
extern void segment_unmap(T memory, int id);

#undef T
#endif
