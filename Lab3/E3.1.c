#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "semaphore.h"
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>

#define BUFFER_SIZE 16
#define OVER (-1)

sem_t empty_normal, full_normal, empty_urgent, full_urgent, full_any;

typedef struct Buffer{
	int in, out;
	long long *buffer;
	int size;
}Buffer;

Buffer * buffer_init(int size);

Buffer* normal;
Buffer* urgent;

Buffer* buffer_init(int size){
	normal = (Buffer *) malloc(sizeof(Buffer));
	normal->size = size;
	normal->buffer = (long long *) malloc(normal->size * sizeof(long long));
	normal->in = 0;
	normal->out = 0;
	return normal;
}

Buffer* buffer_init_u(int size){
	urgent = (Buffer *) malloc(sizeof(Buffer));
	urgent->size = size;
	urgent->buffer = (long long *) malloc(urgent->size * sizeof(long long));
	urgent->in = 0;
	urgent->out = 0;
	return urgent;
}

long long current_timestamp() {
	struct timeval te;
	gettimeofday(&te, NULL); // get current time
	long long milliseconds = te.tv_sec*1000LL;
	return milliseconds;
}

/* Store an integer in the buffer */
void put (long long data, Buffer* buf, sem_t *empty, sem_t *full){
	sem_wait(empty);
	buf->buffer[buf->in] = data;
	buf->in = (buf->in + 1) % buf->size;
	sem_post(&full_any);
	sem_post(full);
}

/* Read and remove an integer from the buffer */
int get (void){
	int data; 

	sem_wait(&full_any);
	int k;
	if((k = sem_trywait(&full_urgent)) == 0) {
		printf("urgent semaphore acquired\n");
		data = urgent->buffer[urgent->out];
		urgent->out = (urgent->out + 1) % urgent->size;
		sem_post(&empty_urgent);
		printf("read %d from buffer urgent\n", data);
	} 
	else{
		printf("need to wait for semaphore\n");
		sem_wait(&full_normal);
		data = normal->buffer[normal->out];
		normal->out = (normal->out + 1) % normal->size;
		printf("read %d from buffer normal\n", data);
		sem_post(&empty_normal);
	}   
	sem_wait(&full_any);
	return data;
}


/* A test program: one thread inserts integers from 1 to 10000,
the other reads and prints them. */

static void *producer(void *data){
	int n;
	for (n = 0; n < 10000; n++) {
		int x = 0;
		x = (rand() % (10-1) + 1);
		sleep(x/1000);
		long long ms = current_timestamp();
		x = rand() % 100;
		if(x < 20){
			put(ms, urgent, &empty_urgent, &full_urgent);
			printf("putting %lld in buffer urgent\n", ms);
		}
		else{
			put (ms, normal, &empty_normal, &full_normal);
			printf("putting %lli in buffer normal\n", ms);
		}
	}
	put(OVER, normal, &empty_normal, &full_normal);
	return NULL;
}

static void *consumer(void *data){
	int d;
	while (1){
		sleep(0.01);
		d = get();
		if (d == OVER)
		      break;
	}
	return NULL;
}

int main (void){
	if (sem_init(&empty_normal, 0, BUFFER_SIZE) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	if (sem_init(&full_normal, 0, 0) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	if (sem_init(&empty_urgent, 0, BUFFER_SIZE) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	if (sem_init(&full_urgent, 0, 0) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	if (sem_init(&full_any, 0, 0) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}	

	pthread_t th_a, th_b;
	void *retval;

	buffer_init(BUFFER_SIZE);
	buffer_init_u(BUFFER_SIZE);

	/* Create the threads */
	pthread_create (&th_a, NULL, producer, 0);
	pthread_create (&th_b, NULL, consumer, 0);
	/* Wait until producer and consumer finish. */
	pthread_join (th_a, &retval);
	pthread_join (th_b, &retval);
	return 0;
}

