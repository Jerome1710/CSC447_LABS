#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int * array;
int length;
int count;

int count3s()
{
   int i;
   count = 0;
   int count_p = 0; 
  
   omp_set_num_threads(8);
   #pragma omp parallel firstprivate(count_p) 
   {
      #pragma omp for 
      for(i = 0; i < length; i++)
      {
         if(array[i] == 3)
         {
            count_p++;
         }
      }
      #pragma omp critical
      {
         count += count_p;
      }
   }
   
   
   return count;
}


int main(int argc, char *argv[])
{
   /*
   Create array and allocate memory
   */
   int i;
   length = 100000000;  
   array = calloc(length, sizeof(int));


   /*
   Fill array with random number 0-9
   */
   srand(time(NULL));  
   for(i = 0; i < length; i++)
   {
      array[i] = rand()%10;
   }

   double start = omp_get_wtime();
   count3s(); 
   double time = omp_get_wtime() - start;
   printf("Time taken: %f\n", time);
   printf("The number of 3's is %d\n", count);
   
   return 0;
}