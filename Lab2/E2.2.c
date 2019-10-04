#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

sem_t s;

void signal_handler(int my_signal){
	printf("wait on semaphore s returned for timeout\n");
	exit(0);
}

int wait_with_timeout(sem_t *S, int tmax){
	
	signal(SIGALRM, signal_handler);
   	alarm(tmax);
	
	sem_wait(&s);
	printf("wait returned normally\n");
}

void* th1_function(void* arg){
	int *value_ptr = (int*) arg;	
	int value = *value_ptr;

	int x = 0;
	x = rand()%6;
	sleep(x/1000);
	printf("waiting on semaphore after %d milliseconds\n", x*1000);
	wait_with_timeout(&s, value/1000);

	pthread_exit(0);
}

void* th2_function(void* arg){
	int x = 0;
	x = (rand() % (10000-1000) + 10);
	sleep(x/1000);
	printf("performing signal on semaphore s after %d milliseconds\n", x);
	
	sem_post(&s);

	pthread_exit(0);
}


void main(int argc, char** argv){
	
	int tmax = atoi(argv[1]);


	if (sem_init(&s, 0, 0) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
        }

	pthread_t tid_1, tid_2;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&tid_1, &attr, th1_function, &tmax);
	pthread_create(&tid_2, &attr, th2_function, NULL);

	pthread_join(tid_1,NULL);
	pthread_join(tid_2,NULL);
}
