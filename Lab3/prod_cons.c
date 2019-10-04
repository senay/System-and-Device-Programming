#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "semaphore.h"

#define BUFFER_SIZE 16
#define OVER (-1)

typedef struct Buffer{
  sem_t *empty, *full, *me_p, *me_c;
  int in, out;
  int *buffer;
  int size;
}Buffer;

Buffer * buffer_init(int size);

Buffer *b;


Buffer *
buffer_init(int size){
  b = (Buffer *) malloc(sizeof(Buffer));
  b->size = size;
  b->buffer = (int *) malloc(b->size * sizeof(int));
  b->in = 0;
  b->out = 0;
  b->empty = (sem_t *) malloc(sizeof(sem_t)); sem_init (b->empty, 0, b->size);
  b->full = (sem_t *) malloc(sizeof(sem_t));  sem_init (b->full, 0, 0);
  b->me_p = (sem_t *) malloc(sizeof(sem_t));  sem_init (b->me_p, 0, 1);  // not necessary for single producer
  b->me_c = (sem_t *) malloc(sizeof(sem_t));  sem_init (b->me_c, 0, 1);  // not necessary for single consumer

  return b;
}

/* Store an integer in the buffer */
void 
put (int data){
  sem_wait(b->empty);
  b->buffer[b->in] = data;
  b->in = (b->in + 1) % b->size;
  sem_post(b->full);
}

/* Read and remove an integer from the buffer */
int 
get (void){
  int data;
  
  sem_wait(b->full);
  data = b->buffer[b->out];
  b->out = (b->out + 1) % b->size;
  sem_post(b->empty);
  return data;
}

/* A test program: one thread inserts integers from 1 to 10000,
   the other reads and prints them. */

static void *
producer (void *data){
  int n;
  for (n = 0; n < 10000; n++) {
      printf ("%d --->\n", n);
      put (n);
    }
  put (OVER);
  return NULL;
}

static void *
consumer (void *data){
  int d;
  while (1){
      d = get();
      if (d == OVER)
	      break;
      printf ("---> %d\n", d);
    }
  return NULL;
}

int
main (void)
{
  pthread_t th_a, th_b;
  void *retval;

  buffer_init(BUFFER_SIZE);
  
  /* Create the threads */
  pthread_create (&th_a, NULL, producer, 0);
  pthread_create (&th_b, NULL, consumer, 0);
  /* Wait until producer and consumer finish. */
  pthread_join (th_a, &retval);
  pthread_join (th_b, &retval);
  return 0;
}

