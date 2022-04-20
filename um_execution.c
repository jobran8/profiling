/*
 * um_execution.c
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Implementation of the um_execution.h interface. Extracts all instructions,
 * keeps track of these instructions, and contains a switch statement to
 * call certain functions from um_instructions.h in accordance with a specified
 * input.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bitpack.h"
#include "um_instructions.h"
#include "um_execution.h"

#define T Instruction_T

/* instruction declarations ================================================ */
static inline void switch_commands(T instruction, Memory_T memory,
                                   uint32_t *registers, int *prog_counter);
static inline T pack_instruction(T instruction, uint32_t word);

/* struct definition ======================================================= */
struct T {
    unsigned opcode, register_A, register_B, register_C;
};

typedef enum Um_opcode { /* way to map numbers to global variables */
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/* function definition ===================================================== */

/* execute
 *
 *      Purpose: Call proper functions to execute instructions in segment 0.
 *
 *   Parameters: Instance of program memory, pointer to an array of
 *               registers, and a pointer to program counter to keep track of
 *               instructions.
 *
 *      Returns: None
 *
 * Expectations: None
*/
extern void execute(Memory_T program, uint32_t *registers, int *prog_counter)
{
    uint32_t word;

    /* allocate space for instruction struct */
    T instruction = malloc(sizeof(*instruction));

    /* runs until halt is reached or a failed case */
    while (true) {
        word = segment_load(program, 0, *prog_counter);
        instruction = pack_instruction(instruction, word);
        switch_commands(instruction, program, registers, prog_counter);
        (*prog_counter)++; /* moves to next instruction */
    }

}

/* static function definitions============================================== */

/* switch_commands
 *
 *      Purpose: Command loop to execute instruction based on given opcode.
 *
 *   Parameters: Instance of instruction struct, instance of program memory,
 *               pointer to an array of registers, and a pointer to program
 *               counter to keep track of instructions.
 *
 *      Returns: None
 *
 * Expectations: None
*/
static inline void switch_commands(T instruction, Memory_T memory,
                                  uint32_t *registers, int *prog_counter)
{
    (void)prog_counter;

    /* performs a certain instruction based on opcode */
    switch(instruction->opcode) {
        case CMOV:
            conditional_move(registers, instruction->register_A,
                             instruction->register_B, instruction->register_C);
            break;

        case SLOAD:
            segmented_load(registers, instruction->register_A,
                            instruction->register_B, instruction->register_C,
                            memory);
            break;

        case SSTORE:
            segmented_store(registers, instruction->register_A,
                            instruction->register_B, instruction->register_C,
                            memory);
            break;

        case ADD:
            addition(registers, instruction->register_A,
                     instruction->register_B, instruction->register_C);
            break;

        case MUL:
            multiplication(registers, instruction->register_A,
                            instruction->register_B, instruction->register_C);
            break;

        case DIV:
            division(registers, instruction->register_A,
                    instruction->register_B, instruction->register_C);
            break;

        case NAND:
            bitwise_nand(registers, instruction->register_A,
                         instruction->register_B, instruction->register_C);
            break;

        case HALT:
            free(instruction);
            halt(memory);
            break;

        case ACTIVATE:
            map_segment(registers, instruction->register_A,
                         instruction->register_B, instruction->register_C,
                         memory);
            break;

        case INACTIVATE:
            unmap_segment(registers, instruction->register_A,
                           instruction->register_B, instruction->register_C,
                           memory);
            break;

        case OUT:
            output(registers, instruction->register_A,
                    instruction->register_B, instruction->register_C);
            break;

        case IN:
            input(registers, instruction->register_A,
                    instruction->register_B, instruction->register_C);
            break;

        case LOADP:
            *prog_counter = (load_program(registers, instruction->register_A,
                          instruction->register_B, instruction->register_C,
                          memory)) - 1;
            break;

        case LV:
            load_value(registers, instruction->register_A,
                        instruction->register_B, instruction->register_C,
                        (uint32_t)instruction->register_C);
            break;
    }

}

/* pack_instruction
 *
 *      Purpose: Command loop to execute instruction based on given opcode.
 *
 *   Parameters: Instance of instruction struct, a word that is passed in
 *               which holds an instruction.
 *
 *      Returns: A fully extracted instruction (instruction_T).
 *
 * Expectations: Valid opcode numbered 0 through 13.
*/
static inline T pack_instruction(T instruction, uint32_t word)
{
    /* pack opcode and make sure no bigger than 14 */
    instruction->opcode = Bitpack_getu(word, 4, 28);
    assert(instruction->opcode < 14);

    if (instruction->opcode == 13) {
        instruction->register_A = Bitpack_getu(word, 3, 25);
        instruction->register_B = 0;

        /* give register C the value of the instruction */
        instruction->register_C = Bitpack_getu(word, 25, 0);
    } else {
        instruction->register_A = Bitpack_getu(word, 3, 6);
        instruction->register_B = Bitpack_getu(word, 3, 3);
        instruction->register_C = Bitpack_getu(word, 3, 0);
    }

    return instruction; /* return packed instruction */
}
