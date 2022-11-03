#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

//Prototype
char cipher(char p, string k);

int main(int argc, string argv[])
{
    //Check the argument and print error message if needed.
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }

    //Check the lenght of the key
    if (strlen(argv[1]) != 26)
    {
        printf("Key must contain 26 characters\n");
        return 1;
    }

    //Validate the key:
    int counter = 0;
    for (int i = 0; i < 26; i++) //For each letter in the key
    {
        if (isalpha(argv[1][i]) == 0) //Check it's alphabetic
        {
            printf("Key cannot contain non-alphabetic characters\n");
            return 1;
        }
        for (int j = 0; j < 26; j++) //Go through the key again, checking for the same value (lower or uppercase)
        {
            if (toupper(argv[1][i]) == argv[1][j] || tolower(argv[1][i]) == argv[1][j])
            {
                counter++;
            }
        }
        if (counter != 1) //There should be one letter of each
        {
            printf("Key cannot contain repeated characters\n");
            return 1;
        }
        counter = 0;
    }

    //Get the validated key.
    string k = argv[1];

    //Get the plaintext:
    string plaintext = get_string("plaintext: ");

    //Loop through the plaintext, returning the ciphertext to array substituting each character.
    for (int i = 0, j = strlen(plaintext); i < j; i++)
    {
        plaintext[i] = cipher(plaintext[i], k);
    }

    //Print the result:
    printf("ciphertext: %s\n", plaintext);
    return 0;

}

//This is the function that ciphers the text:
char cipher(char p, string k)
{
    //Make an array to use as an alphabetical indexed reference.
    string alphupper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string alphlower = "abcdefghijklmnopqrstuvwxyz";

    //If it's not an alphabetic character, return the same character
    if (isalpha(p) == 0)
    {
        return p;
    }

    //Loop through the reference alphabetical indexed array for a match. Then return the char in the indexed position of the key.
    for (int i = 0; i < 26; i++)
    {
        if ((isupper(p) != 0) && (p == alphupper[i]))
        {
            return toupper(k[i]);
        }
        else if ((islower(p) != 0) && (p == alphlower[i]))
        {
            return tolower(k[i]);
        }
    }
    return 0;
}