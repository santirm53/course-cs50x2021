#include <stdio.h>
#include <cs50.h>
#include <math.h>

// MASTERCARD: 16 digits, Start with 51/52/53/54/55
// VISA: 13/16 digits, Start with 4
// AMEX: 15 digits, Start with 34/37

// Luhn's Algorithm:
// 1. Multiply every other digit by 2, starting with the second number to the last
// 2. Add the sum of those digits
// 3. Add the sum of the other digits that were not multiplied
// 4. If the total sum ends with 0, it is valid.

int main(void)
{
    //Getting a positive credit card number
    long cc;
    do
    {
        cc = get_long("Number:");
    }
    while (cc < 0);

    //Count the digits with a loop and a counter until the total is decimal (cero in a long)
    int count = 0;
    long i = cc; //this (and the program from now on) wont acknowledge the first digit if it's cero, but I reckon it won't matter
    while (i > 0)
    {
        i = i / 10;
        count++;
    }

    //As long as I have two digits, get the last one (modulo and divide by 10 and then floor the result) and then:
    //If the result has 2 digits after multiplying it by 2, do the same as before with the big number to store each in one integer (x1 and x2) and add them to the sum total.
    //If the results has 1 digit after multiplying it, just add that result to the sum total.
    long n = cc;
    int sum = 0;
    int digit;
    while (n > 9)
    {
        digit = floor(n % 100 / 10);
        if (digit * 2 > 9)
        {
            int x1;
            int x2;
            x1 = (digit * 2) % 10;
            x2 = floor((digit * 2) % 100 / 10);
            sum = sum + x1 + x2;
        }
        else
        {
            sum = sum + (digit * 2);
        }
        n = floor(n / 100);
    }

    //For the second part of the sum: Reset "n". Then, while it has 2 digits, sum the last one (using modulo like before) and take two digits out (by dividing by 100 and flooring).
    n = cc;
    while (n > 0) // The condition for this loop may need work. Greater than 0 may ignore the first digit if it's actually a cero. But if that'd be the case it wouldn't sum anything anyways.
    {
        digit = floor(n % 10);
        sum = sum + digit;
        n = floor(n / 100);
    }

    //Checking the first two digits. Two loops with "n", dividing until it has one digit or two before the decimal part and then flooring it. Store the numbers in separate integers.
    int h1;
    int h2;
    n = cc;
    while (n > 9)
    {
        n = n / 10;
    }
    h1 = floor(n);
    n = cc;
    while (n > 99)
    {
        n = n / 10;
    }
    h2 = floor(n % 10);

    //Last checkings, if checksum checks out, branch out depending on count from the start plus the first two digits.
    if (sum % 10 == 0)
    {
        if (count == 15 && h1 == 3 && (h2 == 4 || h2 == 7))
        {
            printf("AMEX\n");
        }
        else if (count == 16 && h1 == 5 && (h2 > 0 && h2 <= 5))
        {
            printf("MASTERCARD\n");
        }
        else if ((count == 13 || count == 16) && h1 == 4)
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}