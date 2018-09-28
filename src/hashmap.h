#ifndef HASHMAP_H
#define HASHMAP_H	1

struct token {
	char *key;
	char *value;
	struct token *next;
};

typedef struct token token_t;

struct hashmap {
    int size;
    token_t **entries;
};

typedef struct hashmap hashmap_t;

// Creates new hash on heap and returns address
hashmap_t *create_hash(int size);

// Hash a string for a map
int hash(hashmap_t *map, char *key);

// Create k/v pair
token_t *new_token(char *key, char *value);

// Insert in map
int put_token(hashmap_t *map, char *key, char *val);

// Get from map
char *get_token(hashmap_t *map, char *key);

// Extract from map
int extract_token(hashmap_t *map, char *key);

#endif
