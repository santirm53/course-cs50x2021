#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];
int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool check_cycle(int i, int j);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }
    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }
        record_preferences(ranks);
        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)  //Look up the candidates list for a match with the provided name.
        {
            //If it matches, update ranks array for the corresponding rank/j. And end the vote function.
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    //First of all I'm creating a struct to index every candidate to his position in each "ranks" array.
    typedef struct
    {
        int id;
        int index;
    }
    candidate;

    //Now I'll declare an array of this struct type and populate the ID's.
    candidate candidates_struct[candidate_count];
    for (int i = 0; i < candidate_count; i++)
    {
        candidates_struct[i].id = i;
    }

    //Now I'll populate the index part of the struct depending on the ranks array the voter entered.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (ranks[i] == candidates_struct[j].id)
            {
                candidates_struct[j].index = i;
            }
        }
    }
    //So now I have an array that saves for each candidate by number id (transferable to the preferences table) his index
    //for any given ranks list.

    //Now I'll loop through the 2D array preferences, in each cell checking some things:
    //1. That i != j, so we're not comparing anyone to himself.
    //2. That i == ranks[n], so we're on the "does he win?" correct row.
    //3. That his index is in fact better (lower) than "j's" index, given any ranks order list.

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            for (int n = 0; n < candidate_count; n++)
            {
                if ((i != j) && (i == ranks[n]) && (candidates_struct[i].index < candidates_struct[j].index))
                {
                    preferences[i][j]++;
                }
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    //We'll loop through each cell of the preferences array, cheking if votes are bigger than the inverted pair.
    //Then adding the pair counter.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //First I'll create an array that serves as an index for sorting the pairs later.
    //And another counter to not modify pair_count because I'm reusing the code.
    int strength[pair_count];
    int strength_count = 0;

    //Then I'll loop through the table the same way as in add pairs, adding the strength of victory to a new array that will
    //therefore correspond to the same index as each pair in the pairs array.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                strength[strength_count] = (preferences[i][j]) - (preferences[j][i]);
                strength_count++;
            }
        }
    }

    //Now to the sorting, I'll use an inverted bubble sort that "reads" strength array and modifies both "strength[]" (maybe unnecesary?)
    //and pairs[].
    pair temp2;
    int swap_counter = 1;
    while (swap_counter != 0)
    {
        swap_counter = 0;
        for (int i = 0, temp; i < (pair_count - 2); i++)
        {
            if (strength[i] < strength[i + 1])
            {
                temp = strength[i];
                strength[i] = strength[i + 1];
                strength[i + 1] = temp;
                temp2 = pairs[i];
                pairs[i] = pairs [i + 1];
                pairs[i + 1] = temp2;
                swap_counter++;
            }
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++) //For each pair at the pairs array:
    {
        if (check_cycle(pairs[i].winner, pairs[i].loser) == true) //Change the locked array if the custom function is true
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    //So the winner of the election is that one who doesn't lose to anyone in the locked table/array.
    //So, that candidate who has no TRUE's in it's COLUMN.
    int column_value[candidate_count];

    //So this checks the columns, cell by cell, and creates an array indexed with the candidate list
    //(or the preferences or locked columns for that matter) which has a value of 0 if there are no true values in that column.
    for (int j = 0; j < candidate_count; j++)
    {
        column_value[j] = 0;
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[i][j] == true)
            {
                column_value[j]++;
            }
        }
    }

    //Lastly, this checks for the index of the columns that have no "trues" and prints the corresponding candidate.
    for (int i = 0; i < candidate_count; i++)
    {
        if (column_value[i] == 0)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}

//Custom function:
bool check_cycle(int i, int j)
{
    //Base case 1, if it has already lost to him.
    if (locked[j][i] == true)
    {
        return false;
    }
    else
    {
        //Check who beats "i" (the winner)
        for (int x = 0; x < candidate_count; x++)
        {
            if (locked[x][i] == true) //If anyone does, check if someone beats him and check if that one is beaten by "j". Recursively.
            {
                return check_cycle(x, j);
            }
        }
        //Base case 2, I guess, if no one beats the beater of the winner.
        return true;
    }
}