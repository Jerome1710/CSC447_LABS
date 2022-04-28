#include <stdio.h>
#include <stdlib.h>

//This will be the size of the array which we will count 3s on
#define ARRAYSIZE 1000000
int * array;
int main()
{

    //Create an array of the given size and then randomly generate number from 0-9
    array = calloc(ARRAYSIZE, sizeof(int));
    int i;
    srand(time(NULL));//Seeds based on variable time
    for (i = 0 ; i < ARRAYSIZE ; i++)
    {
        array[i] = rand()%10;
    }

    int threes = count3s(array);
    printf ("The number of threes in the array is: %d", threes);
}

/*
Basic function, iterate over entire array, if element == 3 increment counter then return in the end.
*/
int count3s (int * array)
{
    int counter = 0;
    int i;
    for (i = 0 ; i < ARRAYSIZE ; i++)
    {
        if (array[i] == 3)
        {
            counter +=1;
        }
    }
    return counter;
}