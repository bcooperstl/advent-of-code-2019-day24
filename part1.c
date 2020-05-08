#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_ROUNDS 1024

#define BUG '#'
#define EMPTY '.'

// having fun with bit-manipulation. because why use character arrays when I can do everything in uint32_t
uint32_t getNext(uint32_t current)
{
    printf("top - current is %d\n", current);
    /*
    positions: 01 02 03 04 05  shifts 00 01 02 03 04
               06 07 08 09 10  shifts 05 06 07 08 09
               11 12 13 14 15  shifts 10 11 12 13 14
               16 17 18 19 20  shifts 15 16 17 18 19
               21 22 23 24 25  shifts 20 21 22 23 24
    */
    uint32_t next=current;
    
    int num_adjacent[26]; // making my life easier by wasting an int so I can index from 1-25 instead of 0-24
    
    // set the four corners first
    // top-left
    num_adjacent[1]=((current&(1<<1))?1:0)+((current&(1<<5))?1:0);
    // top-right
    num_adjacent[5]=((current&(1<<3))?1:0)+((current&(1<<9))?1:0);
    // bottom-left
    num_adjacent[21]=((current&(1<<15))?1:0)+((current&(1<<21))?1:0);
    // bottom-right
    num_adjacent[25]=((current&(1<<19))?1:0)+((current&(1<<23))?1:0);
    
    // 3 middle cells in top row
    // 2 gets from positions 1, 3, 7 so shifts 0 (i-2), 2 (i), and 6 (i+4)
    for (int i=2; i<=4; i++)
        num_adjacent[i]=((current&(1<<(i-2)))?1:0)+((current&(1<<i))?1:0)+((current&(1<<(i+4)))?1:0); 
    // 3 middle cells in bottom row
    // 22 gets from positions 21, 23, 17 so shifts 20 (i-2), 22 (i), and 16 (i-6)
    for (int i=22; i<=24; i++)
        num_adjacent[i]=((current&(1<<(i-2)))?1:0)+((current&(1<<i))?1:0)+((current&(1<<(i-6)))?1:0);
    
    // 3 middle rows
    for (int i=1; i<=3; i++)
    {
        int left_idx=5*i+1;
        // left cell
        // 6 gets from positions 1, 7, 11 so shifts 0 (left_idx-6), 6 (left_idx), and 10 (left_idx+4)
        num_adjacent[left_idx]=((current&(1<<(left_idx-6)))?1:0)+((current&(1<<left_idx))?1:0)+((current&(1<<(left_idx+4)))?1:0);

        int right_idx=left_idx+4;
        // right cell
        // 10 gets from positions 5, 9, 15, so shifts 4 (right_idx-6), 8 (right_idx-2), and 14 (right_idx+4)
        num_adjacent[right_idx]=((current&(1<<(right_idx-6)))?1:0)+((current&(1<<(right_idx-2)))?1:0)+((current&(1<<(right_idx+4)))?1:0);
        
        // ok, do the middle cells now
        for (int j=left_idx+1; j<=left_idx+3; j++)
        {
            // middle cell
            // 7 gets from positions 2, 6, 8, and 12, so shifts 1 (j-6), 5 (j-2), 7 (j), 11 (j+4)
            num_adjacent[j]=((current&(1<<(j-6)))?1:0)+((current&(1<<(j-2)))?1:0)+((current&(1<<j))?1:0)+((current&(1<<(j+4)))?1:0);
        }
    }
    for (int i=1; i<=25; i++)
    {
        if (current&(1<<(i-1)) && num_adjacent[i]!=1) // if there is a bug at i and not only one adjacent to it, kill the bug
        {
            printf("killing bug at %d\n", i);
            next=next^(1<<(i-1)); // flip the i-1 bit using XOR to kill the bug
        }
        else if ((!(current&(1<<(i-1)))) && (num_adjacent[i]==1 || num_adjacent[i]==2)) // if there is no bug at i and one or two to it, make a bug
        {
            printf("spawning bug at %d\n", i);
            next=next^(1<<(i-1)); // flip the i-1 bit using XOR to spawn the bug
        }
        else
        {
            printf("not doing anything to bug at %d with num-adjacent=%d current=%d masked_current=%d\n", i, num_adjacent[i], current, current&(1<<(i-1)));
        }
    }
    return next;
}

void printGame(uint32_t current)
{
    for (int i=0; i<=24; i++)
    {
        char ch=(current&(1<<i))?BUG:EMPTY;
        printf("%c", ch);
        if (i%5==4)
            printf("\n");
    }
}

int main (int argc, char * argv[])
{
    FILE * infile=NULL;
    char buffer[256];
    
    memset(&buffer, '\0', sizeof(buffer));
    
    uint32_t current=0;
    uint32_t iterations[MAX_ROUNDS];
    uint32_t round=0;
    
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }
    
    fprintf(stderr, "Opening file %s\n", argv[1]);
    
    infile=fopen(argv[1], "r");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        exit(1);
    }
    
    int pos=1;
    while (fgets(buffer, sizeof(buffer), infile))
    {
        char * eol = strchr(buffer, '\r');
        if (eol)
        {
            *eol='\0';
        }
        eol = strchr(buffer, '\n');
        if (eol)
        {
            *eol='\0';
        }
        
        for (int i=0; i<5; i++)
        {
            if (buffer[i]==BUG)
            {
                current=current|pos;
            }
            pos=pos<<1;
        }
    }
    
    fclose(infile);
    
    printf("Starting position is:\n");
    printGame(current);
    
    iterations[0]=current;
    int found=0;
    for (int i=1; (i<MAX_ROUNDS&&found==0); i++)
    {
        
        iterations[i]=getNext(iterations[i-1]);
        printf("Iteration %d:\n", i);
        printGame(iterations[i]);
        for (int j=0; j<i; j++)
        {
            if (iterations[i]==iterations[j])
            {
                printf("Solution found with value %d\n", iterations[i]);
                found=1;
            }
        }
    }
    
    if (found==0)
    {
        printf("No loop found in first %d iterations\n", MAX_ROUNDS);
    }
    
    return 0;
}
