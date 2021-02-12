typedef struct HashTable_T* HashTable;

HashTable create(int size);
void destroy(HashTable ht);
void *get(HashTable ht, int key);
void insert(HashTable ht, int key, const void *value);
void *delete(HashTable ht, int key);