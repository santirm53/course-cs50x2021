#include <cs50.h>
#include <stdio.h>

int main(void)
{
    //Declare "n" integer and ask the user for it's value (the height of the pyramid).
    //Make sure it's greater than 1 and less than 8.
    int n;
    do
    {
        n = get_int("Height:");
    }
    while (n < 1 || n > 8);

    //Nested "for loops": the first one draws each row (same as height with a new line after each one).
    //The other for loops draw each character. First, all characters of the row until n-1-current row are spaces.
    //Then, current row plus 1 hashes, then 2 spaces, then current row plus 1 hashes again.
    for (int r = 0; r < n; r++)
    {
        for (int i = 0; i < n - 1 - r; i++)
        {
            printf(" ");
        }
        for (int j = 0; j < r + 1; j++)
        {
            printf("#");
        }
        for (int k = 0; k < 2; k++)
        {
            printf(" ");
        }
        for (int j = 0; j < r + 1; j++)
        {
            printf("#");
        }
        printf("\n");
    }
}