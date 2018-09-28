/*
 * hashmap.c - Practice 2
 *
 * 2018 drugescu <drugescu@drugescu-VirtualBox>
 *
 * Token hashmap, doesn't replace existing entries, only verifies they exist.
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "utils.h"

#include "hashmap.h"

#define _XOPEN_SOURCE 600

// Creates new hash on heap and returns address
hashmap_t *create_hash(int size) {

    hashmap_t *map = NULL;

    int i;

    if (size < 1) return NULL;

    // Malloc error
    map = malloc(sizeof(hashmap_t));
    if (map == NULL)
        return NULL;

    // Alloc head
    map->entries = malloc(sizeof(token_t *) * size);
    if (map->entries == NULL)
        return NULL;

    // Init entries
    for (i = 0; i < size; i++ ) {
        map->entries[i] = NULL;
    }

    map->size = size;

    return map;
}

// Hash a string for a map
int hash(hashmap_t *map, char *key) {

    size_t val, i;

#ifdef HASHMAP_USE_SIMPLE_HASH
    for (val = i = 0; i < strlen(key); val <<= 8, val += key[ i++ ]);
#else // Use Jenkins one at a time hash
    for (val = i = 0; i < strlen(key); ++i) {
        val += key[i];
        val += (val << 10);
        val += (val << 15);
    }
#endif

    return val % map->size;
}

// Create k/v pair
token_t *new_token(char *key, char *value) {
    token_t *new;

    new = malloc(sizeof(token_t));

    if (new == NULL)
        return NULL;

    // Insert key
    new->key = strdup(key);
    if (new->key == NULL)
        return NULL;

    // Insert value
    new->value = strdup(value);
    if (new->value == NULL)
        return NULL;

    new->next = NULL;

    return new;
}

// Insert in map
int put_token(hashmap_t *map, char *key, char *val) {
    int bin = 0;

    token_t *new = NULL;
    token_t *next = NULL;
    token_t *last = NULL;

    // Generate a hash for the key
    bin = hash(map, key);
    printf("Putting token (%s : %s) in hash : %d\n", key, val, bin);

    // Set to beginning of map
    next = map->entries[bin];

    // Search for the key
    while(next != NULL && next->key != NULL && strcmp(key, next->key) > 0) {
        last = next;
        next = next->next;
    }

    // If there's a pair return that it exists
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
        printf("               Already exists in hashmap!\n");
        return KEY_EXISTS;
    }
    else {
        // Otherwise create and insert new k/v pair
        new = new_token(key, val);

        // Treat start of list, add pair before all
        if (next == map->entries[bin]) {
            new->next = next;
            map->entries[bin] = new;
            printf("               Added at start of bin.\n");
        }
        // Treat end of list, add pair after last
        else if (next == NULL) {
            last->next = new;
            printf("               Added at end of bin.\n");
        }
        // Treat midpoint, repoint surrounding pairs
        else {
            new->next = next;
            last->next = new;
            printf("               Added midway.\n");
        }

        return KEY_AVAILABLE;
    }
    
}

// Get from map
char *get_token(hashmap_t *map, char *key) {
    int bin = 0;

    token_t *pair;

    // Set bin/hash
    bin = hash(map, key);

    // Search for value;
    pair = map->entries[bin];
    while(pair != NULL && pair->key != NULL && (strcmp(key, pair->key) > 0)) {
        pair = pair->next;
    }

    // Return pair
    if (pair == NULL || pair->key == NULL || strcmp(key, pair->key) != 0) {
        printf("Token not found in hashmap : %s\n", key);
        return NULL;
    }
    else {
        printf("Got token from hash : %s - %s\n", pair->key, pair->value);
        return pair->value;
    }
}

// Extract from map
int extract_token(hashmap_t *map, char *key) {
    int bin = 0;

    token_t *next = NULL;
    token_t *last = NULL;

    // Get the bin number/hash
    bin = hash(map, key);

    // Set to beginning of map
    next = map->entries[bin];

    // Search for the key
    // used to be next != null
    while((next->next != NULL) && (next->key != NULL) && (strcmp(key, next->key) > 0)) {
        last = next;
        next = next->next;
    }

    // If there's a pair return that it exists
    if((next != NULL) && (next->key != NULL) && (strcmp( key, next->key ) == 0)) {
        // Treat beginning of list
        if (next == map->entries[bin]) {

            last = map->entries[bin];

            map->entries[bin] = map->entries[bin]->next;

            last->next = NULL;
            free(last);
            last = NULL;

            printf("Deleted from hash at beginning of bin.\n");
        }
        // Treat end of list
        else if (next->next == NULL) {
            last->next = NULL;

            free(next);

            printf("Deleted from hash at end of bin.\n");
        }
        // Treat midpoint
        else {
            last->next = next->next;
            next->next = NULL;
            free(next);
            next = NULL;
            printf("Deleted from hash at midpoint of bin.\n");
        }

        return TOKEN_DELETED;
    }
    else {
        return TOKEN_DELETION_ERROR;
    }

}
