/*
 * um_main.c
 * by Rachel Scrivanich and Andrew Maynard, 4/14/22
 * um
 *
 * Contains main() function to properly use all needed interfaces in order
 * to correctly make a software implementation that emulates a universal
 * machine.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h> /* struct for finding file size */
#include <stdint.h>

#include "um_initialize.h"
#include "um_execution.h"

int main(int argc, char *argv[])
{
    FILE *fp;
    struct stat st; /* instance of stat structure */

    /* exits failure if amount of arguments isn't equal to 2 */
    if (argc != 2) {
        fprintf(stderr, "Incorrect Number of Arguments!");
        return 1;
    }

    stat(argv[1], &st);
    uint32_t file_size = st.st_size;

    /* runs when file size has number of bytes that are divisible by 4 */
    if (file_size % 4 == 0) {
        fp = fopen(argv[1], "r"); /* opens a file for reading */
        assert(fp != NULL);

        uint32_t registers[8] = { 0 }; /* initialize 8 registers to 0 */
        int32_t prog_counter = 0; /* initialize program counter to 0 */

        /* initializes program and executes instructions */
        Memory_T program = initialize(fp, (file_size / 4));
        fclose(fp);
        execute(program, registers, &prog_counter);

        return 0; /* exit success */
    }

    return 1; /* exit failure */
}
