#include <cs50.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

int main(void)
{
    //Get text and set variables.
    string t = get_string("Text:");
    int words = 0, letters = 0, sentences = 0;

    //Loop for every character in the text, check what it is to count.
    for (int n = 0, i = strlen(t); n < i; n++)
    {
        //Ok so here I tried to think of every possibility:
        if (isalpha(t[n]) != 0) //If it's a letter, add a letter.
        {
            letters++;
        }
        else if (t[n] == '.' || t[n] == '?' || t[n] == '!') //If it's an "ending" punctuation, add a word and a sentence.
        {
            words++;
            sentences++;
        }
        else if (isalpha(t[n - 1]) != 0 && (t[n] == ' ' || t[n + 1] == ' '))
            //^Here we cover:
            //1. Being a 'space' between words, so a space with a LETTER before it.
            //2. Being a 'non-ending' punctuation mark, like a comma or a semicolon (i.e. something after a letter and before a SPACE).
        {
            words++; //And we count a word in both cases.
        }
        //ELSE the other possibilities, were we count nothing:
        //1. A 'space', after any punctuation (so NOT a letter before it).
        //2. Other signs separating words, such as apostrophes or hyphens (so someting after a letter and before a LETTER)
    }

    //Calculate L and S.
    float l = ((float) letters * 100) / words;
    float s = ((float) sentences * 100) / words;

    //Calculate grade and round.
    float grade = (0.0588 * l) - (0.296 * s) - 15.8;
    grade = round(grade);

    //Print result.
    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", (int) grade);
    }
}