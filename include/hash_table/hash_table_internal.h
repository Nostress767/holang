#pragma once

#include <stdlib.h>

#include "types.h"
#ifdef DLL_HEADER_SOURCE
#undef DLL_HEADER_SOURCE
#include "vector/vector.h"
#define DLL_HEADER_SOURCE
#endif

typedef struct BucketEntry
{
	usize keySize;
	void *key;
	void *value;
} BucketEntry;

typedef struct Bucket
{
	Vector entries;
} Bucket;

typedef struct HashTable
{
	f32 maxLoadFactor;
	u64 hashKey; /* Key used in hash function */
	usize sz;         /* Size of element      */
	u32 n;            /* Amount of elements (key-value pairs)   */
	Vector buckets;         /* Vector of Buckets */
} HashTable;

static inline u32 rotl(u32 x, u32 b)
{
	return (u32)(((x) << (b)) | ((x) >> (32 - (b))));
}

static inline void u32_to_8_le(u8 p[static 4], u32 v)
{
	p[0] = (u8)((v));
    	p[1] = (u8)((v) >> 8);
    	p[2] = (u8)((v) >> 16);
    	p[3] = (u8)((v) >> 24);
}

static inline u32 u8_to_32_le(const u8 p[static 4])
{
	return (((u32)(p[0])) | ((u32)(p[1]) << 8) | ((u32)(p[2]) << 16) | ((u32)(p[3]) << 24));
}

static inline void sipround(u32 v[4])
{
        v[0] += v[1];
        v[1] = rotl(v[1], 5);
        v[1] ^= v[0];
        v[0] = rotl(v[0], 16);
        v[2] += v[3];
        v[3] = rotl(v[3], 8);
        v[3] ^= v[2];
        v[0] += v[3];
        v[3] = rotl(v[3], 7);
        v[3] ^= v[0];
        v[2] += v[1];
        v[1] = rotl(v[1], 13);
        v[1] ^= v[2];
        v[2] = rotl(v[2], 16);
}

static inline void hash_table_uninit_table(HashTable *ht)
{
	usize hashTableSize = vector_capacity(ht->buckets);
	for(usize i = 0; i < hashTableSize; ++i){
		Bucket bucket;
		[[maybe_unused]] VectorError e = vector_at(ht->buckets, i, &bucket);
		if(bucket.entries){
			u32 vectorSize = vector_size(bucket.entries);
			for(u32 j = 0; j < vectorSize; ++j){
				BucketEntry bucketEntry;
				e = vector_at(bucket.entries, j, &bucketEntry);
				free(bucketEntry.key);
				free(bucketEntry.value);
			}
			vector_uninit(bucket.entries);
		}
	}
	vector_uninit(ht->buckets);
}

#define HASH_TABLE_INTERNAL
#include "hash_table.h"

