#include "HashTable.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h> 
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

typedef struct commandBuffer {
	char command[8];
	int key;
	void *value;
} Command;

void errorExit(const char *msg) {	
	perror(msg);
	exit(-1);
}

int main(int argc, char **argv) {	
	const char *name = "buffer";
	HashTable tb;
	Command *c;
	int fd;	
	
	tb = create(atoi(argv[1]));
	fd = shm_open(name, O_CREAT | O_RDWR, 0666); 
	
	if (fd < 0)
		errorExit("Error in shm_open: ");
		
	c = mmap(NULL, sizeof(struct commandBuffer), PROT_READ, MAP_SHARED, fd, 0);

	if (c == (void *) -1)
		errorExit("Error in mmap: ");
		
	while (1) {
		if (strcmp(c->command, "insert") == 0) {
			insert(tb, c->key, c->value);
			printf("Inserted key %d with value %d\n", c->key, (int) c->value);
		}
		else if (strcmp(c->command, "delete") == 0) {
			delete(tb, c->key);
			printf("Deleted key %d\n", c->key);
		}
		else if (strcmp(c->command, "get") == 0) {			
			printf("Got key's %d value %d\n", c->key, (int) get(tb, c->key));
		}
		else {
			munmap(c, sizeof(struct commandBuffer));
			close(fd);
			shm_unlink(name);
			destroy(tb);
			break;
		}			
	}
	
	return 0;
}




