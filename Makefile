CFLAGS= -pthread
.PHONY: hash

clean:
	rm -f hash *.o

hash: TestMain.c HashTable.h HashTable.o
	gcc $(CFLAGS) -o hash TestMain.c HashTable.o

HashTable.o: HashTable.c HashTable.h
	gcc $(CFLAGS) -c HashTable.c