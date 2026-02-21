#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "../server/hashmap.h"
//NOTE: I know this leaks memory, it relies on the OS to clean up (bad practise but this is just for testing purposese)
//I could probably break up tests into functions at some point for cleaner code
//I could also probably use asserts if I really wanted to make it look nice

int latch = 0;
void generate_random_str(char *buffer, uint8_t bufferSize) {

    size_t size = bufferSize;

    for(size_t i = 0; i < size - 1; i++) {
        char randomChar = 'A' + (rand() % 26);
        buffer[i] = randomChar;
    }

    buffer[size - 1] = '\0';
    return;
}


int main(void) {
    srand(time(NULL));
    Hashmap map;

restart:
    if(!hashmap_init(&map, 100, hashmap_djb2)) {
        printf("Hashmap allocation failed - hashmap_init\n");
        return -1;
    }

    
    size_t itemsToAdd = 1000;
    size_t itemsToDelete = 100; //Technically a max due to rerolling
    uint8_t itemSize = 100;
    char **keys = malloc(itemsToAdd * sizeof(char*));
    char **vals = malloc(itemsToAdd * sizeof(char*));


    if(!keys || !vals) {
        printf("K/V alloc failure - internal\n");
        return -2;
    }

    for(size_t i = 0; i < itemsToAdd; i++) {
        uint8_t keySize = rand() % itemSize + 2;
        uint8_t valSize = rand() % itemSize + 2;
        
        keys[i] = calloc(keySize, sizeof(char));
        keys[i][0] = keySize;
        vals[i] = calloc(valSize, sizeof(char));
        vals[i][0] = valSize;
        if(!keys[i] || !vals[i]) {
            printf("K/V str alloc failure - internal\n");
            return -3;
        }
    }


    //TEST 1 - INSERTION
    for(size_t i = 0; i < itemsToAdd; i++) {
        generate_random_str(keys[i], keys[i][0]);
        generate_random_str(vals[i], vals[i][0]);

        if(!hashmap_insert(&map, keys[i], strlen(keys[i]), vals[i], strlen(vals[i]))) {
            printf("Hashmap insertion failed alloc - hashmap_insert\n");
            return -4;
        }
    }

    //TEST 2 - DELETE RANDOM VALUES
    for(size_t i = 0; i < itemsToDelete; i++) {

        size_t index = rand() % itemsToAdd;
        if(!keys[index] || !vals[index]) {
            continue;
        }


        if(!hashmap_delete(&map, keys[index], strlen(keys[index]))) {
            printf("Hashmap deletion failed - hashmap_delete, '%s', '%s'\n",keys[index], vals[index]);
            return -5;
        }

        //TEST 3 - FIND DELETED ITEM
        if(hashmap_find(&map, keys[index], strlen(keys[index]))) { //Shouldnt be able to find a deleted item
            printf("Hashmap deletion or find failed - hashmap_delete/find, '%s', '%s'\n", keys[index], vals[index]);
            return -6;
        }

        //Yes this leaks memory
        keys[index] = NULL;
        vals[index] = NULL;
    }


    //TEST 4 - FIND ITEMS
    for(size_t i = 0; i < itemsToAdd; i++) {
        if(keys[i] == NULL) {
            continue;
        }

        const char *value = hashmap_find(&map, keys[i], strlen(keys[i]));
        if(!value || strcmp(value, vals[i])) {
            printf("Hashmap find failed - hashmap_find\n");
            return -7;
        }
    }

    latch++;


    hashmap_rehash(&map, 10, NULL);

    if(latch != 1) {
        goto restart; //Test again rehashing
    }

    hashmap_display(&map);
    hashmap_destroy(&map);
    return 0;
}




