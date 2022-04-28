#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define QUEUESIZE 5
#define WORK_MAX 30
#define PRODUCER_CPU   25
#define PRODUCER_BLOCK 10
#define CONSUMER_CPU   25
#define CONSUMER_BLOCK 10

//This part of the code makes random waits for the consumer and producer
int memory_access_area[100000];
void msleep(unsigned int milli_seconds)
{
  struct timespec req = {0}; /* init struct contents to zero */
  time_t          seconds;

  seconds        = (milli_seconds/1000);
  milli_seconds  = milli_seconds - (seconds * 1000);

  req.tv_sec  = seconds;
  req.tv_nsec = milli_seconds * 1000000L;

  while(nanosleep(&req, &req)==-1) {
    printf("restless\n");
    continue;
  }

}
void do_work(int cpu_iterations, int blocking_time)
{
  int i;
  int j;
  int local_var;

  local_var = 0;
  for (j = 0; j < cpu_iterations; j++ ) {
    for (i = 0; i < 1000; i++ ) {
      local_var = memory_access_area[i];
    }
  }
  
  if ( blocking_time > 0 ) {
    msleep(blocking_time);
  }
}



typedef struct {
  int buf[QUEUESIZE];   //Buffer to store items
  int head;             //Next to remove
  int tail;             //Next to add

  int full;             // ==1 when full
  int empty;            // == 1 when empty

  pthread_mutex_t *mutex;     //Lock
  pthread_cond_t  *notFull;   //wait till not full to produce
  pthread_cond_t  *notEmpty;  //wait till not empty to consume
} queue;

typedef struct {
  queue *q;  //points to the queue and its mutex related stuff     
  int   *count; //points to a counter that tracks work 
  int    tid; //ID of each thead
} pcdata;

queue *queueInit (void)
{
  //Initialize the queue's variables, starts empty not full, head and tail start 0 and initialze the mutex and CVs
  queue *q;
  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);
  q->empty = 1;  
  q->full  = 0;   

  q->head  = 0;   
  q->tail  = 0;   
  q->mutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mutex, NULL);

  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);

  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  return (q);
}

void queueDelete (queue *q)
{
  //destroy and deallocate stuff
  pthread_mutex_destroy (q->mutex);
  free (q->mutex);
	

  pthread_cond_destroy (q->notFull);
  free (q->notFull);

  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);

  free (q);
}

void queueAdd (queue *q, int in)
{
 
  //Add the needed item into the buffer and increment the next items position
  q->buf[q->tail] = in;
  q->tail++;

  //wrap to be circular array
  if (q->tail == QUEUESIZE)
    q->tail = 0;

  //head == tail means we are full
  if (q->tail == q->head)
    q->full = 1;

  //Obviously the array aint empty since we just produced something
  q->empty = 0;

  return;
}

void queueRemove (queue *q, int *out)
{
  //Copy out elements and increment the pointer to next consume
  *out = q->buf[q->head];
  q->head++;

  //Wrap
  if (q->head == QUEUESIZE)
    q->head = 0;

  //if head == tail when consuming we are empty
  if (q->head == q->tail)
    q->empty = 1;

  //Queue cant be full if we just consumed
  q->full = 0;

  return;
}

void *producer (void *parg)
{
  //Intialize data from producer arguments
  queue  *fifo;
  int     item_produced;
  pcdata *mydata;
  int     my_tid;
  int    *total_produced;

  mydata = (pcdata *) parg;

  fifo           = mydata->q;
  total_produced = mydata->count;
  my_tid         = mydata->tid;


  while (1) 
  {
    
    do_work(PRODUCER_CPU, PRODUCER_BLOCK);

    //Lock before you start work
    pthread_mutex_lock(fifo->mutex);
    //wait when the queue is full and you're not done working condwait until it isnt full
    while (fifo->full && *total_produced < WORK_MAX) 
    {  
      pthread_cond_wait(fifo->notFull, fifo->mutex);
    }

    //before you decide that you no longer feel like existing, make sure you unlock and tell the consumers to continue working
    if (*total_produced >= WORK_MAX) {
      pthread_mutex_unlock(fifo->mutex);
      pthread_cond_signal(fifo->notEmpty);
      break;
    }

   
    item_produced = (*total_produced);
    (*total_produced)++;
    queueAdd (fifo, item_produced);
    
    

    printf("Producer %d: has produced(%d).\n", my_tid, item_produced);
    //make sure you unlock and tell the consumers to continue working
    pthread_mutex_unlock(fifo->mutex);
    pthread_cond_signal(fifo->notEmpty);

  }

  printf("Producer %d: has exited.\n", my_tid);
  return (NULL);
}

void *consumer (void *carg)
{
  queue  *fifo;
  int     item_consumed;
  pcdata *mydata;
  int     my_tid;
  int    *total_consumed;

  mydata = (pcdata *) carg;

  fifo           = mydata->q;
  total_consumed = mydata->count;
  my_tid         = mydata->tid;


  while (1) {

    //Lock before you start work
    pthread_mutex_lock(fifo->mutex);
    //when the queue is empty and you're not done working, condwait util it is not empty
    while (fifo->empty && *total_consumed < WORK_MAX) 
    {  
      pthread_cond_wait(fifo->notEmpty, fifo->mutex);
      
    }

    //when you are done working make sure you unlock so you dont block others and inform the producers
    if (*total_consumed == WORK_MAX) 
    {
      
      pthread_cond_broadcast(fifo->notFull);
      pthread_mutex_unlock(fifo->mutex);
      break;
    }

    do_work(CONSUMER_CPU,CONSUMER_CPU);
    queueRemove (fifo, &item_consumed);
    (*total_consumed)++;
    
    //make sure you unlock and tell the producers to continue working
    pthread_cond_signal(fifo->notFull);
    pthread_mutex_unlock(fifo->mutex);

    
    printf ("Consumer %d: has consumed item(%d).\n", my_tid, item_consumed);

  }

  printf("Consumer %d: has  exited.\n", my_tid);
  return (NULL);
}

int main (int argc, char *argv[])
{
  clock_t begin = clock();

  pthread_t *con;
  int        cons;
  int       *concount;

  queue     *fifo;
  int        i;

  pthread_t *pro;
  int       *procount;
  int        pros;

  pcdata    *thread_args;

  if (argc != 3) {
    printf("Usage: producer_consumer number_of_producers number_of_consumers\n");
    exit(0);
  }

  pros = atoi(argv[1]);
  cons = atoi(argv[2]);

  fifo = queueInit ();
  if (fifo ==  NULL) {
    fprintf (stderr, "main: Queue Init failed.\n");
    exit (1);
  }

  procount = (int *) calloc (1, sizeof (int));
  if (procount == NULL) { 
    fprintf(stderr, "procount allocation failed\n"); 
    exit(1); 
  }
  
  concount = (int *) calloc (1, sizeof (int));
  if (concount == NULL) { 
    fprintf(stderr, "concount allocation failed\n"); 
    exit(1); 
  }

  pro = (pthread_t *) malloc (sizeof (pthread_t) * pros);
  if (pro == NULL) { 
    fprintf(stderr, "pros\n"); 
    exit(1); 
  }

  con = (pthread_t *) malloc (sizeof (pthread_t) * cons);
  if (con == NULL) { 
    fprintf(stderr, "cons\n"); 
    exit(1); 
  }


  for (i=0; i<pros; i++){ 

    thread_args = (pcdata *)malloc (sizeof (pcdata));
    if (thread_args == NULL) {
      fprintf (stderr, "main: Thread_Args Init failed.\n");
      exit (1);
    }

    thread_args->q     = fifo;
    thread_args->count = procount;
    thread_args->tid   = i;
    pthread_create (&pro[i], NULL, producer, thread_args);
  }


  for (i=0; i<cons; i++){

    thread_args = (pcdata *)malloc (sizeof (pcdata));
    if (thread_args == NULL) {
      fprintf (stderr, "main: Thread_Args Init failed.\n");
      exit (1);
    }

    thread_args->q     = fifo;
    thread_args->count = concount;
    thread_args->tid   = i;
    pthread_create (&con[i], NULL, consumer, thread_args);
  }


  for (i=0; i<pros; i++)
    pthread_join (pro[i], NULL);
  for (i=0; i<cons; i++)
    pthread_join (con[i], NULL);

 
  queueDelete (fifo);


clock_t end = clock(); 
double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

printf("Time spent: %f", time_spent);
  return 0;
}







