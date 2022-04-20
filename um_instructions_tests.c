/*
 * um_instructions_tests.c
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Contains all declarations and definitions of functions to test the
 * architecture of our word instructions set up in um_instructions.h
 * interface. Tests each function in that interface through a "main()"
 * and considers many edge cases.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bitpack.h"
#include "um_instructions.h"

/* function declarations ================================================= */
void test_addition();
void test_multiplication();
void test_division();
void test_bitwise_nand();
void test_output();
void test_input();
void test_halt();
void test_conditional_move();
void test_segmented_load();
void test_segmented_store();
void test_map_segment();
void test_unmap_segment();

int main()
{
    test_addition();
    test_multiplication();
    test_division();
    test_bitwise_nand();
    test_output();
    test_input();
    test_conditional_move();
    test_segmented_load();
    test_segmented_store();
    test_map_segment();
    test_unmap_segment();

    return 0;
}

/* test_addition
 *
 *      Purpose: Test to see if we can successfully add values in two
 *               different registers and store in third.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Make sure values are usable and added correctly.
*/
void test_addition()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    addition(registers, 0, 5, 7);
    assert(registers[0] == 12);
}

/* test_multiplication
 *
 *      Purpose: Test to see if we can successfully multiply two values
 *               in registers and store in the third.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Make sure values are usable and added correctly.
*/
void test_multiplication()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    multiplication(registers, 0, 5, 7);
    assert(registers[0] == 35);
}

/* test_division
 *
 *      Purpose: Test to see if we can successfully divide two values
 *               in registers and store in the third.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Make sure values are usable and added correctly.
*/
void test_division()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    division(registers, 0, 5, 7);
    assert(registers[0] == 0);
    division(registers, 0, 7, 2);
    assert(registers[0] == 3);
}

/* test_bitwise_nand
 *
 *      Purpose: Test to see if we can successfully get the correct value
 *               from taking the negations of two values "anded".
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Make sure values are usable and added correctly.
*/
void test_bitwise_nand()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    bitwise_nand(registers, 0, 5, 7);
    // assert(registers[0] == -6);
    // TODO weird test
}

/* test_output
 *
 *      Purpose: Test to see if we can successfully output the correct
 *               character in register C.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: uint32_t in register C that is a valid character and one
 *               that is not. Character F should appear in terminal to
 *               standard output.
*/
void test_output()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 70 };
    output(registers, 0, 0, 7);
    printf("\n");

    // registers[7] = 256;
    // output(registers, 0, 0, 7);
}

/* test_input
 *
 *      Purpose: Test to see if we can successfully take in an inputted
 *               character (from stdin for the sake of testing) and store
 *               the value in register C.
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Integer is correct ascii vlaue of input, input is invalid
 *               and out of bounds, and using file _______ try EOF condition.
*/
void test_input()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    // input(registers, 0, 0, 7);
    // assert(registers[7] == 70);
    // printf("\n");
    (void)registers;

    // TODO test getting EOF? not sure, getc?
    // input(registers, 0, 0, 7);
    // assert(registers[7] == (uint32_t)~0);
}

/* test_halt
 *
 *      Purpose: Test to see if we can successfully stop the program and
 *               exit with no memory leaks.
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Run with valgrind for leaks and check exit code to be 1.
*/
void test_halt()
{
    Memory_T memory = segment_new();
    halt(memory);
}

/* test_load_value
 *
 *      Purpose: Test to see if we can successfully load a given value from
 *               main memory (OPCODE 13)
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Store a value and attempt to get it using load.
*/
// TODO

/* test_conditional_move
 *
 *      Purpose: Test to see if we can successfully move contents of register
 *               A to B if C is empty.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Test if C is empty and non-empty.
*/
void test_conditional_move()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    conditional_move(registers, 3, 4, 0);
    assert(registers[4] == 3);
    registers[0] = 1;
    conditional_move(registers, 4, 5, 0);
    assert(registers[5] != 3);
}

/* test_segmented_load
 *
 *      Purpose: Test to see if we can successfully load a word from a spot
 *               in main memory.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Put a word in memory and take it out.
*/
void test_segmented_load()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    Memory_T new_memory = segment_new();

    segment_map(new_memory, 1);
    segment_map(new_memory, 1);

    segment_store(new_memory, 1, 0, 100);
    registers[1] = 1;
    registers[2] = 0;
    segmented_load(registers, 0, 1, 2, new_memory);
    assert(registers[0] == 100);
    segment_free(new_memory);
}

/* test_segmented_store
 *
 *      Purpose: Test to see if we can successfully store a word at a spot
 *               in main memory.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Store a word and take it out.
*/
void test_segmented_store()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    Memory_T new_memory = segment_new();
    segment_map(new_memory, 1);
    segment_map(new_memory, 10);
    registers[0] = 1;
    registers[1] = 9;
    registers[2] = 100;

    segmented_store(registers, 1, 2, 0, new_memory);
    segmented_load(registers, 5, 0, 1, new_memory);
    assert(registers[5] == 100);

    segment_free(new_memory);
}

/* test_map_segment
 *
 *      Purpose: Test to see if we can successfully map to a new segment
 *               with the right amount of space.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Try to store an item in newly mapped segment and register B
 *               holds the correct ID.
*/
void test_map_segment()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    Memory_T new_memory = segment_new();
    registers[6] = 200;
    registers[0] = 0;
    map_segment(registers, 0, 7, 6, new_memory);
    int id = registers[7];
    assert(id == 0);

    registers[0] = 0;
    registers[1] = 199;
    registers[2] = 100;

    segmented_store(registers, 1, 2, 0, new_memory);
    segmented_load(registers, 5, 0, 1, new_memory);
    assert(registers[5] == 100);

    segment_free(new_memory);
}

/* test_unmap_segment
 *
 *      Purpose: Test to see if we can successfully unmap a given id.
 *
 *   Parameters: None
 *
 *      Returns: None
 *
 *        Tests: Unsure
*/
void test_unmap_segment()
{
    uint32_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    Memory_T new_memory = segment_new();
    registers[0] = 0;
    map_segment(registers, 0, 7, 6, new_memory);
    registers[5] = 0;
    unmap_segment(registers, 0, 0, 5, new_memory);
    segment_free(new_memory);
}
