#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h> 
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "HashTable.h"
#include "Utils.h"

Buffer *buffer;	
HashTable tb;

void *work(void *arg) {
	void *value;
	int counter = (int ) arg;

	if (strcmp(buffer->commands[counter].command, "insert") == 0) {
		insert(tb, buffer->commands[counter].key, buffer->commands[counter].value);
		printf("Inserted (%d, %d)\n", buffer->commands[counter].key, (int) buffer->commands[counter].value);
	}
	else if (strcmp(buffer->commands[counter].command, "delete") == 0) {
		value = delete(tb, buffer->commands[counter].key);
		if (value != NULL)
			printf("Deleted (%d, %d)\n", buffer->commands[counter].key, (int) value);
		else
			printf("Key %d doesn't exist\n", buffer->commands[counter].key);
	}
	else if (strcmp(buffer->commands[counter].command, "get") == 0) {
		value = get(tb, buffer->commands[counter].key);			
		if (value != NULL)
			printf("Got (%d, %d)\n", buffer->commands[counter].key, (int) value);
		else
			printf("Key %d doesn't exist\n", buffer->commands[counter].key);
	}

	__atomic_store_n(&(buffer->commands[counter].isReady), 0, __ATOMIC_SEQ_CST);
}

int main(int argc, char **argv) {		
	int fd;		
	int counter = 0;
	pthread_t thread;
	struct timespec start, end;

	tb = create(atoi(argv[1]));
	fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(fd, sizeof(struct Buffer)); 
	
	if (fd < 0)
		errorExit("Error in shm_open: ");
		
	buffer = mmap(NULL,  sizeof(struct Buffer), PROT_WRITE, MAP_SHARED, fd, 0);	

	if (buffer == (void *) -1)
		errorExit("Error in mmap: ");

	memset(&(buffer->commands), 0, OPS * sizeof(struct commandBuffer));
	buffer->counter = -1;	
	pthread_mutex_init(&(buffer->lock), NULL);	
		
	while (1) {	
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	
		while (__atomic_load_n(&(buffer->commands[counter].isReady), __ATOMIC_SEQ_CST) == 0) {
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
			if (end.tv_sec - start.tv_sec >= 10) {
				pthread_mutex_destroy(&(buffer->lock));
				munmap(buffer, sizeof(struct commandBuffer));
				close(fd);
				shm_unlink(name);
				destroy(tb);
				fprintf(stderr, "Server timeout, exiting...\n");
				return 0;
			}
		}		
		
		pthread_create(&thread, NULL, work, (void *) counter);		

		if (counter == OPS - 1)
			counter = 0;
		else
			counter++;					
	}
	
}




