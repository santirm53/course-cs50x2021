#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void)
{
    //Get the money owed to the user, checking it's positive.
    float owed;
    do
    {
        owed = get_float("Change owed:");
    }
    while (owed < 0);

    //Convert owed to cents and round the value.
    int cents;
    cents = round(owed * 100);

    //Create the change variable (number of coins) and start the substracting loop with a counter.
    int x = 0;
    while (cents >= 25)
    {
        cents = cents - 25;
        x++;
    }
    while (cents >= 10)
    {
        cents = cents - 10;
        x++;
    }
    while (cents >= 5)
    {
        cents = cents - 5;
        x++;
    }
    while (cents > 0)
    {
        cents = cents - 1;
        x++;
    }

    //Print the counter
    printf("%i\n", x);
}
