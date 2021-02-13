#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "Utils.h"

int safeCounterIncrement(Buffer *buffer) {
	int tmp = 0;

	pthread_mutex_lock(&(buffer->lock));
	if (buffer->counter == OPS - 1)
		buffer->counter = 0;
	else
		buffer->counter++;
	
	tmp = buffer->counter;
	pthread_mutex_unlock(&(buffer->lock));

	return tmp;
}

int main(int argc, char **argv) {		
	Buffer *buffer;
	Command source, *dest;
	char *commands[3] = {"insert", "get", "delete"};
	int fd, index;		
	
	fd = shm_open(name, O_RDWR, 0666);	 
	
	if (fd < 0)
		errorExit("Error in shm_open:");
		
	buffer = mmap(NULL, sizeof(struct Buffer), PROT_WRITE, MAP_SHARED, fd, 0);

	if (buffer == (void *) -1)
		errorExit("Error in mmap:");
	
	srand(time(NULL));
	source.isReady = 0;

	for (int i = 0; i < 500; i++) {
		index = safeCounterIncrement(buffer);
		dest = &(buffer->commands[index]);
		
		while (__atomic_load_n(&(buffer->commands[index].isReady), __ATOMIC_SEQ_CST) == 1) {}		
		
		strcpy(source.command, commands[rand() % 3]);
		source.key = i;
		source.value = (void *) getpid();
		memcpy(dest, &source, sizeof (struct commandBuffer));
		__atomic_store_n(&(dest->isReady), 1, __ATOMIC_SEQ_CST);
		sleep(0.5);
	}	

	return 0;
}




