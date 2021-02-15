#!/bin/sh

clients=0;
hashTableSize=0;

make

echo "Please give the hashtable's size:"
read hashTableSize
echo "Please give the number of clients:"
read clients

./server $hashTableSize &

for i in $( seq 1 $clients )
do
	./client &
done

make clean
