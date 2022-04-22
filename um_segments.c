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
//#include <seq.h> /* Hanson's sequence library */
//#include <uarray.h>

#include "um_segments.h"
#define T Memory_T

/* macros ================================================================== */
//#define SEQ_SEGMENT 1000
//#define SEQ_UNMAPPED 500

/* struct definition ======================================================= */
struct T {
    struct array **segments;
    uint32_t *unmapped_segments; /* sequence holding unmapped ids */
    int num_mapped;
    int num_unmapped;
};

struct array {
    uint32_t *words;
    int length; 
};

struct array *copy(struct array *segment);
void free_words(struct array *segment);

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
    //assert(memory != NULL);

    /* create sequence of segments and check memory correctly allocated */
    memory->segments = calloc(100000000, sizeof(struct array));
    //assert(memory->segments != NULL);

    /* create sequence of unmapped segments */
    memory->unmapped_segments = calloc(100000000, sizeof(uint32_t));
    //assert(memory->unmapped_segments != NULL);
    
    memory->num_mapped = 0;
    memory->num_unmapped = 0;

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
    //assert(memory != NULL);
    struct array *segment_array;
    //fprintf(stderr, "num mapped to free: %d\n", memory->num_mapped);

    /* frees all individual segment arrays */
    for (int i = 0; i < memory->num_mapped; i++) {
        segment_array = memory->segments[i];
        
        if (segment_array != NULL) {
            //fprintf(stderr, "not NULL\n");
            
            free(segment_array->words);
            free(segment_array);
        }
    }

    /* frees sequences and structs themselves */
    free(memory->segments);
    free(memory->unmapped_segments);
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
    //assert(memory != NULL);
    //assert(id >= 0 && id < memory->num_mapped);

    /* gets desired segment array based on input */
    struct array *segment_array = memory->segments[id];
    //assert(offset >= 0 && offset < segment_array->length);

    /* gets desired value in segment array */
    uint32_t load_value = segment_array->words[offset];

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
    //assert(memory != NULL);
    //fprintf(stderr, "num mapped: %u\n", memory->num_mapped);
    //fprintf(stderr, "id: %u\n",id);
    //assert(id >= 0 && id < memory->num_mapped);

    struct array *segment_array = memory->segments[id];
    //assert(offset >= 0 && offset < segment_array->length);

    /* stores word at proper address in segment array */
    segment_array->words[offset] = word;
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
    //assert(memory != NULL);
    //assert(id >= 0 && id < memory->num_mapped);

    /* gets desired segment array and finds length */
    struct array *segment_array = memory->segments[id];

    /* creates copy of segment array and frees old array */
    struct array *program_array = copy(segment_array);
    struct array *old_program_array = memory->segments[0];
    free(old_program_array->words);
    free(old_program_array);

    /* puts contents into segment 0 */
    memory->segments[0] = program_array;
}

struct array *copy(struct array *segment)
{
    int size = segment->length;
    struct array *new_arr = malloc(sizeof(struct array));
    new_arr->words = calloc(size, sizeof(uint32_t));
    for (int i = 0; i < size; i++) {
        new_arr->words[i] = segment->words[i];
    }
    new_arr->length = size;
    return new_arr;
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
    //assert(memory != NULL);
    //assert(id >= 0 && id < memory->num_mapped);

    struct array *segment_array = memory->segments[id];
    free(segment_array->words);
    free(segment_array);
    memory->segments[id] = NULL;

    /* adds id to unmapped segments sequence */
    memory->unmapped_segments[memory->num_unmapped] = id;
    memory->num_unmapped++;
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
    //assert(memory != NULL);
    //assert(size > 0);

    int index = 0;
    //fprintf(stderr, "num mapped: %d num unmapped: %d\n", memory->num_mapped, memory->num_unmapped);

    /* creates new segment array and checks memory was correctly allocated */
    struct array *new_segment_array = malloc(sizeof(struct array));
    new_segment_array->length = size;
    //assert(new_segment_array != NULL);

    /* initializes every element in new segment array to be 0 */
    new_segment_array->words = calloc(size, sizeof(uint32_t));

    /* gets length of unmapped segments sequence */
    if (memory->num_unmapped == 0) {
        /* adds new segment array to sequence of segments */
        index = memory->num_mapped;
        memory->segments[index] = new_segment_array;
    } else {
        /* get index value and store in memory segments sequence */
        uint32_t last = memory->num_unmapped - 1;
        index = memory->unmapped_segments[last];
        memory->segments[index] = new_segment_array;
        memory->unmapped_segments[last] = 0;
        memory->num_unmapped--;
    }
    memory->num_mapped++;

    return (uint32_t)index; /* returns index of newly mapped segment */
}
