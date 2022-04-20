/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.
 *
 * Any additional functions and unit tests written for the lab go
 * here.
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode { /* way to map numbers to global variables */
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void)
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c)
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, 
                                      Um_register c)
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction divide(Um_register a, Um_register b, Um_register c)
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction input(Um_register a, Um_register b, Um_register c)
{
        return three_register(IN, a, b, c);
}

static inline Um_instruction conditional_move(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction map_segment(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(ACTIVATE, a, b, c);
}

static inline Um_instruction unmap_segment(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(INACTIVATE, a, b, c);
}

static inline Um_instruction load_program(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(LOADP, a, b, c);
}

static inline Um_instruction bitwise_nand(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction segment_load(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segment_store(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction load_val(Um_register a, Um_register b, 
                                              Um_register c)
{
        return three_register(LV, a, b, c);
}

Um_instruction output(Um_register c);

/* Functions for working with streams */
static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }

}

/* function definitions */
Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
    uint64_t um_instruction = 0;

    um_instruction = Bitpack_newu(um_instruction, 4, 28, op);
    um_instruction = Bitpack_newu(um_instruction, 3, 6, ra);
    um_instruction = Bitpack_newu(um_instruction, 3, 3, rb);
    um_instruction = Bitpack_newu(um_instruction, 3, 0, rc);

    return (uint32_t)um_instruction;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
    uint64_t um_instruction = 0;
    um_instruction = Bitpack_newu(um_instruction, 4, 28, 13);
    um_instruction = Bitpack_newu(um_instruction, 3, 25, ra);
    um_instruction = Bitpack_newu(um_instruction, 25, 0, val);
    return (uint32_t)um_instruction;
}

Um_instruction output(Um_register c)
{
    return three_register(10, 0, 0, c);
}


/* Unit tests for the UM */
void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

/* ensure that halt terminates program */
void build_ensure_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, input(0 , 0, r1));
        append(stream, output(r1));
}

/* add 48 and 6 */
void build_add_test(Seq_T stream)
{
    append(stream, loadval(r1, 48));
    append(stream, loadval(r2, 52));
    append(stream, add(r3, r1, r2));
    append(stream, output(r3));
    append(stream, halt());
}

/* multiply 30 and 2 */
void build_multiplication_test(Seq_T stream)
{
    append(stream, loadval(r2, 10));
    append(stream, loadval(r3, 10));
    append(stream, multiply(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

/* divide 10 by two and 5 by two */
void build_division_test(Seq_T stream)
{
    append(stream, loadval(r2, 200));
    append(stream, loadval(r3, 2));
    append(stream, divide(r1, r2, r3));
    append(stream, output(r1));

    append(stream, loadval(r2, 201));
    append(stream, loadval(r3, 2));
    append(stream, divide(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

/* Nanding twice gives the origional result */
void build_bitwise_nand_test(Seq_T stream)
{
    append(stream, loadval(r1, 100));
    append(stream, loadval(r2, 100));
    append(stream, bitwise_nand(r4, r1, r2));
    append(stream, bitwise_nand(r5, r4, r4));
    append(stream, output(r5));
    append(stream, halt());
}

/* test input + output and functionallity after */
void build_io_test(Seq_T stream)
{   
    append(stream, input(0, 0, r2));
    append(stream, output(r2));
    append(stream, halt());
}

/* load a value and a character into register */
void build_loadval_test(Seq_T stream)
{
    append(stream, loadval(r4, 100));
    append(stream, output(r4));
    append(stream, loadval(r4, 'a'));
    append(stream, output(r4));
    append(stream, halt());
}

/* test a successful move and an unsuccessful move */
void build_conditional_move_test(Seq_T stream)
{
    append(stream, loadval(r2, 100));
    append(stream, loadval(r3, 0));
    append(stream, loadval(r1, 130));
    append(stream, conditional_move(r1, r2, r3));
    append(stream, output(r1));

    append(stream, loadval(r3, 20));
    append(stream, conditional_move(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

/* Make sure a segment is mapped with the correct amount of words */
void build_map_segment_test(Seq_T stream)
{
    append(stream, loadval(r3, 11));
    append(stream, loadval(r1, 0));
    append(stream, loadval(r2, 0));
    append(stream, map_segment(r1, r2, r3));
    append(stream, loadval(r1, 1));
    append(stream, loadval(r2, 10));
    append(stream, loadval(r3, 100));
    append(stream, segment_store(r1, r2, r3));
    append(stream, loadval(r3, 10));
    append(stream, segment_load(r2, r1, r3));
    append(stream, output(r2));
    append(stream, halt());
}

/* 
 * Make sure we can unmap a segment and the next segment mapped will
 * go to the index that was unmapped and not added onto the end, so
 * we are effectively reusing memory segments. We map 40 segments
 * in order to output a printable ascii character to make sure it
 * correctly maps to the right spot.
 */
void build_unmap_segment_test(Seq_T stream)
{
    append(stream, loadval(r3, 11));
    append(stream, loadval(r1, 0));
    append(stream, loadval(r2, 0));
    
    for (int i = 0; i < 40; i++) {
        append(stream, map_segment(r1, r2, r3));
    }
    
    append(stream, loadval(r3, 35));
    append(stream, map_segment(r1, r2, r3));
    append(stream, unmap_segment(r1, r2, r3));
    append(stream, map_segment(r1, r2, r3));
    append(stream, output(r2));
    append(stream, halt());
}


/* Load halt instruction into segment and call load program on it */
void build_load_program_test(Seq_T stream)
{
    append(stream, loadval(r3, 1)); 
    append(stream, map_segment(r1, r2, r3));
    append(stream, loadval(r1, 0));
    append(stream, loadval(r2, 9));  
    append(stream, segment_load(r3, r1, r2));
    append(stream, loadval(r1, 1)); 
    append(stream, loadval(r2, 0)); 
    append(stream, segment_store(r1, r2, r3));
    append(stream, load_program(r5, r1, r2));
    append(stream, halt());
}

/* store and load a value at last index */
void build_segment_store_test(Seq_T stream)
{
    append(stream, loadval(r3, 3)); 
    append(stream, map_segment(r1, r2, r3));
    append(stream, loadval(r1, 1));
    append(stream, loadval(r2, 2));
    append(stream, loadval(r3, 100));
    append(stream, segment_store(r1, r2, r3));
    append(stream, segment_load(r4, r1, r2));
    append(stream, output(r4));
    append(stream, halt());
}

/* store and load a value at 0 index */
void build_segment_load_test(Seq_T stream)
{
    append(stream, loadval(r3, 3)); 
    append(stream, map_segment(r1, r2, r3));
    append(stream, loadval(r1, 1));
    append(stream, loadval(r2, 0));
    append(stream, loadval(r3, 100));
    append(stream, segment_store(r1, r2, r3));
    append(stream, loadval(r2, 1));
    append(stream, loadval(r3, 0));
    append(stream, segment_load(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}









