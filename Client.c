#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
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
	Command *dest, source;
	char *commands[3] = {"insert", "delete", "get"};
	int fd;		
	
	fd = shm_open(name, O_RDWR, 0666);
	ftruncate(fd, sizeof(struct commandBuffer)); 
	
	if (fd < 0)
		errorExit("Error in shm_open:");
		
	dest = mmap(NULL, sizeof(struct commandBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (dest == (void *) -1)
		errorExit("Error in mmap:");
	
	for (int i = 0; i < 20; i++) {
		strcpy(source.command, "insert");
		source.key = i;
		source.value = (void *) i;
		memcpy(dest, &source, sizeof (struct commandBuffer));
	}
	for (int i = 0; i < 20; i++) {
		strcpy(source.command, "get");
		source.key = i;		
		memcpy(dest, &source, sizeof (struct commandBuffer));
	}
	for (int i = 0; i < 20; i++) {
		strcpy(source.command, "delete");
		source.key = i;		
		memcpy(dest, &source, sizeof (struct commandBuffer));
	}
	for (int i = 0; i < 20; i++) {
		strcpy(source.command, "get");
		source.key = i;		
		memcpy(dest, &source, sizeof (struct commandBuffer));
	}
	
	strcpy(source.command, "kill");		
	memcpy(dest, &source, sizeof (struct commandBuffer));
	
	munmap(dest, sizeof(struct commandBuffer));
	close(fd);
	shm_unlink(name);
	
	return 0;
}




