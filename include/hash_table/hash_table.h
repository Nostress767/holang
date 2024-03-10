#pragma once

#include "types.h"

constexpr u64 hashTableDefaultHashKey = 0x0123456789ABCDEF;
constexpr f32 hashTableDefaultMaxLoadFactor = 1.f;
constexpr u32 hashTableInitialSize = 1 << 7;
constexpr u32 hashTableBucketEntryInitialSize = 1 << 5;

typedef struct Bucket
{
	u8 *entries;
	usize entriesSize; /* Internal size of bucket entries array */
	u32 entriesAmount; /* Amount of bucket entries in array	    */
} Bucket;

enum HashTableError : u8
{
	hashTableErrorOutOfMemory,
	hashTableErrorSuccess,
	hashTableErrorNullKeyInput,
	hashTableErrorNullValueInput,
	hashTableErrorKeyNotFound,
	hashTableErrorKeyAlreadyExists,
};

typedef struct HashTable
{
	usize keySz;	   		   /* Size of keys 				*/
	usize valueSz;	   		   /* Size of elements 				*/
	Bucket *buckets;   		   /* Array of Buckets 				*/
	usize bucketsSize; 		   /* Internal size of bucket array 		*/
	u32 n;		   		   /* Amount of elements 			*/
	f32 maxLoadFactor; 		   /* Affects when to resize hash table 	*/

	u32 (*customHash)(const void *key);
	int (*keyComp)(const void *a, const void *b);

	/* Function used to manage element memory in the buckets */
	/* The value is placed right after the key inside bucketData */
	const void* (*customAllocator)(const void *key, const void *value, u8 *bucketData);

	enum HashTableError lastError; 	   /* Last occured error 			*/
} HashTable;

#undef DLL_FUNCTIONS
#define DLL_FUNCTIONS \
DLL_X(hash_table_half_siphash, u32, const usize inLen, u8 in[restrict const static inLen], u8 k[restrict const static 8]) \
DLL_X(hash_table_init, HashTable* , const usize keySz, const usize valueSz) \
DLL_X(hash_table_init_with_hash, HashTable* , const usize keySz, const usize valueSz, u32 (*customHash)(const void *key)) \
DLL_X(hash_table_init_with_hash_and_keycomp, HashTable* , const usize keySz, const usize valueSz, u32 (*customHash)(const void *key), int (*keyComp)(const void *a, const void *b)) \
DLL_X(hash_table_init_with_hash_keycomp_and_allocator, HashTable* , const usize keySz, const usize valueSz, u32 (*customHash)(const void *key), int (*keyComp)(const void *a, const void *b), const void* (*customAllocator)(const void *key, const void *value, u8 *bucketData)) \
DLL_X(hash_table_uninit, void, HashTable *ht) \
DLL_X(hash_table_at, void*, HashTable ht[restrict const static 1], const void *key) \
DLL_X(hash_table_reserve, void, HashTable ht[static 1], usize reserveSize) \
DLL_X(hash_table_insert, void, HashTable ht[restrict static 1], const void *key, const void *value) \
DLL_X(hash_table_erase, void, HashTable ht[static 1], const void *key) \
DLL_X(hash_table_clear, void, HashTable ht[static 1])

static inline enum HashTableError hash_table_get_last_error(HashTable ht[const static 1])
{
	return ht->lastError;
}

static inline bool hash_table_empty(HashTable ht[const static 1])
{
	return ht->n == 0;
}

static inline u32 hash_table_size(HashTable ht[const static 1])
{
	return ht->n;
}

static inline usize hash_table_key_sz(HashTable ht[const static 1])
{
	return ht->keySz;
}

static inline usize hash_table_value_sz(HashTable ht[const static 1])
{
	return ht->valueSz;
}

static inline usize hash_table_capacity(HashTable ht[const static 1])
{
	return ht->bucketsSize;
}

static inline f32 hash_table_load_factor(HashTable ht[const static 1])
{
	return ((f32)ht->n) / ht->bucketsSize;
}

static inline void hash_table_set_max_load_factor(HashTable ht[static 1], f32 maxLoadFactor)
{
	ht->maxLoadFactor = maxLoadFactor;
}

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(hash_table)
#include "dll/plugger.h"

