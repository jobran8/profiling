/*
 * um_segments.c
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * The implementation for the "main memory" used in the UM emulator. Uses a
 * sequence of arrays where the sequence in the main memory and the arrays
 * are segments of memory that can be used.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <seq.h> /* Hanson's sequence library */
#include <uarray.h>

#include "um_segments.h"
#define T Memory_T

/* macros ================================================================== */
#define SEQ_SEGMENT 1000
#define SEQ_UNMAPPED 500

/* struct definition ======================================================= */
struct T {
    Seq_T segments;
    Seq_T unmapped_segments; /* sequence holding unmapped ids */
};

/* function definitions =====================================================*/

/* segment_new
 *
 *      Purpose: Create and initialize the main memory segment that will store
 *               instructions and other segments.
 *
 *   Parameters: None
 *
 *      Returns: The instance of the main memory
 *
 * Expectations: Make sure all memory is able to be allocated without failure.
*/
extern T segment_new()
{
    /* allocate space for memory equal to the size of the struct */
    T memory = malloc(sizeof(struct T));
    assert(memory != NULL);

    /* create sequence of segments and check memory correctly allocated */
    memory->segments = Seq_new(SEQ_SEGMENT);
    assert(memory->segments != NULL);

    /* create sequence of unmapped segments */
    memory->unmapped_segments = Seq_new(SEQ_UNMAPPED);
    assert(memory->unmapped_segments != NULL);

    return memory; /* return created memory */
}

/* segment_free
 *
 *      Purpose: Free all memory associated with the main memory.
 *
 *   Parameters: The instance of main memory.
 *
 *      Returns: None
 *
 * Expectations: The memory segment passsed in is not null.
*/
extern void segment_free(T memory)
{
    assert(memory != NULL);
    UArray_T segment_array;

    /* frees all individual segment arrays */
    for (int i = 0; i < Seq_length(memory->segments); i++) {
        segment_array = Seq_get(memory->segments, i);

        if (segment_array != NULL) {
            UArray_free(&segment_array);
        }
    }

    /* frees all contents of unmapped segments sequence */
    for (int i = 0; i < Seq_length(memory->unmapped_segments); i++) {
        free(Seq_get(memory->unmapped_segments, i));
    }

    /* frees sequences and structs themselves */
    Seq_free(&(memory->segments));
    Seq_free(&(memory->unmapped_segments));
    free(memory);
}

/* segment_load
 *
 *      Purpose: Gets a word from a segment in main memory.
 *
 *   Parameters: The main memory, the index of the segment that holds the
 *               value, and the index of that segment.
 *
 *      Returns: The word at the given location.
 *
 * Expectations: Main memory is not null.
*/
extern uint32_t segment_load(T memory, int id, int offset)
{
    assert(memory != NULL);
    assert(id >= 0 && id < Seq_length(memory->segments));

    /* gets desired segment array based on input */
    UArray_T segment_array = Seq_get(memory->segments, id);
    assert(offset >= 0 && offset < UArray_length(segment_array));

    /* gets desired value in segment array */
    uint32_t load_value = *(uint32_t *)UArray_at(segment_array, offset);

    return load_value;
}

/* segment_store
 *
 *      Purpose: Stores a desired word in a segment in main memory.
 *
 *   Parameters: The main memory, the index of segment in main memory, the
 *               index for the word to be placed, and the word to be placed.
 *
 *      Returns: None
 *
 * Expectations: Main memory is not null and the index of segment and index
 *               of location in that segment are valid.
*/
extern void segment_store(T memory, int id, int offset, uint32_t word)
{
    /* checks memory isn't empty and id is in bounds */
    assert(memory != NULL);
    assert(id >= 0 && id < Seq_length(memory->segments));

    UArray_T segment_array = Seq_get(memory->segments, id);
    assert(offset >= 0 && offset < UArray_length(segment_array));

    /* stores word at proper address in segment array */
    *(uint32_t *)UArray_at(segment_array, offset) = word;
}

/* segment_load_program
 *
 *      Purpose: Loads a new program into the instructions slot in main
 *               memory and frees memory associated with the old instruction
 *               set.
 *
 *   Parameters: The main memory and the index of segment to load.
 *
 *      Returns: None
 *
 * Expectations: Main memory is not null and the index of segment and index
 *               of location in that segment are valid.
*/
extern void segment_load_program(T memory, int id)
{
    assert(memory != NULL);
    assert(id >= 0 && id < Seq_length(memory->segments));

    /* gets desired segment array and finds length */
    UArray_T segment_array = Seq_get(memory->segments, id);
    int segment_array_length = UArray_length(segment_array);

    /* creates copy of segment array and frees old array */
    UArray_T program_array = UArray_copy(segment_array, segment_array_length);
    UArray_T old_program_array = Seq_get(memory->segments, 0);
    UArray_free(&old_program_array);

    /* puts contents into segment 0 */
    Seq_put(memory->segments, 0, program_array);
}

/* segment_unmap
 *
 *      Purpose: Unmap a segment in main memory.
 *
 *   Parameters: The main memory and the index of segment to unmap.
 *
 *      Returns: None
 *
 * Expectations: Main memory is not null and the index of segment is valid.
 *
 *         Note: Adds the index of the segment that was unmapped to the
 *               unmapped sequence to keep track of available memory.
*/
extern void segment_unmap(T memory, int id)
{
    assert(memory != NULL);
    assert(id >= 0 && id < Seq_length(memory->segments));

    UArray_T segment_array = Seq_put(memory->segments, id, NULL);
    UArray_free(&segment_array);

    /* allocate space for a new id to be stored */
    int *newId = malloc(sizeof(int));
    *newId = id;

    /* adds id to unmapped segments sequence */
    Seq_addhi(memory->unmapped_segments, (void *)(uintptr_t)newId);
}

/* segment_map
 *
 *      Purpose: Adds a new segment to main memory that can hold a pre-defined
 *               amount of words.
 *
 *   Parameters: The main memory and the number of words the segment will hold.
 *
 *      Returns: The index of the segment that was created.
 *
 * Expectations: The main memory is not null, the number of words is valid,
 *               and the memory created for the segment successfully.
*/
extern uint32_t segment_map(T memory, int size)
{
    assert(memory != NULL);
    assert(size > 0);

    int index;

    /* creates new segment array and checks memory was correctly allocated */
    UArray_T new_segment_array = UArray_new(size, sizeof(int));
    assert(new_segment_array != NULL);

    /* initializes every element in new segment array to be 0 */
    for (int i = 0; i < UArray_length(new_segment_array); i++) {
        *(uint32_t *)UArray_at(new_segment_array, i) = 0;
    }

    /* gets length of unmapped segments sequence */
    int unmapped_segments_length = Seq_length(memory->unmapped_segments);

    if (unmapped_segments_length == 0) {
        /* adds new segment array to sequence of segments */
        index = Seq_length(memory->segments);
        Seq_addhi(memory->segments, new_segment_array);
    } else {
        /* get index value and store in memory segments sequence */
        index = *(int *)Seq_get(memory->unmapped_segments, 0);
        free(Seq_remlo(memory->unmapped_segments));
        Seq_put(memory->segments, index, new_segment_array);
    }

    return (uint32_t)index; /* returns index of newly mapped segment */
}
