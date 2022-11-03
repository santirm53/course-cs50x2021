#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//Prototype
char cipher(char p, int k);

int main(int argc, string argv[])
{
    //Check for every character in the second word (the key) if it's a digit, or if there are more or less than 2 words (program name + key)
    //Print error message if needed.
    if (argc != 2)
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }

    for (int n = 0, i = strlen(argv[1]); n < i; n++)
    {
        if (isdigit(argv[1][n]) == 0)
        {
            printf("Usage: ./caesar key\n");
            return 1;
        }
    }

    //Convert the second argument written (the key) to an integer.
    int k = atoi(argv[1]);

    //Get the plaintext:
    string plaintext = get_string("plaintext: ");

    //Loop through the plaintext, returning the ciphertext to array substituting each character.
    for (int i = 0, l = strlen(plaintext); i < l; i++)
    {
        plaintext[i] = cipher(plaintext[i], k);
    }

    //Print the result:
    printf("ciphertext: %s\n", plaintext);

}

//This is the function that ciphers the text:
char cipher(char p, int k)
{
    //Make an array to use as an alphabetical indexed reference for the formula c = (p+k) % 26.
    string alphupper = "ABCDEFGHIJKLMNOPQRSTUVWYZ";
    string alphlower = "abcdefghijklmnopqrstuvwxyz";

    // if it's not an alphabetic character, return the same character
    if (isalpha(p) == 0)
    {
        return p;
    }

    //Loop through the reference alphabetical indexed array for a match. Then return the char in the indexed position + key, preserving the wrap effect with the formula.
    for (int i = 0; i < 26; i++)
    {
        if ((isupper(p) != 0) && (p == alphupper[i]))
        {
            return alphupper[((i + k) % 26)];
        }
        else if ((islower(p) != 0) && (p == alphlower[i]))
        {
            return alphlower[((i + k) % 26)];
        }
    }
    return 0;
}