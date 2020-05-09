#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ROUNDS 200
#define ALL_DEPTHS ((2*ROUNDS)+3) // can add 1 depth positive and negative each minute. want to keep a single extra one on the outside of the max

#define BUG '#'
#define EMPTY '.'
#define GRID '?'

struct grid
{
    char map[5][5];
};

typedef struct grid grid;

struct world
{
    grid grids[ALL_DEPTHS];
};

typedef struct world world;

int notEmpty(grid * g)
{
    for (int i=0; i<5; i++)
    {
        for (int j=0; j<5; j++)
        {
            if (g->map[i][j]==BUG)
            {
                return 1;
            }
        }
    }
    return 0;
}

void printWorld(world * world )
{
    for (int i=0; i<ALL_DEPTHS; i++)
    {
        if (notEmpty(&world->grids[i]))
        {
            printf("Depth: %d\n", i-(ROUNDS+1));
            for (int j=0; j<5; j++)
            {
                for (int k=0; k<5; k++)
                {
                    printf("%c", world->grids[i].map[j][k]);
                }
                printf("\n");
            }
        }
    }
}

/*

     |     |         |     |     
  1  |  2  |    3    |  4  |  5  
     |     |         |     |     
-----+-----+---------+-----+-----
     |     |         |     |     
  6  |  7  |    8    |  9  |  10 
     |     |         |     |     
-----+-----+---------+-----+-----
     |     |A|B|C|D|E|     |     
     |     |-+-+-+-+-|     |     
     |     |F|G|H|I|J|     |     
     |     |-+-+-+-+-|     |     
 11  | 12  |K|L|?|N|O|  14 |  15 
     |     |-+-+-+-+-|     |     
     |     |P|Q|R|S|T|     |     
     |     |-+-+-+-+-|     |     
     |     |U|V|W|X|Y|     |     
-----+-----+---------+-----+-----
     |     |         |     |     
 16  | 17  |    18   |  19 |  20 
     |     |         |     |     
-----+-----+---------+-----+-----
     |     |         |     |     
 21  | 22  |    23   |  24 |  25 
     |     |         |     |     

if the lettered cells are depth 0, the numbered cells are depth -1, and there is another grid of numbered cells at ? with depth 1.
clear as mud, right?

-- tuples given as [depth][row][column] rows are 0-4, cols are 0-4.

A: [0][0][0] has 4 neighbors  8 [-1][1][2], 12 [-1][2][1],  B  [0][0][1],  F  [0][1][0]
B: [0][0][1] has 4 neighbors  8 [-1][1][2],  A  [0][0][0],  C  [0][0][2],  G  [0][1][1]
C: [0][0][2] has 4 neighbors  8 [-1][1][2],  B  [0][0][1],  D  [0][0][3],  H  [0][1][2]
D: [0][0][3] has 4 neighbors  8 [-1][1][2],  C  [0][0][2],  E  [0][0][4],  I  [0][1][3]
E: [0][0][4] has 4 neighbors  8 [-1][1][2],  D  [0][0][3], 14 [-1][2][3],  J  [0][1][4]
F: [0][1][0] has 4 neighbors  A  [0][0][0], 12 [-1][2][1],  G  [0][1][1],  K  [0][2][0]
G: [0][1][1] has 4 neighbors  B  [0][0][1],  F  [0][1][0],  H  [0][1][2],  L  [0][2][1]
H: [0][1][2] has 8 neighbors  C  [0][0][2],  G  [0][1][1],  I  [0][1][3],  5 from top    row in deeper [1][0][0-4] 
I: [0][1][3] has 4 neighbors  D  [0][0][3],  H  [0][1][2],  J  [0][1][4],  N  [0][2][3]
J: [0][1][4] has 4 neighbors  E  [0][0][4],  I  [0][1][3], 14 [-1][2][3],  O  [0][2][4]
K: [0][2][0] has 4 neighbors  F  [0][1][0], 12 [-1][2][1],  L  [0][2][1],  P  [0][3][0]
L: [0][2][1] has 8 neighbors  G  [0][1][1],  K  [0][2][0],  Q  [0][3][1],  5 from left   row in deeper [1][0-4][0]
N: [0][2][3] has 8 neighbors  I  [0][1][3],  O  [0][2][4],  S  [0][3][3],  5 from right  row in deeper [1][0-4][4]
O: [0][2][4] has 4 neighbors  J  [0][1][4],  N  [0][2][3], 14 [-1][2][3],  T  [0][3][4]
P: [0][3][0] has 4 neighbors  K  [0][2][0], 12 [-1][2][1],  Q  [0][3][1],  U  [0][4][0]
Q: [0][3][1] has 4 neighbors  L  [0][2][1],  P  [0][3][0],  R  [0][3][2],  V  [0][4][1]
R: [0][3][2] has 8 neighbors  Q  [0][3][1],  S  [0][3][3],  W  [0][4][2],  5 from bottom row in deeper [1][4][0-4]
S: [0][3][3] has 4 neighbors  N  [0][2][3],  R  [0][3][2],  T  [0][3][4],  X  [0][4][3]
T: [0][3][4] has 4 neighbors  O  [0][2][4],  S  [0][3][3], 14 [-1][2][3],  Y  [0][4][4]
U: [0][4][0] has 4 neighbors  P  [0][3][0], 12 [-1][2][1],  V  [0][4][1], 18 [-1][3][2]
V: [0][4][1] has 4 neighbors  Q  [0][3][1],  U  [0][4][0],  W  [0][4][2], 18 [-1][3][2]
W: [0][4][2] has 4 neighbors  R  [0][3][2],  V  [0][4][1],  X  [0][4][3], 18 [-1][3][2]
X: [0][4][3] has 4 neighbors  S  [0][3][3],  W  [0][4][2],  Y  [0][4][4], 18 [-1][3][2]
Y: [0][4][4] has 4 neighbors  T  [0][3][4],  X  [0][4][3], 14 [-1][2][3], 18 [-1][3][2]
*/

int bugVal(char ch)
{
    if (ch==BUG)
        return 1;
    return 0;
}

void workWorld(world * w)
{
    world tmp;
    for (int i=0; i<ALL_DEPTHS; i++)
    {
        for (int j=0; j<5; j++)
        {
            for (int k=0; k<5; k++)
            {
                tmp.grids[i].map[j][k]=w->grids[i].map[j][k];
            }
        }
    }
    for (int i=1; i<ALL_DEPTHS-1; i++) // work through all of the possible worlds where we would have bugs.
    {
        //printf("Working for depth %d\n", i-(ROUNDS+1));
        if (!(notEmpty(&tmp.grids[i-1]) || notEmpty(&tmp.grids[i]) || notEmpty(&tmp.grids[i+1])))
        {
            //printf("  Skipping working depth %d because depths %d %d and %d are all empty\n", i-(ROUNDS+1), i-(ROUNDS+1)-1, i-(ROUNDS+1), i-(ROUNDS+1)+1);
            continue;
        }
        
        int num_adjacent[5][5];

        num_adjacent[0][0]=bugVal(tmp.grids[i-1].map[1][2]) + bugVal(tmp.grids[i-1].map[2][1]) + bugVal(tmp.grids[i].map[0][1])   + bugVal(tmp.grids[i].map[1][0]);
        num_adjacent[0][1]=bugVal(tmp.grids[i-1].map[1][2]) + bugVal(tmp.grids[i].map[0][0])   + bugVal(tmp.grids[i].map[0][2])   + bugVal(tmp.grids[i].map[1][1]);
        num_adjacent[0][2]=bugVal(tmp.grids[i-1].map[1][2]) + bugVal(tmp.grids[i].map[0][1])   + bugVal(tmp.grids[i].map[0][3])   + bugVal(tmp.grids[i].map[1][2]);
        num_adjacent[0][3]=bugVal(tmp.grids[i-1].map[1][2]) + bugVal(tmp.grids[i].map[0][2])   + bugVal(tmp.grids[i].map[0][4])   + bugVal(tmp.grids[i].map[1][3]);
        num_adjacent[0][4]=bugVal(tmp.grids[i-1].map[1][2]) + bugVal(tmp.grids[i].map[0][3])   + bugVal(tmp.grids[i-1].map[2][3]) + bugVal(tmp.grids[i].map[1][4]);
        num_adjacent[1][0]=bugVal(tmp.grids[i].map[0][0])   + bugVal(tmp.grids[i-1].map[2][1]) + bugVal(tmp.grids[i].map[1][1])   + bugVal(tmp.grids[i].map[2][0]);
        num_adjacent[1][1]=bugVal(tmp.grids[i].map[0][1])   + bugVal(tmp.grids[i].map[1][0])   + bugVal(tmp.grids[i].map[1][2])   + bugVal(tmp.grids[i].map[2][1]);
        num_adjacent[1][2]=bugVal(tmp.grids[i].map[0][2])   + bugVal(tmp.grids[i].map[1][1])   + bugVal(tmp.grids[i].map[1][3])   + bugVal(tmp.grids[i+1].map[0][0]) + 
                           bugVal(tmp.grids[i+1].map[0][1]) + bugVal(tmp.grids[i+1].map[0][2]) + bugVal(tmp.grids[i+1].map[0][3]) + bugVal(tmp.grids[i+1].map[0][4]);
        num_adjacent[1][3]=bugVal(tmp.grids[i].map[0][3])   + bugVal(tmp.grids[i].map[1][2])   + bugVal(tmp.grids[i].map[1][4])   + bugVal(tmp.grids[i].map[2][3]);
        num_adjacent[1][4]=bugVal(tmp.grids[i].map[0][4])   + bugVal(tmp.grids[i].map[1][3])   + bugVal(tmp.grids[i-1].map[2][3]) + bugVal(tmp.grids[i].map[2][4]);
        num_adjacent[2][0]=bugVal(tmp.grids[i].map[1][0])   + bugVal(tmp.grids[i-1].map[2][1]) + bugVal(tmp.grids[i].map[2][1])   + bugVal(tmp.grids[i].map[3][0]);
        num_adjacent[2][1]=bugVal(tmp.grids[i].map[1][1])   + bugVal(tmp.grids[i].map[2][0])   + bugVal(tmp.grids[i].map[3][1])   + bugVal(tmp.grids[i+1].map[0][0]) + 
                           bugVal(tmp.grids[i+1].map[1][0]) + bugVal(tmp.grids[i+1].map[2][0]) + bugVal(tmp.grids[i+1].map[3][0]) + bugVal(tmp.grids[i+1].map[4][0]); 
        num_adjacent[2][2]=0;
        num_adjacent[2][3]=bugVal(tmp.grids[i].map[1][3])   + bugVal(tmp.grids[i].map[2][4])   + bugVal(tmp.grids[i].map[3][3])   + bugVal(tmp.grids[i+1].map[0][4]) + 
                           bugVal(tmp.grids[i+1].map[1][4]) + bugVal(tmp.grids[i+1].map[2][4]) + bugVal(tmp.grids[i+1].map[3][4]) + bugVal(tmp.grids[i+1].map[4][4]); 
        num_adjacent[2][4]=bugVal(tmp.grids[i].map[1][4])   + bugVal(tmp.grids[i].map[2][3])   + bugVal(tmp.grids[i-1].map[2][3]) + bugVal(tmp.grids[i].map[3][4]);
        num_adjacent[3][0]=bugVal(tmp.grids[i].map[2][0])   + bugVal(tmp.grids[i-1].map[2][1]) + bugVal(tmp.grids[i].map[3][1])   + bugVal(tmp.grids[i].map[4][0]);
        num_adjacent[3][1]=bugVal(tmp.grids[i].map[2][1])   + bugVal(tmp.grids[i].map[3][0])   + bugVal(tmp.grids[i].map[3][2])   + bugVal(tmp.grids[i].map[4][1]);
        num_adjacent[3][2]=bugVal(tmp.grids[i].map[3][1])   + bugVal(tmp.grids[i].map[3][3])   + bugVal(tmp.grids[i].map[4][2])   + bugVal(tmp.grids[i+1].map[4][0]) + 
                           bugVal(tmp.grids[i+1].map[4][1]) + bugVal(tmp.grids[i+1].map[4][2]) + bugVal(tmp.grids[i+1].map[4][3]) + bugVal(tmp.grids[i+1].map[4][4]);
        num_adjacent[3][3]=bugVal(tmp.grids[i].map[2][3])   + bugVal(tmp.grids[i].map[3][2])   + bugVal(tmp.grids[i].map[3][4])   + bugVal(tmp.grids[i].map[4][3]);
        num_adjacent[3][4]=bugVal(tmp.grids[i].map[2][4])   + bugVal(tmp.grids[i].map[3][3])   + bugVal(tmp.grids[i-1].map[2][3]) + bugVal(tmp.grids[i].map[4][4]);
        num_adjacent[4][0]=bugVal(tmp.grids[i].map[3][0])   + bugVal(tmp.grids[i-1].map[2][1]) + bugVal(tmp.grids[i].map[4][1])   + bugVal(tmp.grids[i-1].map[3][2]);
        num_adjacent[4][1]=bugVal(tmp.grids[i].map[3][1])   + bugVal(tmp.grids[i].map[4][0])   + bugVal(tmp.grids[i].map[4][2])   + bugVal(tmp.grids[i-1].map[3][2]);
        num_adjacent[4][2]=bugVal(tmp.grids[i].map[3][2])   + bugVal(tmp.grids[i].map[4][1])   + bugVal(tmp.grids[i].map[4][3])   + bugVal(tmp.grids[i-1].map[3][2]);
        num_adjacent[4][3]=bugVal(tmp.grids[i].map[3][3])   + bugVal(tmp.grids[i].map[4][2])   + bugVal(tmp.grids[i].map[4][4])   + bugVal(tmp.grids[i-1].map[3][2]);
        num_adjacent[4][4]=bugVal(tmp.grids[i].map[3][4])   + bugVal(tmp.grids[i].map[4][3])   + bugVal(tmp.grids[i-1].map[2][3]) + bugVal(tmp.grids[i-1].map[3][2]);
        
        for (int j=0; j<5; j++)
        {
            for (int k=0; k<5; k++)
            {
                //printf("num_adjacent[%d][%d]=%d\n", j, k, num_adjacent[j][k]);
                if (tmp.grids[i].map[j][k]==BUG && num_adjacent[j][k] != 1) // if there is a bug at [j][k] and not only one adjacent to it, kill the bug
                {
                    //printf("  clearing bug at depth %d [%d][%d] with num_adjacent=%d\n", i-(ROUNDS+1), j, k, num_adjacent[j][k]);
                    w->grids[i].map[j][k]=EMPTY;
                }
                else if (tmp.grids[i].map[j][k]==EMPTY && (num_adjacent[j][k]==1 || num_adjacent[j][k]==2)) // if there is no bug at i and one or two to it, make a bug
                {
                    //printf("  creating bug at depth %d [%d][%d] with num_adjacent=%d\n", i-(ROUNDS+1), j, k, num_adjacent[j][k]);
                    w->grids[i].map[j][k]=BUG;
                }
            }
        }
    }
}

int main (int argc, char * argv[])
{
    FILE * infile=NULL;
    char buffer[256];
    
    memset(&buffer, '\0', sizeof(buffer));
    
    world w;
    
    for (int i=0; i<ALL_DEPTHS; i++)
    {
        for (int j=0; j<5; j++)
        {
            for (int k=0; k<5; k++)
            {
                w.grids[i].map[j][k]=EMPTY;
            }
        }
        w.grids[i].map[2][2]=GRID;
    }

    int time=0;
    
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
    
    int row=0;
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
            w.grids[ROUNDS+1].map[row][i]=buffer[i];
        }
        row++;
    }
    
    w.grids[ROUNDS+1].map[2][2]=GRID;
    
    fclose(infile);
    
    printf("Starting position is:\n");
    printWorld(&w);
    
    for (int i=1; i<=ROUNDS; i++)
    {
        workWorld(&w);
        printf("After minute %d:\n", i);
        printWorld(&w);
    }

    int bugCount=0;
    for (int i=0; i<ALL_DEPTHS; i++)
    {
        for (int j=0; j<5; j++)
        {
            for (int k=0; k<5; k++)
            {
                if (w.grids[i].map[j][k]==BUG)
                {
                    bugCount++;
                }
            }
        }
    }
    
    printf("Final bug count after %d minutes is %d\n", ROUNDS, bugCount);
    return 0;
}
