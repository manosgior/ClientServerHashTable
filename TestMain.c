#include "HashTable.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

int ops = 0;
HashTable tb;

void *work(void *arg) {
	int randomOp;
	int pid = (int) arg;
	
	srand((unsigned)time(NULL));
	
	for (int i = 0; i < ops; i++) {
		randomOp = rand() % 3;
		
		switch(randomOp) {
			case 0:
				insert(tb, i, (void *) i);
				break;
			case 1:
				delete(tb, i);
				break;
			case 2:
				printf("%d\n", (int) get(tb, rand() % ops));
				break;
		}
	}
}


int main(int argc, char **argv) {
	int numThreads = 0;
	int hastTableSize = 0;
	
	if (argc != 4) {
		fprintf(stderr, "Usage: ./hash HASH_TABLE_SIZE NUMBER_OF_THREADS OPS_PER_THREAD\n");
		exit(-1);
	}
	hastTableSize = atoi(argv[1]);
	numThreads = atoi(argv[2]);
	ops = atoi(argv[3]);
	
	tb = create(hastTableSize);
	
	pthread_t threads[numThreads];	
	
	for (int i = 0; i < numThreads; i++) {
		pthread_create(&threads[i], NULL, work, (void *) (i + 1));
	}
	
	for (int i = 0; i < numThreads; i++) {
		pthread_join(threads[i], NULL);
	}
	
	destroy(tb);
	
	return 0;
}