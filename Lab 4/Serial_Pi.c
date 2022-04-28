#include <stdio.h>
#include <omp.h>  



int main()
{
    long long num_steps = 1000000000;
    double step = 1./(double)num_steps;
    double x; 
    double pi;
    double sum = 0.0;

    int i;
    double start = omp_get_wtime();
    for (i = 0 ; i < num_steps ; i++)
    {
        x = (i + .5)*step;
        sum = sum + 4.0/(1.+ x*x);
    }

    pi = sum*step;
    double time = omp_get_wtime() - start;

    printf("The value of PI is %15.12f\n",pi);
    printf("The time to calculate PI in serial is %f seconds\n",time);
return 0;
}
