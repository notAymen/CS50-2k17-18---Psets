/*

Program: Recovers JPEGs from a forensic image.

Website: https://docs.cs50.net/2018/x/psets/4/recover/recover.html#background

*/

#include <stdio.h>
#include <stdlib.h>

// Just an alias, idk I like it
typedef unsigned char BYTE;

// Function Prototype
int check4JPEG_signature(BYTE *buffer);


int main(int argc, char *argv[]) {

// Correct Usage Check - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #1

        // Correct Usage Check
        if (argc != 2) {

            fprintf(stderr, "Usage: ./recover image");
            return 1;
        }



// Setting up to Read in the Infile - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #2

        // Store filename
        char *infile = argv[1];

        // Create File Pointer to Read Infile
        FILE *inptr = fopen(infile, "r");  // <--- notice the b, that's because we need/want to read the file in binary

        // Check if pointer was successfully created
        if (inptr == NULL) {

            fprintf(stderr, "Could not open %s\n", infile);
            return 2;
        }



// Setting up the Operation - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #3

        // We'll use this to keep track of what serial number we're at
        int serialCount = 0;

        // We'll use this to signal that we've found the beginning of the first JPEG
        int first_JPEG_found = 0;

        // We'll use this to store the JPEG filenames
        char *outfile = malloc(8);

        // We'll use this to store a 512 Byte block of data (array of 512 --> 1 bytes)
        BYTE buffer[512];

        // Declare File Pointer; we'll use this for the outfiles
        FILE *outptr;



// Find 1st JPEG - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #4

        // From the beginning of the memory card till we find the first JPEG
        while (!first_JPEG_found) {

            // Read a 512 Byte block
            fread(buffer, sizeof(BYTE), 512, inptr);

            // Check if the block starts with the 4 signature JPEG bytes
            if (check4JPEG_signature(buffer) == 1) {

                // Signal that we've found the beginning of the first JPEG
                first_JPEG_found++;

                // Make Serial number for the JPEG & Increment Serial Number
                sprintf(outfile, "%.3i.jpeg", serialCount); serialCount++;

                // Close previous file (if it exists), Open a new File & Associate File Pointer to it
                outptr = fopen(outfile, "w");

                // Error checking
                if (outptr == NULL) {

                    fclose(inptr);
                    fprintf(stderr, "Could not create %s\n", outfile);
                    return 2;
                }

                // Write the Block
                fwrite(buffer, sizeof(BYTE), 512, outptr);
            }
        }



// Find the rest of the JPEGs - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #5

        // After finding the first JPEG till the end of the memory card
        while (fread(buffer, sizeof(BYTE), 512, inptr) > 0) {  // <---- read a 512 Byte block

            // Check if the block starts with the 4 signature JPEG bytes
            if (check4JPEG_signature(buffer) == 1) {

                // Make Serial number for the JPEG, Store in Variable & Increment Serial Number
                sprintf(outfile, "%.3i.jpeg", serialCount); serialCount++;

                // Close previous file (if it exists), Open a new File & Associate File Pointer to it
                freopen(outfile, "w", outptr);

                // Error checking
                if (outptr == NULL) {

                    fclose(inptr); fclose(outptr);
                    fprintf(stderr, "Could not create %s\n", outfile);
                    return 2;
                }

                // Write the Block
                fwrite(buffer, sizeof(BYTE), 512, outptr);
                    
            }

            else {

                // Write the Block
                fwrite(buffer, sizeof(BYTE), 512, outptr);
            }
                
        }



// Close Streams & Free Memory - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #6

    fclose(inptr); fclose(outptr);

    free(infile); free(outfile);



// Success - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #7

    return 0;
}



// Function: Tests 4 bytes to see whether or not they match the JPEG signature bytes, returns true if so.
int check4JPEG_signature(BYTE *buffer) {

    if (    buffer[0] == 0xff
         && buffer[1] == 0xd8
         && buffer[2] == 0xff
         && 0xe0 <= buffer[3] && buffer[3] <= 0xef ) {

        return 1; // 1 meaning true
    }

    else {
            
        return 0;  // 0 meaning false    
    }
       
}
