CFLAGS= -pthread
LIBS= -lrt
.PHONY: hash

application: server client

server: Server.c HashTable.h HashTable.o
	gcc $(CFLAGS) -o server Server.c HashTable.o $(LIBS)
	
client: Client.c
	gcc $(CFLAGS) -o client Client.c $(LIBS)

HashTable.o: HashTable.c HashTable.h
	gcc $(CFLAGS) -c HashTable.c
	
clean:
	rm -f hash server client *.o