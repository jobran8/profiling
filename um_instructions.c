/*
 * um_instructions.c
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Implementation of the um_instructions.h interface. Defines all functions
 * dealing with the instructions that will be executed by the universal
 * machine.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "um_instructions.h"

/* addition
 *
 *      Purpose: Add two register values and store in the third.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null.
*/
void addition(uint32_t *registers, unsigned A, unsigned B, unsigned C)
{
    assert(registers != NULL);
    registers[A]  = registers[B] + registers[C];
}

/* multiplication
 *
 *      Purpose: Multiply two register values and store in the third.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null.
*/
void multiplication(uint32_t *registers, unsigned A, unsigned B, unsigned C)
{
    assert(registers != NULL);
    registers[A]  = registers[B] * registers[C];
}

/* division
 *
 *      Purpose: Divide the contents of two registers and store in the third.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null and contents of register C is
 *               not 0.
*/
void division(uint32_t *registers, unsigned A, unsigned B, unsigned C)
{
    assert(registers != NULL);
    assert(registers[C] != 0); /* can't divide by 0 */

    registers[A]  = registers[B] / registers[C];
}

/* bitwise_nand
 *
 *      Purpose: Get the not of a bitwise operation between two registers
 *               and store result in the third.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null.
*/
void bitwise_nand(uint32_t *registers, unsigned A, unsigned B, unsigned C)
{
    assert(registers != NULL);
    registers[A]  = ~(registers[B] & registers[C]);
}

/* multiplication
 *
 *      Purpose: Multiply two register values and store in the third.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null and contents of register C
 *               contain a valid ascii number.
*/
void output(uint32_t *registers, unsigned A, unsigned B, unsigned C)
{
    (void)A;
    (void)B;

    assert(registers != NULL);
    assert(registers[C] <= 255); /* assert valid ascii character */

    putchar(registers[C]);
}

/* input
 *
 *      Purpose: Take in an inputed character and load it into given register.
 *               Load register with max value if end of file has been reached.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers is not null, the input is a valid ascii character.
*/
void input(uint32_t *registers, unsigned A, unsigned B, unsigned C)
{
    (void)A;
    (void)B;
    assert(registers != NULL);

    /* get inputted char and check if valid ascii character */
    int input = getchar();
    assert(input >= 0 && input <= 255);

    /* checks if end of input has been reached */
    if (input == EOF) {
        registers[C] = ~0; /* makes 32-bit word with all 1s */
    } else {
        registers[C] = (uint32_t)input;
    }
}

/* halt
 *
 *      Purpose: Terminate the program and free all memory.
 *
 *   Parameters: The instance of main memory being used.
 *
 *      Returns: None
 *
 * Expectations: None
*/
void halt(Memory_T memory)
{
    segment_free(memory);
    exit(EXIT_SUCCESS);
}

/* load_value
 *
 *      Purpose: Get an instruction from main memory
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null
*/
void load_value(uint32_t *registers, unsigned A, unsigned B, unsigned C,
                uint32_t value)
{
    (void)B;
    (void)C;

    assert(registers != NULL);
    registers[A] = value;
}

/* conditional_move
 *
 *      Purpose: Move the contents of register A to B if register C is empty.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null
*/
void conditional_move(uint32_t *registers, unsigned A, unsigned B, unsigned C)
{
    assert(registers != NULL);

    if (registers[C] != 0) {
        registers[A] = registers[B];
    }

}

/* segmented_load
 *
 *      Purpose: Load a word from a segment in main memory. The word is given
 *               by the contents of register B for the id and the contents of
 *               register C for the offset. Word is stored in register A.
 *
 *   Parameters: Pointer to the registers struct, the register number
 *               of the three registers to use in unwrapped word, and a
 *               pointer to an instance of main memory.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null
*/
void segmented_load(uint32_t *registers, unsigned A, unsigned B, unsigned C,
                    Memory_T memory)
{
    assert(registers != NULL);
    registers[A] = segment_load(memory, registers[B], registers[C]);
}

/* segmented_store
 *
 *      Purpose: Store a word into a segment in main memory. The word is in
 *               register B, contents of register C contains the id and
 *               contents of register A contains the offset.
 *
 *   Parameters: Pointer to the registers struct, the register number
 *               of the three registers to use in unwrapped word, and a
 *               pointer to an instance of main memory.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null
*/
void segmented_store(uint32_t *registers, unsigned A, unsigned B, unsigned C,
                     Memory_T memory)
{
    assert(registers != NULL);
    segment_store(memory, registers[A], registers[B], registers[C]);
}

/* map_segment
 *
 *      Purpose:
 *
 *   Parameters: Pointer to the registers struct, the register number
 *               of the three registers to use in unwrapped word, and a
 *               pointer to an instance of main memory.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null
*/
void map_segment(uint32_t *registers, unsigned A, unsigned B,
                               unsigned C, Memory_T memory)
{
    (void)A;

    assert(registers != NULL);
    uint32_t id = segment_map(memory, registers[C]);
    registers[B] = id; /* stores id in register B */
}

/* unmap_segment
 *
 *      Purpose: Unmap a segment in main memory, id is given by contents of
 *               register C.
 *
 *   Parameters: Pointer to the registers struct, the register number
 *               of the three registers to use in unwrapped word, and a
 *               pointer to an instance of main memory.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null
*/
void unmap_segment(uint32_t *registers, unsigned A, unsigned B, unsigned C,
                   Memory_T memory)
{
    (void)A;
    (void)B;

    assert(registers != NULL);
    segment_unmap(memory, registers[C]);
}

/* load_program
 *
 *      Purpose: Takes in inputted array of registers and memory to duplicate
 *               a segment in memory and return it. Sets program counter
 *               to register C.
 *
 *   Parameters: Pointer to the registers struct and the register number
 *               of the three registers to use in unwrapped word.
 *
 *      Returns: None
 *
 * Expectations: Registers pointer is not null
*/
int load_program(uint32_t *registers, unsigned A, unsigned B, unsigned C,
                 Memory_T memory)
{
    (void)A;

    assert(registers != NULL);
    int prog_counter = 0;

    if (registers[B] != 0) {
       segment_load_program(memory, registers[B]);
    }

    prog_counter = registers[C];
    return prog_counter;
}
