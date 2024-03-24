#pragma once

#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

INTERNAL u32 rotl(u32 x, u32 b)
{
	return (u32)(((x) << (b)) | ((x) >> (32 - (b))));
}

INTERNAL void u32_to_8_le(u8 p[static 4], u32 v)
{
	p[0] = (u8)((v));
    	p[1] = (u8)((v) >> 8);
    	p[2] = (u8)((v) >> 16);
    	p[3] = (u8)((v) >> 24);
}

INTERNAL u32 u8_to_32_le(const u8 p[static 4])
{
	return (((u32)(p[0])) | ((u32)(p[1]) << 8) | ((u32)(p[2]) << 16) | ((u32)(p[3]) << 24));
}

INTERNAL void sipround(u32 v[4])
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

INTERNAL usize _usize_bit_ceil(const usize value)
{
	usize i = 1;
	while(i < value)
		i <<= 1;
	return i;
}

INTERNAL void* _search_bucket_entry(HashTable ht[restrict const static 1], const void *key, Bucket **outBucket)
{
	u32 hash = ht->customHash ? ht->customHash(key) : hash_table_half_siphash(ht->keySz, (u8 *restrict const)key, (u8*)&hashTableDefaultHashKey);

	Bucket *foundBucket = &ht->buckets[hash & (ht->bucketsSize - 1)]; /* bucketsSize needs to be a power of 2 for this to be valid */

	if(outBucket)
		*outBucket = foundBucket;

	for(u32 i = 0; i < foundBucket->entriesAmount; ++i){
		u8 *keyValue = &foundBucket->entries[i * (hash_table_key_sz(ht) + hash_table_value_sz(ht))];

		if((ht->keyComp && ht->keyComp(keyValue, key) == 0) || (memcmp(keyValue, key, hash_table_key_sz(ht)) == 0))
			return keyValue + hash_table_key_sz(ht);
	}

	return nullptr;
}

INTERNAL void* _alloc_bucket_entries_if_not_exists(Bucket bkt[static 1], const usize entrySz)
{
	if(!bkt->entries){
		bkt->entriesSize = hashTableBucketEntryInitialSize;
		bkt->entries = malloc(entrySz * bkt->entriesSize);
	}
	return bkt->entries;
}

INTERNAL void* _realloc_bucket_entries_if_full(Bucket bkt[static 1], const usize entrySz)
{
	if(bkt->entriesAmount == bkt->entriesSize){
		void *tmp = realloc(bkt->entries, entrySz * bkt->entriesSize * 2);
		if(!tmp)
			return nullptr;
		bkt->entriesSize *= 2;
		bkt->entries = tmp;
	}
	return bkt->entries;
}

