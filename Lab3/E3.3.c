#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "semaphore.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <limits.h>

#define BUFFER_SIZE 16
#define OVER (-1)

typedef struct thread_data{
	int *list;
	int left;
	int right;
}thread_data ;

void swap(int v[], int i, int j);
void *quicksort (void *thread_arg);

void *quicksort (void *thread_arg){
	
	struct thread_data *t_data;
	t_data = (struct thread_data *) thread_arg;
	
	int i, j, x, tmp;
	int left = t_data->left;
	int right = t_data->right;
	if(left >= right)
		pthread_exit(0);
	int *v;
	v = t_data->list;
	x = v[left];
	i = left - 1;
	j = right + 1;
	while (i < j) {
		while (v[--j] > x);
		while (v[++i] < x);
		if (i < j)
			swap (v, i,j);
	}
	
	pthread_t th_a, th_b;
	struct thread_data td;
	td.list = v;
	td.left = left;
	td.right = j;
	pthread_create (&th_a, NULL, quicksort, (void *) &td);

	td.list = v;
	td.left = j+1;
	td.right = right;
	pthread_create (&th_b, NULL, quicksort, (void *) &td);

	pthread_join (th_a, NULL);
	pthread_join (th_b, NULL);

}

void swap(int v[], int i, int j){
	int tmp;
	tmp = v[i];
	v[i] = v[j];
	v[j] = tmp;
}

int main (void){
	int fd, len, pg;
	struct stat stat_buf;
	int *v;

	if ((fd = open ("fv.b", O_RDWR)) == -1)
		perror ("open");

	if (fstat(fd, &stat_buf) == -1)
		perror ("fstat");

	len = stat_buf.st_size;

	v = mmap (0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);	

	pthread_t th_1;
	struct thread_data td;
	td.list = v;
	td.left = 1;
	td.right = 22;
	pthread_create(&th_1, NULL, quicksort, (void *) &td);

	pthread_join(th_1, NULL);
	
	int i;
	for(i=0; i<23; i++)
		printf("array value at index %d is %d\n", i, v[i]);
		
	return 0;
}
