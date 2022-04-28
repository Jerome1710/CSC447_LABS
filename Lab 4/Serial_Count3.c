#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


int main ()
{
    /*
    Length 100,000,000 for time testing purposes
    Create array and allocate memory for it
    */
    int length = 100000000;
    int * array;
    array = calloc(length, sizeof(int));

    int i; 

    /*
    Randomize the numbers in the array 0-9
    */
    srand(time(NULL));
    for (i = 0 ; i < length ; i++)
    {
        array[i] = rand()%10;
    }

    /*
    Serial count 3s in the array
    */
    
    double start = omp_get_wtime();
    int count = 0;
    for (i = 0 ; i < length ; i++)
    {
        if (array[i] == 0)
        {
            count +=1;
        }
    }
    
    double time = omp_get_wtime() - start;
    printf("The total number of 3s in the array is: %d\n", count);
    printf("The total amount of time taken to run the serial code is: %f\n", time);


    return 0;
}