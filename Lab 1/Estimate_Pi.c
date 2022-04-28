#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <time.h>

#define ARRAY_SIZE 1000000 //Change this according to what you want 1000, 10000, 100000, 1000000


//This function I used to generate a random float between [0, 1]
float generateCoords()
{
    int x = rand();
    float coord = x / (float) RAND_MAX;
    return coord;
}

int main()
{
    float inCircle = 0;
    float inSquare = 0;
    //float x_coord[ARRAY_SIZE];
    //float y_coord[ARRAY_SIZE];

    int i;

    for (i = 0 ; i < ARRAY_SIZE ; i++)
    {
        //x_coord[i] = generateCoords; couldnt use these as i got error 3221225725 which according to what i looked up means i ran out of memory
        //y_coord[i] = generateCoords;

        /*
        generate a float between 0 and 1 that is then assigned to the x and y coordinate of the "dart"
        signifiy where it is landing on the graph/board
        */
        float x = generateCoords();
        float y = generateCoords();

        /*
        we use the circle formula here and check if the value is less than 1, 
        if yes that means the dart landed inside the circle
        */
        float isCircle = sqrt((x*x) + (y*y)); 
        
        if (isCircle < 1)
        {
            inCircle +=1; //If in the circle increase the number of darts successfully hit
        }
    }

    float estimated_pi = 4 * inCircle / ARRAY_SIZE;
    printf("%f" ,estimated_pi);
    return 0;
}