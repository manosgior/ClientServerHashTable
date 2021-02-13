#ifndef _UTILS_H_
#define _UTILS_H_

#define OPS 50

const char *name = "buffer";

typedef struct commandBuffer {
	volatile int isReady;
	char command[8];
	int key;
	void *value;
} Command;

typedef struct Buffer {
	volatile int counter;
	pthread_mutex_t lock;
	Command commands[OPS];
} Buffer;

void errorExit(const char *msg) {	
	perror(msg);
	exit(-1);
}

#endif
