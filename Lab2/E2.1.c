#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>


int g = 0;
sem_t sem1, sem2, me, me_ind;
int indicator = 0;


void signal_handler(int my_signal){
	if(my_signal==SIGUSR1)
		exit(0);
}

void* client_thread(void* arg){
	char *value_ptr = (char*) arg;	
	FILE *fpr;
	fpr = fopen(value_ptr,"rb");
	int x = 0;
	pid_t tid = syscall(SYS_gettid);
	while(fread(&x, sizeof(int), 1, fpr) > 0 ){
		sem_wait(&me);		
		g = x;
		sem_post(&sem1);
		sem_wait(&sem2);
		printf("result = %d: tid = %d\n",g, tid);
		sem_post(&me);
			
	}
	sem_wait(&me_ind);
	indicator++;	
	sem_post(&me_ind);

	

	if(indicator == 2)
		kill(getpid(),SIGUSR1);	

	pthread_exit(0);
}

void main(int argc, char** argv){
	
	pthread_t tid_1, tid_2;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if (sem_init(&me, 0, 1) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
        }

	if (sem_init(&me_ind, 0, 1) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
        }

	if (sem_init(&sem1, 0, 0) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
        }

	if (sem_init(&sem2, 0, 0) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
        }

	char *filename1 = "fv1.b";
	char *filename2 = "fv2.b";

	pthread_create(&tid_1, &attr, client_thread, (void *)filename1);
	pthread_create(&tid_2, &attr, client_thread, (void *)filename2);

	signal(SIGUSR1,signal_handler);
	while(1){
		/*if(indicator == 2){
			exit(0);
		}*/
		sem_wait(&sem1);
		g = g*3;
		sem_post(&sem2);
	}

	pthread_join(tid_1,NULL);
	pthread_join(tid_2,NULL);
}
