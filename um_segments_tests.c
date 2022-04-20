/*
 * um_segments_tests.c
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Contains all declarations and definitions of functions to test the
 * architecture of our main memory set up in um_segments.h interface.
 * Tests each function in that interface through a "main()" and considers
 * many edge cases.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <seq.h>
#include <uarray.h>

#include "um_segments.h"

/* redefinition for testing purposes ====================================== */
#define T Memory_T

#define SEQ_SEGMENT 1000
#define SEQ_UNMAPPED 500

struct T {
    Seq_T segments;
    Seq_T unmapped_segments;
};

/* function declarations =================================================== */
void test_segment_new();
void test_segment_free();
void test_segment_map();
void test_segment_unmap();
void test_segment_map_unmap();
void test_segment_store();
void test_segment_load();
void test_segment_load_program();


/* function definitions ==================================================== */
int main()
{
    test_segment_new();
    test_segment_free();
    test_segment_map();
    test_segment_unmap();
    test_segment_map_unmap();
    test_segment_store();
    test_segment_load();
    test_segment_load_program();

    return 0;
}

/* test_segment_new
 *
 *    Purpose: Test to see if our main memory can successfully be allocated
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Memory allocation succesful
 *
*/
void test_segment_new()
{
    Memory_T new_memory = segment_new();

    assert(new_memory != NULL);
    assert(new_memory->segments != NULL);
    assert(new_memory->unmapped_segments != NULL);

    segment_free(new_memory);
}

/* test_segment_free
 *
 *    Purpose: Make sure memory can be deallocated succesfully
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Memory free is successful on empty list; all other tests will
 *             test the functionality of freeing main memory with memory
 *             segments mapped and memory segments mapped and unmapped, so
 *             our sequence of memory will have some indices that are NULL in
 *             between non-null indices therefore testing to see if try to
 *             free a NULL segment and get an error.
 *
*/
void test_segment_free()
{
        Memory_T new_memory = segment_new();
        segment_free(new_memory);
}

/* test_segment_map
 *
 *    Purpose: Make sure we can map segments succesfully with right amount
 *             of memory in segments.
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Can map segments onto the end of main memory in the correct
 *             location and return the correct index of segment.
 *
*/
void test_segment_map()
{
    Memory_T new_memory = segment_new();
    int indexOfSegment = segment_map(new_memory, 4);
    assert(indexOfSegment == 0);

    indexOfSegment = segment_map(new_memory, 6);
    assert(indexOfSegment == 1);

    assert(UArray_length(Seq_get(new_memory->segments, 0)) == 4);
    segment_free(new_memory);

}

/* test_segment_unmap
 *
 *    Purpose: Make sure memory is unmapped and set to NULL
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Unmap the end of memory and assert correct state of memory and
 *             unmapped segments gains an element.
 *
*/
void test_segment_unmap()
{
    /* unmap empty list */
    Memory_T new_memory = segment_new();
    segment_map(new_memory, 3);
    segment_map(new_memory, 8);

    segment_unmap(new_memory, 1);

    assert(Seq_get(new_memory->segments, 1) == NULL);
    assert(Seq_length(new_memory->unmapped_segments) == 1);

    segment_free(new_memory);
}

/* test_segment_unmap
 *
 *    Purpose: Test behavior of map and unmap when a segment between first
 *             address and end of main memory.
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Unmap out of bounds, unmapped_segments works properly, length
 *             of main memory updates, (line 170) tests freeing memory
 *             when a segment between start and end is NULL.
 *
*/
void test_segment_map_unmap()
{
    Memory_T new_memory = segment_new();
    segment_map(new_memory, 3);
    segment_map(new_memory, 10);
    segment_map(new_memory, 17);
    segment_unmap(new_memory, 1);

    /* segment_unmap(new_memory, 1000); */

    assert(Seq_length(new_memory->segments) == 3);
    assert(Seq_length(new_memory->unmapped_segments) == 1);

    /* segment_free(new_memory); */

    segment_map(new_memory, 12);
    assert(Seq_length(new_memory->segments) == 3);
    assert(Seq_length(new_memory->unmapped_segments) == 0);

    segment_free(new_memory);
}

/* test_segment_store
 *
 *    Purpose: Test the ability to store words in specific segments.
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Store values in spots, store with offset out of bounds (208),
 *             store with ID out of bounds (209), store on NULL segment (210).
 *
*/
void test_segment_store()
{
    Memory_T new_memory = segment_new();
    segment_map(new_memory, 10);
    segment_map(new_memory, 5);
    segment_map(new_memory, 4);
    segment_unmap(new_memory, 1);
    segment_map(new_memory, 5);

    segment_store(new_memory, 1, 4, 58);
    segment_store(new_memory, 0, 0, 31);
    segment_store(new_memory, 2, 3, 22);
    assert(*((int *)UArray_at(Seq_get(new_memory->segments, 1), 4)) == 58);
    assert(*((int *)UArray_at(Seq_get(new_memory->segments, 0), 0)) == 31);
    assert(*((int *)UArray_at(Seq_get(new_memory->segments, 2), 3)) == 22);

    /* segment_store(new_memory, 2, -1, 22); */
    /* segment_store(new_memory, 3, 4, 58); */

    /* segment_unmap(new_memory, 1);
       segment_store(new_memory, 1, 4, 58); */

    segment_free(new_memory);
}

/* test_segment_load
 *
 *    Purpose: Test the ability to load words from segments in memory.
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Load values in spots, load from offset out of bounds (245),
 *             load from ID out of bounds (246), load on NULL segment (247),
 *             try to load from empty spot in segment(249).
 *
*/
void test_segment_load()
{
    Memory_T new_memory = segment_new();
    segment_map(new_memory, 10);
    segment_map(new_memory, 5);
    segment_map(new_memory, 4);

    segment_store(new_memory, 1, 4, 58);
    segment_store(new_memory, 0, 0, 31);
    segment_store(new_memory, 2, 3, 22);

    assert(segment_load(new_memory, 0, 0) == 31);
    assert(segment_load(new_memory, 0, 0) == 31);
    assert(segment_load(new_memory, 0, 0) == 31);

    /* segment_load(new_memory, 0, 100); */
    /* segment_load(new_memory, 3, 2); */
    /* segment_unmap(new_memory, 1);
       segment_load(new_memory, 1, 0); */
    /* assert(segment_load(new_memory, 0, 1) == 0); */

    segment_free(new_memory);
}

/* test_segment_load_program
 *
 *    Purpose: Test the ability to load a new program from another segment and
 *             recycle memory from old instructions.
 *
 * Parameters: None
 *    Returns: None
 *
 *      Tests: Make sure new program is loaded in correctly and old memory
 *             associated with old program was deleted. Tests invallid id (283)
 *             Tests to make sure deep copy is made.
 *
*/
void test_segment_load_program()
{
    Memory_T new_memory = segment_new();

    segment_map(new_memory, 10);
    segment_map(new_memory, 5);
    segment_map(new_memory, 4);

    segment_store(new_memory, 0, 4, 50);
    segment_store(new_memory, 1, 2, 100);

    segment_load_program(new_memory, 1);
    assert(segment_load(new_memory, 0, 2) == 100);

    /* segment_load_program(new_memory, 1000); */

    segment_store(new_memory, 1, 0, 100);
    assert(segment_load(new_memory, 0, 0) != 100);

    segment_free(new_memory);
}
