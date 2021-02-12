#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include "HashTable.h"

struct node{
	int key;
	void *value;
	pthread_rwlock_t lock;
	struct node *next;
};

struct HashTable_T {
	struct node **lists;
	int size;
};

struct node *createNode(struct node *next, int key, const void *value) {
	struct node *new = malloc(sizeof(struct node));

	new->key = key;
	new->value = (void *) value;
	new->next = next;
	pthread_rwlock_init(&(new->lock), NULL);
	
	return new;
}

struct node *createList() {
	struct node *head;
	struct node *tail;
	
	tail = createNode(NULL, INT_MAX, (int *) INT_MAX);
	head = createNode(tail, -1, (int *) -1);
	
	return head;
}

void *getFromList(struct node *head, int key) {
	struct node *curr, *pred;
	void *toReturn = NULL;
	
	pthread_rwlock_rdlock(&(head->lock));
	pred = head;
	curr = pred->next;
	
	pthread_rwlock_rdlock(&(curr->lock));
	
	while (curr->key < key) {
		pthread_rwlock_unlock(&(pred->lock));
		pred = curr;
		curr = curr->next;
		pthread_rwlock_rdlock(&(curr->lock));
	}
	
	if (curr->key == key)
		toReturn = curr->value;
		
	pthread_rwlock_unlock(&(pred->lock));
	pthread_rwlock_unlock(&(curr->lock));
		
	return toReturn;
}

void insertInList(struct node *head, int key, const void *value) {
	struct node *curr, *pred;
	
	pthread_rwlock_wrlock(&(head->lock));
	pred = head;
	curr = pred->next;
	
	pthread_rwlock_wrlock(&(curr->lock));
	
	while (curr->key < key) {
		pthread_rwlock_unlock(&(pred->lock));
		pred = curr;
		curr = curr->next;
		pthread_rwlock_wrlock(&(curr->lock));
	}
	if (curr->key == key)  {
		curr->value = (void *) value;
	}
	else {
		struct node *new = createNode(curr, key, value);		
		pred->next = new;
	}
	
	pthread_rwlock_unlock(&(pred->lock));
	pthread_rwlock_unlock(&(curr->lock));	
}

void *deleteFromList(struct node *head, int key) {
	struct node *curr, *pred;
	void *toReturn = NULL;
	
	pthread_rwlock_wrlock(&(head->lock));
	pred = head;
	curr = pred->next;
	
	pthread_rwlock_wrlock(&(curr->lock));
	
	while (curr->key < key) {
		pthread_rwlock_unlock(&(pred->lock));
		pred = curr;
		curr = curr->next;
		pthread_rwlock_wrlock(&(curr->lock));
	}
	if (curr->key == key) {
		toReturn = curr->value;		
		pred->next = curr->next;
		pthread_rwlock_unlock(&(curr->lock));
		pthread_rwlock_destroy(&(curr->lock));
		free(curr);
	}
	else
		pthread_rwlock_unlock(&(curr->lock));
	pthread_rwlock_unlock(&(pred->lock));
	
	return toReturn;
}

HashTable create(int size) {
	HashTable new = malloc(sizeof(struct HashTable_T));
	
	new->size = size;
	
	new->lists = malloc(size * sizeof(struct node *));
	
	for (int i = 0; i < size; i++) {
		new->lists[i] = createList();
	}
	
	return new;
}

void destroy(HashTable ht) {
	struct node *pred, *curr;
	
	for (int i = 0; i < ht->size; i++) {
		curr = ht->lists[i];		
		
		while (curr != NULL) {
			pred = curr;
			curr = curr->next;
			pthread_rwlock_destroy(&(pred->lock));
			free(pred);
		}
	}
	free(ht->lists);
	free(ht);
}

void *get(HashTable ht, int key) {
	int hash = key % ht->size;
	
	return getFromList(ht->lists[hash], key);
}

void insert(HashTable ht, int key, const void *value) {
	int hash = key % ht->size;
	
	insertInList(ht->lists[hash], key, value);
}

void *delete(HashTable ht, int key) {
	int hash = key % ht->size;
	
	deleteFromList(ht->lists[hash], key);
}