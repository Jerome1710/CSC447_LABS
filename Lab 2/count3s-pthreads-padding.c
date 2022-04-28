#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define MaxThreads 1000
#define ArrayLength 1048576
#define NumberOfThreads 12

void* count3(void* ID);
int * array;

struct intistruct
{
    int value;
    char padding[60];
}
thread_counter[MaxThreads];

int global_counter;

int main () 
{
    int i;//Counter integer for the loops

    //INITIALIZE ARRAY PART
    array = calloc(ArrayLength, sizeof(int));
    srand(time(NULL));
    for (i = 0 ; i < ArrayLength ; i++)
    {
        array[i] = rand() % 10;
    }
    //INITIALIZE ARRAY PART

    //INITIALIZE THREADS PART
    pthread_t tID[MaxThreads];
    for (i = 0 ; i < NumberOfThreads ; i++)
    {
        pthread_create(&tID[i], NULL, count3, (void*)i);
    }

    for (i = 0 ; i < NumberOfThreads ; i++)
    {
        pthread_join(tID[i], NULL);
    }
    //INITIALIZE THREADS PART

    printf("The number of 3's is %d\n", global_counter);

   /* As a test, let us count 3's the slow, serial way. */
   global_counter = 0;
   for(i = 0; i < ArrayLength; i++)
      if(array[i] == 3)
         global_counter++;
   printf("The number of 3's is %d\n", global_counter);

   return 0;
}

    //FUNCTION TO BE PASSED TO THE THREAD
    void* count3(void* ID)
    {
        //Each thread works on length/total number of threads
        int length_per_thread = ArrayLength / NumberOfThreads;
        //int thread_ID = (int)ID;
        int start_index = (int)ID * length_per_thread;
        int i;
        for (i = start_index ; i < (start_index + length_per_thread) ; i++)
        {
            if (array[i] == 3)
            {
                thread_counter[(int)ID].value++;
            }
        }

        global_counter += thread_counter[(int)ID].value;

        return 0;
    }
    //FUNCTION TO BE PASSED TO THE THREAD
