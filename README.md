Simple HashTable implementation with (key : int) -> (value : void *).
The hash "function" is a simple modulo (key % hashTableSize).

Client-Server logic:

The server allocates a shared memory (cyclic) buffer of size "OPS" (defined in Utils.h) + sizeof(int) + sizeof(pthread_mutex_t).
The integer is a simple counter protected with the lock, which marks the last available slot in the buffer/queue (needed by the clients).
If the buffer was large enough (e.g. infinite) a simple atomic increment would suffice (e.g fetch_and_add).
Each of "OPS" array's element is a struct of type:

struct commandBuffer {
	volatile int isReady; // set to 1 by a client after the completion of memcpy 
	char command[8];      // which command to execute {insert, delete, get}
	int key;              // the key to {insert, delete, get}
	void *value;          // the value to insert
};
 
Server's algorithm:
	1. Get the time
	2. Atomically load the current slot's isReady value.
		2.1 If it is 0 (unready)
			2.1.1 Get the time
			2.1.2 If more than 30 seconds elapsed
				2.2.2.1 destroy everything and return (mutexes, file pointers, munmap, hashtable) //timeout
			2.1.3 else go to (2)
		2.2 else go to (3)
	3. Create the thread which will execute the client's command // it will "consume" the slot and set its isReady value to 0
	4. Increment or reset your local counter //which traverses the buffer in a round-robin fashion
	5. Go to (1)

Client's algorithm:
	1. for i in {1..100} // could be configurable, doesn't really matter
		2. Get the first free slot (lock the shared counter, increment or reset it, unlock it and return its value)
		3. While the current slot's isRead value is 1 (ready) do noop // The server hasn't consumed the slot yet
		4. set the command's string 
		5. set the key (the loop's index for simplicity)
		6. set the value (the process' pid for simplicity)
		7. atomically change the current slot's isReady field to 1
		8. sleep 1 (convenient with multiple clients, gives time for a person to actually see the server's output)

Obvsiously the above design choices are arbitrary and could be easily changed/configured 
e.g. the buffer's size, the client's # of operations, the server's timeout, the hashtable's hash function
