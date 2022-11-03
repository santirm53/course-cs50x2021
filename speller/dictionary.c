// Implements a dictionary's functionality

#include <stdbool.h>

#include "dictionary.h"

#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table. Chosen by trial and error.
const unsigned int N = 60000;

// Hash table
node *table[N];

//Node counter:
unsigned int node_counter = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // TODO
    int index = hash(word);
    for (node *trav = table[index]; trav != NULL; trav = trav->next)
    {
        if (strcasecmp(word, trav->word) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // TODO
    //Hash function:
    // Source: djib2 by Dan Bernstein (http://www.cse.yorku.ca/~oz/hash.html) modified by reddit user u\SocratesSatisfied.
    unsigned long hash = 5381;
    int c = *word;
    c = tolower(c);
    while (*word != 0)
    {
        hash = ((hash << 5) + hash) + c;
        c = *word++;
        c = tolower(c);
    }
    return hash % N;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // TODO
    //Open dictionary file.
    FILE *input = fopen(dictionary, "r");
    if (input == NULL)
    {
        printf("Error accessing dictionary file.\n");
        return 0;
    }

    //Loading each word to a linked list.
    node *sll = NULL;

    //Read strings one at a time and create a node for each word.
    //Malloc buffer for the text:
    char *buffer = malloc(sizeof(char) * (LENGTH + 1));
    if (buffer == NULL)
    {
        printf("Error allocating memory for loader scan buffer.\n");
        return 0;
    }

    //Repeat for the whole text file:
    while (fscanf(input, "%s", buffer) != EOF)
    {
        //If it's the first node on the list:
        if (node_counter == 0)
        {
            //Malloc a node, fill it and change the head of the list.
            node *n = malloc(sizeof(node));
            if (n == NULL)
            {
                printf("Error allocating memory for new node.\n");
                return 0;
            }
            strcpy(n->word, buffer);
            n->next = NULL;
            node_counter++;
            sll = n;
        }
        //If it's not the first node:
        else
        {
            //Malloc a temporary node, fill it, connect it to the previous one and change the head.
            node *n = malloc(sizeof(node));
            if (n == NULL)
            {
                printf("Error allocating memory for new node.\n");
                return 0;
            }
            strcpy(n->word, buffer);
            //Put it at the beginning of the linked list.
            n->next = sll;
            sll = n;
            node_counter++;
        }
    }
    free(buffer);
    fclose(input);

    //This can probably be done without the previous sll to load the data but whatever.
    //This loop traverses through the singledly linked list with all the words:

    //Set all pointers in the hash table to NULL
    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    for (node *trav = sll; trav != NULL; trav = sll)
    {
        //I've already allocated the nodes, so I'll just play with the pointers to re-link it to the hash table array:

        //Set the hash table pointer at location "hash value of the word in the node" to the current node in the SLL.
        //And set, for the time being, it's next value to NULL. To avoid orphaning the SLL, first change
        //it's pointer to the next node.

        //If there's a collition:
        if (table[hash(trav->word)] != NULL)
        {
            node *trav2 = table[hash(trav->word)];
            //Set a second pointer to the end of the SLL at the hash table at a given index
            while (trav2->next != NULL)
            {
                trav2 = trav2->next;
            }
            //Then chain the node trav points at to the end of the list at that given index.
            trav2->next = trav;
            sll = trav->next;
            trav->next = NULL;
        }

        //If there's no collision
        else
        {
            table[hash(trav->word)] = trav;
            sll = trav->next;
            trav->next = NULL;
        }
    }
    return 1;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    //Traversal for loop with a counter? Maybe use node_counter?
    return node_counter;

    //OR:
    //unsigned int word_number = 0;
    //for (node *trav = sll; trav != NULL; trav = trav->next)
    //{
    //    word_number++;
    //}
    //return word_number;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // TODO
    //Traverse through the hash table:
    for (int i = 0; i < N; i++)
    {
        //At each row of the table, if there actually is something linked:
        if (table[i] != NULL)
        {
            //Loop through each linked list.
            for (node *trav = table[i]; trav != NULL; trav = table[i])
            {
                node *tmp = trav->next;
                free(trav);
                table[i] = tmp;
            }
        }
    }
    return 1;
}
