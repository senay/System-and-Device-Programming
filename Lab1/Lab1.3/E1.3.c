#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

char next; // to store charcter read from file by the main thread
char this; // to be transformed to upper case by each processing thread
char last; // to be printed by the output thread

void* processor_thread(void* arg)
{
	char *value_ptr = (char*) arg;	
	char value = *value_ptr;


	if(value >= 'a' && value <= 'z') {
		last = value - 32;	
	}
	else
		last = value;
	pthread_exit(0);
}

void* output_thread(void* arg)
{
	putc(last, stdout);	
}

void main(int argc, char** argv){
	if(argc != 2){
		printf("Usage: %s filename\n", argv[0]);
		exit(-1);
	}

	pthread_t tid_P, tid_O;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	FILE *fpr;
	fpr = fopen(argv[1],"r");
	
	//read the first character
	next = getc(fpr);

	this = next;//synchronization point
	
	//create thread to process the first input
	pthread_create(&tid_P, &attr, processor_thread, &this);
	//read the second character while processor thread is changing the character to upper case
	next = getc(fpr);
	//wait on the processing thread to finish
	pthread_join(tid_P, NULL);

	this = next;//synchronization point

	
	pthread_create(&tid_P, &attr, output_thread, &last);
	pthread_create(&tid_O, &attr, processor_thread, &this);	
	next = getc(fpr);

	pthread_join(tid_O, NULL);
	pthread_join(tid_P, NULL);

	this = next; 	//synchronization point

	while((next = getc(fpr)) != EOF){
		pthread_create(&tid_O, &attr, output_thread, &last);
		pthread_create(&tid_P, &attr, processor_thread, &this);
		pthread_join(tid_O, NULL);
		pthread_join(tid_P, NULL);
		this = next;
	}

	pthread_create(&tid_O, &attr, output_thread, &last);
	pthread_create(&tid_P, &attr, processor_thread, &this);
	pthread_join(tid_P, NULL);
	pthread_join(tid_O, NULL);

	pthread_create(&tid_O, &attr, output_thread, &last);
	pthread_join(tid_O, NULL);
}
