//MASM, QEMU
//even = rand()&0xfffffffe;
//odd = rand()|1;
//pipes for communication between threads or via variables
//at the begining one thread the main and it creates other two threads. after the main threads ends the other two must work to print and output in upper case
//three threads that work in three variables, this, next, last

#include <stdio.h>
#include <stdlib.h>

void sort(int *vector, int length);
void writeText(int *vector, char* filename, int length);
void writeBinary(int *vector, char* filename, int length);


int main(int argc, char** argv){
	if(argc != 3){
		printf("Usage: %s n1 n2\n",argv[0]);
		exit(0);
	}
	int* v1 = malloc(atoi(argv[1]) * sizeof(int*));
	int* v2 = malloc(atoi(argv[2]) * sizeof(int*));

	for(int i=0; i<atoi(argv[1]); i++){
		int r = (rand() % (100-10) + 10)|1 ;
		v1[i] = r;	
	}

	for(int i=0; i<atoi(argv[2]); i++){
		int r = (rand() % (40) + 10)*2;
		v2[i] = r;	
	}	
	
	writeBinary(v1, "fv.b", atoi(argv[1]));

	sort(v1, atoi(argv[1]));
	sort(v2, atoi(argv[2]));
	
	writeBinary(v1, "fv1.b", atoi(argv[1]));
	writeBinary(v2, "fv2.b", atoi(argv[2]));	

	writeText(v1,"fv1.txt", atoi(argv[1]));
	writeText(v2,"fv2.txt", atoi(argv[2]));
}

void sort(int *vector, int length){
	for(int i=0;i<length;i++){
		for(int k=i+1; k<length;k++){
			if(vector[i]>vector[k]){
				int tmp = vector[i];
				vector[i] = vector[k];
				vector[k] = tmp;
			}
		}
	}
}

void writeText(int *vector,char* filename, int length){
	FILE *fpw;
	if ((fpw = fopen (filename, "w")) == NULL){
		fprintf(stderr," error open %s\n", filename);
		exit(1);
	}
	fwrite(vector, length, sizeof(int), fpw); 
	fclose(fpw);
}

void writeBinary(int *vector, char* filename, int length ){
	FILE *fpw; 
	if ((fpw = fopen (filename, "wb")) == NULL){
		fprintf(stderr," error open %s\n", filename);
		exit(1);
	}
	fwrite(vector, length, sizeof(int), fpw); 
	fclose(fpw);
}
