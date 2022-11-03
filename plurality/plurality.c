#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
}
candidate;

// Array of candidates
candidate candidates[MAX];

// Number of candidates
int candidate_count;

// Function prototypes
bool vote(string name);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX) //Candidate count is arguments - 1 (program name).
    {
        printf("Maximum number of candidates is %i\n", MAX); //If it's higher than MAX, print error 2.
        return 2;
    }
    for (int i = 0; i < candidate_count; i++) //Populate the array
    {
        candidates[i].name = argv[i + 1]; //Set each name in the struct to the arguments typed (ignoring program name)
        candidates[i].votes = 0; //Set every candidate's votes to 0.
    }

    int voter_count = get_int("Number of voters: "); //Get the number of voters from the user.

    // Loop over all voters
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: "); //Loop for every vote, first ask for the name of the candidate.

        // Check for invalid vote.
        if (!vote(name)) //Vote function must return boolean false if the name doesn't match.
        {
            printf("Invalid vote.\n");
        }
    }

    // Display winner of election
    print_winner();
}

// Update vote totals given a new vote
bool vote(string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)
        {
            candidates[i].votes++;
            return true;
        }
    }
    return false;
}

// Print the winner (or winners) of the election
void print_winner(void)
{
    //Get highest_votecount
    int highest_votecount = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes > highest_votecount)
        {
            highest_votecount = candidates[i].votes;
        }
    }
    //Print the winner/s
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == highest_votecount)
        {
            printf("%s\n", candidates[i].name);
        }
    }

}

