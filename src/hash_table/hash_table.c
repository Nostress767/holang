#define DLL_HEADER_SOURCE
#include "hash_table/hash_table_internal.h"
DEBUG_DEFINE_VTABLE(hash_table)
#undef DLL_HEADER_SOURCE

#include <stdlib.h>
#include <assert.h>

/* default: SipHash-2-4 */
constexpr u32 cRounds = 2;
constexpr u32 dRounds = 4;

/*
	Computes a half SipHash value (fixed with u32 return)
	inLen: input data length in bytes (any usize value)
    	in: pointer to input data (read-only)
    	k: pointer to the key data (read-only), must be 8 bytes 
	return: u32 hash
*/
u32 hash_table_half_siphash(const usize inLen, u8 in[restrict const static inLen], u8 k[restrict const static 8])
{
	/* assert((outLen == 4) || (outLen == 8)); GCOVR_EXCL_BR_LINE */
	u32 v[4] = { 0, 0, 0x6c796765, 0x74656462 };
	u32 k0 = u8_to_32_le(k);
	u32 k1 = u8_to_32_le(k + 4);
	u32 m;
	i32 i;
	const u8 *end = in + inLen - (inLen % sizeof(u32));
	const i32 left = inLen & 3;
	u32 b = ((u32)inLen) << 24;
	v[3] ^= k1;
	v[2] ^= k0;
	v[1] ^= k1;
	v[0] ^= k0;
	u8 *inMut = in;

	/*
	if (outLen == 8)
		v[1] ^= 0xee;
	*/

	for (; inMut != end; inMut += 4) {
		m = u8_to_32_le(inMut);
		v[3] ^= m;

		for (i = 0; i < cRounds; ++i)
			sipround(v);

		v[0] ^= m;
	}

	switch (left) { /* GCOVR_EXCL_BR_LINE */
		case 3:
			b |= ((u32)inMut[2]) << 16;
			[[fallthrough]];
		case 2:
			b |= ((u32)inMut[1]) << 8;
			[[fallthrough]];
		case 1:
			b |= ((u32)inMut[0]);
			break;
		case 0:
			break;
	}

	v[3] ^= b;

	for (i = 0; i < cRounds; ++i)
		sipround(v);

	v[0] ^= b;

	/*
	if (outLen == 8)
		v[2] ^= 0xee;
	else
	*/
		v[2] ^= 0xff;

	for (i = 0; i < dRounds; ++i)
		sipround(v);

	b = v[1] ^ v[3];

	return b;
	/*
	u32_to_8_le(out, b);

	if (outLen == 4)
		return;

	v[1] ^= 0xdd;

	for (i = 0; i < dRounds; ++i)
		sipround(v);

	b = v[1] ^ v[3];
	u32_to_8_le(out + 4, b);
	*/
}

HashTable* hash_table_init(const usize keySz, const usize valueSz)
{
	return hash_table_init_with_hash(keySz, valueSz, nullptr);
}

HashTable* hash_table_init_with_hash(const usize keySz, const usize valueSz, u32 (*customHash)(const void *key))
{
	return hash_table_init_with_hash_and_keycomp(keySz, valueSz, customHash, nullptr);
}

HashTable* hash_table_init_with_hash_and_keycomp(const usize keySz, const usize valueSz, u32 (*customHash)(const void *key), int (*keyComp)(const void *a, const void *b))
{
	return hash_table_init_with_hash_keycomp_and_allocator(keySz, valueSz, customHash, keyComp, nullptr);
}

HashTable* hash_table_init_with_hash_keycomp_and_allocator(const usize keySz, const usize valueSz, u32 (*customHash)(const void *key), int (*keyComp)(const void *a, const void *b), const void* (*customAllocator)(const void *key, const void *value, const usize entrySz, u8 *bucketData))
{
	HashTable *ht = malloc(sizeof *ht);
	if(!ht){
		return nullptr;
	}

	ht->buckets = calloc(hashTableInitialSize, sizeof(*ht->buckets));
	if(!ht->buckets){
		free(ht);
		return nullptr;
	}

	ht->keySz = keySz;
	ht->valueSz = valueSz;
	ht->bucketsSize = hashTableInitialSize;
	ht->n = 0;
	ht->maxLoadFactor = hashTableDefaultMaxLoadFactor;

	ht->customHash = customHash;
	ht->keyComp = keyComp;
	ht->customAllocator = customAllocator;

	ht->lastError = hashTableErrorSuccess;

	return ht;
}

void hash_table_uninit(HashTable *ht)
{
	for(usize i = 0; i < ht->bucketsSize; ++i){
		if(ht->customAllocator){
			for(u32 j = 0; j < ht->buckets[i].entriesAmount; ++j){ /* If a bucket exists, at least one entry does as well */
				const usize entrySz = hash_table_key_sz(ht) + hash_table_value_sz(ht);
				u8 *keyValuePos = &ht->buckets[i].entries[j * entrySz];
				ht->customAllocator(nullptr, nullptr, entrySz, keyValuePos);
			}
		}
		free(ht->buckets[i].entries);
	}
	free(ht->buckets);
	free(ht);
}

void* hash_table_at(HashTable ht[restrict const static 1], const void *key)
{
	return _search_bucket_entry(ht, key, nullptr);
}

void hash_table_reserve(HashTable ht[static 1], usize reserveSize)
{
	constexpr usize biggestCeil = SIZE_MAX - (SIZE_MAX >> 1);
	if(biggestCeil < reserveSize){
		ht->lastError = hashTableErrorOutOfMemory;
		return;
	}

	if(reserveSize <= ht->bucketsSize){
		ht->lastError = hashTableErrorSuccess;
		return;
	}

	reserveSize = _usize_bit_ceil(reserveSize); /* smallest power of two bigger than reserveSize */

	Bucket *newBuckets = calloc(reserveSize, sizeof(*ht->buckets));
	if(!newBuckets){
		ht->lastError = hashTableErrorOutOfMemory;
		return;
	}

	for(u32 i = 0; i < ht->bucketsSize; ++i){
		for(u32 j = 0; j < ht->buckets[i].entriesAmount; ++j){ /* Rehash every entry from every *existing* bucket */
			u8 *keyValue = &ht->buckets[i].entries[j * (hash_table_key_sz(ht) + hash_table_value_sz(ht))];

			u32 hash = ht->customHash ? ht->customHash(keyValue) : hash_table_half_siphash(ht->keySz, keyValue, (u8*)&hashTableDefaultHashKey);
			u32 pos = hash & (reserveSize - 1); /* This is the important part. The entry is kept as-is */

			const usize entrySz = hash_table_key_sz(ht) + hash_table_value_sz(ht);

			if(!_alloc_bucket_entries_if_not_exists(&newBuckets[pos], entrySz))
				goto no_more_memory;

			if(!_realloc_bucket_entries_if_full(&newBuckets[pos], entrySz))
				goto no_more_memory;

			memcpy(&newBuckets[pos].entries[newBuckets[pos].entriesAmount * entrySz], keyValue, entrySz);
			++newBuckets[pos].entriesAmount;
			continue;

			no_more_memory:
			for(u32 k = 0; k < reserveSize; ++k)
				if(newBuckets[k].entries)
					free(newBuckets[k].entries);
			free(newBuckets);
			ht->lastError = hashTableErrorOutOfMemory;
			return;
		}
	}

	for(u32 i = 0; i < ht->bucketsSize; ++i)
		free(ht->buckets[i].entries);
	free(ht->buckets);
	ht->buckets = newBuckets;
	ht->bucketsSize = reserveSize;

	ht->lastError = hashTableErrorSuccess;
}

void hash_table_insert(HashTable ht[restrict static 1], const void *key, const void *value)
{
	if(!key){
		ht->lastError = hashTableErrorNullKeyInput;
		return;
	}
	if(!value){
		ht->lastError = hashTableErrorNullValueInput;
		return;
	}

	if(hash_table_load_factor(ht) > ht->maxLoadFactor){
		hash_table_reserve(ht, 2 * ht->bucketsSize);
		if(hashTableErrorOutOfMemory == hash_table_get_last_error(ht))
			return;
	}

	Bucket *foundBucket = nullptr;
	if(_search_bucket_entry(ht, key, &foundBucket)){
		ht->lastError = hashTableErrorKeyAlreadyExists;
		return;
	}

	const usize entrySz = hash_table_key_sz(ht) + hash_table_value_sz(ht);

	if(!_alloc_bucket_entries_if_not_exists(foundBucket, entrySz)){
		ht->lastError = hashTableErrorOutOfMemory;
		return;
	}

	if(!_realloc_bucket_entries_if_full(foundBucket, entrySz)){
		ht->lastError = hashTableErrorOutOfMemory;
		return;
	}

	if(ht->customAllocator){
		const void *keyValue = ht->customAllocator(key, value, entrySz, &foundBucket->entries[foundBucket->entriesAmount * entrySz]);
		if(!keyValue){
			ht->lastError = hashTableErrorOutOfMemory;
			return;
		}
	}
	else{
		memcpy(&foundBucket->entries[foundBucket->entriesAmount * entrySz], key, hash_table_key_sz(ht));
		memcpy(&foundBucket->entries[foundBucket->entriesAmount * entrySz + hash_table_key_sz(ht)], value, hash_table_value_sz(ht));
	}

	++foundBucket->entriesAmount;
	++ht->n;
	ht->lastError = hashTableErrorSuccess;
}

void hash_table_erase(HashTable ht[static 1], const void *key)
{
	if(!key){
		ht->lastError = hashTableErrorNullKeyInput;
		return;
	}

	Bucket *foundBucket = nullptr;
	typeof(foundBucket->entries) value = _search_bucket_entry(ht, key, &foundBucket);
	if(value){
		--foundBucket->entriesAmount;
		--ht->n;

		if(ht->customAllocator){
			const usize entrySz = hash_table_key_sz(ht) + hash_table_value_sz(ht);
			u8 *keyValuePos = &foundBucket->entries[foundBucket->entriesAmount * entrySz];
			ht->customAllocator(nullptr, nullptr, entrySz, keyValuePos);
		}

		const usize entrySz = hash_table_key_sz(ht) + hash_table_value_sz(ht);
		const usize entryPlace = (abs(value - foundBucket->entries) - hash_table_key_sz(ht)) / entrySz; /* hack to reuse function */
		if(entryPlace < foundBucket->entriesAmount) /* if not the last element */
			memcpy(&foundBucket->entries[entryPlace * entrySz], &foundBucket->entries[foundBucket->entriesAmount * entrySz], entrySz); /* swap with the last */

		if(foundBucket->entriesAmount == 0){
			free(foundBucket->entries);
			foundBucket->entries = nullptr;
		}

		ht->lastError = hashTableErrorSuccess;
		return;
	}

	/* TODO: should resize down? */

	ht->lastError = hashTableErrorKeyNotFound;
}

void hash_table_clear(HashTable ht[static 1])
{
	/* NOTE: maybe resize down? */
	
	Bucket emptyBucket = {};
	for(usize i = 0; i < ht->bucketsSize; ++i){
		if(ht->customAllocator){
			for(u32 j = 0; j < ht->buckets[i].entriesAmount; ++j){ /* If a bucket exists, at least one entry does as well */
				const usize entrySz = hash_table_key_sz(ht) + hash_table_value_sz(ht);
				u8 *keyValuePos = &ht->buckets[i].entries[j * entrySz];
				ht->customAllocator(nullptr, nullptr, entrySz, keyValuePos);
			}
		}
		free(ht->buckets[i].entries);
		ht->buckets[i] = emptyBucket;
	}

	ht->n = 0;
}

