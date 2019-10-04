#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>

sem_t s, sem2, me, me_ind;

int wait_with_timeout(sem_t *S, int tmax){
	struct timespec ts;
	ts.tv_sec = time (NULL) + tmax;
	ts.tv_nsec = 0;
	int k = 0;
	printf("main() about to call sem_timedwait()\n");
	while ((k = sem_timedwait(S, &ts)) == -1 && errno == EINTR)
		continue;       /* Restart if interrupted by handler */


	/* Check what happened */


	if (k == -1) {
		printf("wait on semaphore s returned for timeout\n");
	}
	else
		printf("wait returned normally.\n");


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

	if (sem_init(&s, 0, 0) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
        }

	pthread_t tid_1, tid_2;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int tmax = atoi(argv[1]);
	pthread_create(&tid_1, &attr, th1_function, &tmax );
	pthread_create(&tid_2, &attr, th2_function, NULL);

	pthread_join(tid_1,NULL);
	pthread_join(tid_2,NULL);
}
