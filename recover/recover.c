#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//Pseudocode:
//1. Check first 4 BYTES of each 512B block until the file ends.
//  1.1. If bytes == jpeg start
//      If first jpeg -> start writing into a new file.
//      If not first jpeg -> close previos file and open new jpeg.
//  1.2 If bytes != jpeg start
//      Copy them into the same file after the previous block.

int main(int argc, char *argv[])
{
    //Check for correct usage.
    if (argc != 2)
    {
        printf("Usage: ./recover 'forensic image'.\n");
        return 1;
    }

    //Open file.
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Couldn't open forensic image.\n");
        return 1;
    }

    //Create a buffer for reading.
    typedef uint8_t BYTE;
    BYTE *buffer = malloc(sizeof(BYTE) * 512);
    if (buffer == NULL)
    {
        printf("Error allocating buffer memory.\n");
        return 1;
    }

    //Buffer for the output identifier.
    int jpg_count = 0;
    char *filename = malloc(sizeof(char) * 8);
    if (filename == NULL)
    {
        printf("Error allocating buffer memory.\n");
        return 1;
    }

    //While the end of file hasn't been reached.
    while (fread(buffer, sizeof(BYTE), 512, input))
    {
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            //Check if it's the first jpeg.
            if (jpg_count == 0)
            {
                //Name the string as first jpg. Open a file with that name, get a pointer to it to write to, and write.
                sprintf(filename, "%03i.jpg", jpg_count);
                jpg_count++;
                FILE *output = fopen(filename, "w");
                fwrite(buffer, sizeof(BYTE), 512, output);
                fclose(output);
            }
            else
            {
                //After the first, it must be the next one, so close filename whatever, rename the name string to +1 jpg.
                //Open a new file with a new name, get a pointer to it to write to, and actually write to it.
                sprintf(filename, "%03i.jpg", jpg_count);
                jpg_count++;
                FILE *output = fopen(filename, "w");
                fwrite(buffer, sizeof(BYTE), 512, output);
                fclose(output);
            }
        }
        else if (jpg_count != 0)
        {
            //If it's not the starting bytes, just keep writing to the opened file at the time.
            FILE *output = fopen(filename, "a");
            fwrite(buffer, sizeof(BYTE), 512, output);
            fclose(output);
        }
    }
    //When we reach the end, close working file, free allocated memory.
    fclose(input);
    free(buffer);
    free(filename);
    return 0;

}