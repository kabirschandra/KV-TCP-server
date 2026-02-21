#ifndef HASHMAP_H
#define HASHMAP_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node Node;
typedef struct Hashmap {

    Node **buckets;
    size_t numBuckets;
    const size_t (*hashFunction)(const char *key, size_t keySize); 
    //Fptr does have overhead, but this is negligible next to the LL traversal cost 

    //Usually good practise to include a destructor fptr for elements
    //This map is for a database and only intended to store strings however, so we can just free()
} Hashmap;


/** 
 * @brief Implementation of the djb2 hash function 
 * 
 * @param key :: Key to hash
 * @param keySize :: Size of key to hash (bytes)
 * 
 * @return const size_t :: Hash of key (djb2)
 */
const size_t hashmap_djb2(const char *key, size_t keySize);


/** 
 * @brief Initialise a hashmap with a size and hash function (does not free memory)
 * 
 * @param map :: Pointer to map to be initialised
 * @param size :: Initial size of the map (number of buckets)
 * @param hashFunction :: The hash function pointer to be used with the map. 
 *                The function should take a char array and its size (including \0), and return a hash as size_t
 * 
 * @return bool :: Returns true if map was successfully allocated and false if the alloaction failed
 */
bool hashmap_init(Hashmap *map, const size_t size, const size_t (*hashFunction)(const char *key, size_t keySize));



/** 
 * @brief Destroy a hashmap and associated memory 
 * 
 * @param map :: Pointer to map to be destroyed 
 * 
 * @return void :: None
 */
void hashmap_destroy(Hashmap *map);



/** 
 * @brief Insert a key and value pair into a hashmap 
 * 
 * @param map :: Map pointer to be inserted into 
 * @param key :: Pointer to key data
 * @param keySize :: Size of the key inserted (bytes)
 * @param value :: Pointer to the value data
 * @param valueSize :: Size of the value inserted (bytes)
 * 
 * @return const char* :: Pointer to new item in the map, returns NULL if insertion failed (map is unaltered)
 */
const char *hashmap_insert(Hashmap *map, const char *key, const size_t keySize, const char *value, const size_t valueSize);



/** 
 * @brief Find an item in a hashmap 
 * 
 * @param map :: Pointer to map to be searched
 * @param key :: Key to be searched for
 * @param keySize :: Size of the key to be searched for (bytes)
 * 
 * @return const char* :: Pointer to item if the map (if present), otherwise NULL is returned
 */
const char *hashmap_find(const Hashmap *map, const char *key, const size_t keySize);


/** 
 * @brief Edit an item present in the map 
 * 
 * @param map :: Map pointer to be inserted into 
 * @param key :: Pointer to key data to use to search
 * @param keySize :: Size of the key (bytes)
 * @param newValue :: Pointer to the new value data
 * @param newValueSize :: Size of the new value inserted (bytes)
 * 
 * @return const char* :: Pointer to new item in the map, returns NULL if edit failed, or the item isnt present (map is unaltered)
 */
const char *hashmap_edit(Hashmap *map, const char *key, const size_t keySize, const char *newValue, const size_t newValueSize);


/** 
 * @brief Delete an item from a hashmap
 * 
 * @param map :: Map pointer to delete the item from
 * @param key :: Key to delete from the map
 * @param keySize :: Size of key to be deleted
 *
 * @return bool :: Returns true if item was found and deleted, false if item is not present
 */
bool hashmap_delete(Hashmap *map, const char *key, const size_t keySize);



/** 
 * @brief Rehash a hashmap with a new size and hash function
 *        NOTE: This function involves reallocating a second map, and then copying into it - so is very memory intense
 * 
 * @param map :: Pointer to the map to be rehashed
 * @param newSize :: New number of buckets for the hashmap
 * @param hashFunction :: New (optional) hash function pointer. If the pointer is NULL, the hash function is not changed
 * 
 * @return bool :: Returns true if rehash was successful, and false if it was unsucessful (map is unaltered)
 */
bool hashmap_rehash(Hashmap *map, const size_t newSize, const size_t (*hashFunction)(const char *key, size_t keySize));



/** 
 * @brief Display a hashmap and its contents (buckets and items)
 * 
 * @param map :: Map pointer to display from
 *
 * @return void :: None 
 */
void hashmap_display(const Hashmap *map);

#endif
