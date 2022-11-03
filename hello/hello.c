#include <stdio.h>
#include <cs50.h>

int main(void)
{
    // Declare variable name and ask input from the user.
    string name = get_string("What's your name?");
    // Print greeting.
    printf("hello, %s\n", name);
}