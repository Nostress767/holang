#define DLL_HEADER_SOURCE
#include "hash_table/hash_table_internal.h"
DEBUG_DEFINE_VTABLE(hash_table)
#undef DLL_HEADER_SOURCE

#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* default: SipHash-2-4 */
constexpr u32 cRounds = 2;
constexpr u32 dRounds = 4;

/*
    Computes a half SipHash value
    inLen: input data length in bytes (any usize value)
    in: pointer to input data (read-only)
    k: pointer to the key data (read-only), must be 8 bytes 
    outLen: length of the output in bytes, must be 4 or 8
    out: pointer to output data (write-only), outlen bytes must be allocated
*/
void hash_table_half_siphash(const usize inLen, const u8 in[restrict static inLen], const u8 k[restrict static 8], const usize outLen, u8 out[restrict static outLen])
{
	assert((outLen == 4) || (outLen == 8));
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

	if (outLen == 8)
		v[1] ^= 0xee;

	for (; in != end; in += 4) {
		m = u8_to_32_le(in);
		v[3] ^= m;

		LOGF_TRACE("(%3zu) v[0] %08" PRIx32 "\n", inLen, v[0]);
		LOGF_TRACE("(%3zu) v[1] %08" PRIx32 "\n", inLen, v[1]);
		LOGF_TRACE("(%3zu) v[2] %08" PRIx32 "\n", inLen, v[2]);
		LOGF_TRACE("(%3zu) v[3] %08" PRIx32 "\n", inLen, v[3]);
		for (i = 0; i < cRounds; ++i)
			sipround(v);

		v[0] ^= m;
	}

	switch (left) {
		case 3:
			b |= ((u32)in[2]) << 16;
			[[fallthrough]];
		case 2:
			b |= ((u32)in[1]) << 8;
			[[fallthrough]];
		case 1:
			b |= ((u32)in[0]);
			break;
		case 0:
			break;
	}

	v[3] ^= b;

	LOGF_TRACE("(%3zu) v[0] %08" PRIx32 "\n", inLen, v[0]);
	LOGF_TRACE("(%3zu) v[1] %08" PRIx32 "\n", inLen, v[1]);
	LOGF_TRACE("(%3zu) v[2] %08" PRIx32 "\n", inLen, v[2]);
	LOGF_TRACE("(%3zu) v[3] %08" PRIx32 "\n", inLen, v[3]);
	for (i = 0; i < cRounds; ++i)
		sipround(v);

	v[0] ^= b;

	if (outLen == 8)
		v[2] ^= 0xee;
	else
		v[2] ^= 0xff;

	LOGF_TRACE("(%3zu) v[0] %08" PRIx32 "\n", inLen, v[0]);
	LOGF_TRACE("(%3zu) v[1] %08" PRIx32 "\n", inLen, v[1]);
	LOGF_TRACE("(%3zu) v[2] %08" PRIx32 "\n", inLen, v[2]);
	LOGF_TRACE("(%3zu) v[3] %08" PRIx32 "\n", inLen, v[3]);
	for (i = 0; i < dRounds; ++i)
		sipround(v);

	b = v[1] ^ v[3];
	u32_to_8_le(out, b);

	if (outLen == 4)
		return;

	v[1] ^= 0xdd;

	LOGF_TRACE("(%3zu) v[0] %08" PRIx32 "\n", inLen, v[0]);
	LOGF_TRACE("(%3zu) v[1] %08" PRIx32 "\n", inLen, v[1]);
	LOGF_TRACE("(%3zu) v[2] %08" PRIx32 "\n", inLen, v[2]);
	LOGF_TRACE("(%3zu) v[3] %08" PRIx32 "\n", inLen, v[3]);
	for (i = 0; i < dRounds; ++i)
		sipround(v);

	b = v[1] ^ v[3];
	u32_to_8_le(out + 4, b);
}

HashTable* hash_table_init(const usize sz)
{
	return hash_table_init_with_hash_key(sz, hashTableDefaultHashKey);
}

HashTable* hash_table_init_with_hash_key(const usize sz, u64 customHashKey)
{
	LOGF_TRACE("sz = %zu, customHashKey = %" PRIu64, sz, customHashKey);
	HashTable *ht = malloc(sizeof *ht);
	if(!ht)
		return nullptr;
	Bucket emptyBucket = {};
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_init);
	ht->buckets = vector_init(sizeof emptyBucket);
	if(ht->buckets == nullptr){
		DEBUG_DLCLOSE(vector);
		free(ht);
		return nullptr;
	}
	/* No need to check for errors since we know this won't be out of bounds */
	DEBUG_DLSYM(vector, vector_assign_at);
	[[maybe_unused]] VectorError e = vector_assign_at(ht->buckets, vectorInitialSize - 1, &emptyBucket);
	DEBUG_DLCLOSE(vector);
	/* Now the table is filled with empty buckets */

	ht->maxLoadFactor = hashTableDefaultMaxLoadFactor;
	ht->hashKey = customHashKey;
	ht->sz = sz;
	ht->n = 0;
	return ht;
}

void hash_table_uninit(HashTable *ht)
{
	LOGF_TRACE("HashTable = { .sz = %zu, .n = %u, .buckets = %p }", ht->sz, ht->n, ht->buckets);
	hash_table_uninit_table(ht);
	free(ht);
}

bool hash_table_at(HashTable ht[restrict const static 1], const void *key, const usize keySize, void *outValue)
{
	u32 hash = 0;
	hash_table_half_siphash(keySize, key, (u8*)&ht->hashKey, sizeof hash, (u8*)&hash);

	Bucket foundBucket;
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_at);
	/* NOTE: since this is guaranteed to be within range of the vector, this error can be ignored */
	[[maybe_unused]] VectorError e = vector_at(ht->buckets, hash % vector_capacity(ht->buckets), &foundBucket);

	if(!foundBucket.entries){
		DEBUG_DLCLOSE(vector);
		return false;
	}

	DEBUG_DLSYM(vector, vector_size);
	u32 vecSize = vector_size(foundBucket.entries);
	void *value = nullptr;
	for(u32 i = 0; i < vecSize; ++i){
		BucketEntry bucketEntry;
		/* NOTE: since 'i' only goes up to vector_size this error can be ignored */
		e = vector_at(foundBucket.entries, i, &bucketEntry);

		if(bucketEntry.keySize != keySize)
			continue;
		if(memcmp(bucketEntry.key, key, keySize) == 0){
			value = bucketEntry.value;
			break;
		}
	}
	DEBUG_DLCLOSE(vector);
	if(!value)
		return false;
	
	if(outValue)
		memcpy(outValue, value, ht->sz);

	return true;
}

bool hash_table_empty(HashTable ht[const static 1])
{
	return ht->n == 0;
}

u32 hash_table_size(HashTable ht[const static 1])
{
	return ht->n;
}

usize hash_table_struct_size(HashTable ht[const static 1])
{
	return ht->sz;
}

usize hash_table_capacity(HashTable ht[const static 1])
{
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_capacity);
	usize vecCap = vector_capacity(ht->buckets);
	DEBUG_DLCLOSE(vector);
	return vecCap;
}

f32 hash_table_load_factor(HashTable ht[const static 1])
{
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_capacity);
	usize vecCap = vector_capacity(ht->buckets);
	DEBUG_DLCLOSE(vector);
	return ((f32)ht->n) / vecCap;
}

void hash_table_set_max_load_factor(HashTable ht[static 1], f32 maxLoadFactor)
{
	ht->maxLoadFactor = maxLoadFactor;
}

HashTableError hash_table_reserve(HashTable ht[static 1], u32 reserveSize)
{
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_clone);
	Vector *tempVec = vector_clone(ht->buckets);
	if(!tempVec){
		DEBUG_DLCLOSE(vector);
		return hashTableErrorOutOfMemory;
	}
	DEBUG_DLSYM(vector, vector_reserve);
	VectorError ve = vector_reserve(ht->buckets, reserveSize);

	if(ve == vectorErrorOutOfMemory){
		DEBUG_DLCLOSE(vector);
		return hashTableErrorOutOfMemory;
	}

	DEBUG_DLSYM(vector, vector_zero_all);
	vector_zero_all(ht->buckets); /* vec->n = 0 */
	Bucket emptyBucket = {};
	DEBUG_DLSYM(vector, vector_assign_at);
	DEBUG_DLSYM(vector, vector_capacity);
	/* No need to check for errors since we know this won't be out of bounds */
	ve = vector_assign_at(ht->buckets, vector_capacity(ht->buckets) - 1, &emptyBucket); /* vec->n = reserveSize */
	/* Now the new table space is filled with empty buckets */

	DEBUG_DLSYM(vector, vector_size);
	/* Now just rehash everything */
	u32 oldBucketVecSize = vector_size(tempVec);
	HashTableError he = hashTableErrorSuccess;
	DEBUG_DLSYM(vector, vector_at);
	for(u32 i = 0; i < oldBucketVecSize; ++i){
		Bucket bucket;
		ve = vector_at(tempVec, i, &bucket);

		if(!bucket.entries)
			continue;

		u32 bucketEntriesVecSize = vector_size(bucket.entries);
		for(u32 j = 0; j < bucketEntriesVecSize; ++j){
			BucketEntry oldBucketEntry;
			ve = vector_at(bucket.entries, j, &oldBucketEntry);

			he = hash_table_insert(ht, oldBucketEntry.key, oldBucketEntry.keySize, oldBucketEntry.value);
			if(he != hashTableErrorSuccess){
				hash_table_uninit_table(ht);
				ht->buckets = tempVec; /* Go back to cloned vector */
				DEBUG_DLCLOSE(vector);
				return he;
			}
		}
	}
	DEBUG_DLSYM(vector, vector_uninit);
	/* NOTE: the freeing of memory is separate because we need to make sure that every insertion succeeds (otherwise we can't go back to tempVec) */
	for(u32 i = 0; i < oldBucketVecSize; ++i){
		Bucket bucket;
		ve = vector_at(tempVec, i, &bucket);

		if(!bucket.entries)
			continue;

		u32 bucketEntriesVecSize = vector_size(bucket.entries);
		for(u32 j = 0; j < bucketEntriesVecSize; ++j){
			BucketEntry oldBucketEntry;
			ve = vector_at(bucket.entries, j, &oldBucketEntry);
			free(oldBucketEntry.key);
			free(oldBucketEntry.value);
		}

		vector_uninit(bucket.entries);
	}
	vector_uninit(tempVec);
	DEBUG_DLCLOSE(vector);

	return hashTableErrorSuccess;
}

HashTableError hash_table_insert(HashTable ht[restrict static 1], const void *key, const usize keySize, const void *value)
{
	u32 hash = 0;
	hash_table_half_siphash(keySize, key, (u8*)&ht->hashKey, sizeof hash, (u8*)&hash);

	Bucket foundBucket;
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_at);
	DEBUG_DLSYM(vector, vector_capacity);
	/* NOTE: since this is guaranteed to be within range of the vector, this error can be ignored */
	[[maybe_unused]] VectorError e = vector_at(ht->buckets, hash % vector_capacity(ht->buckets), &foundBucket);

	DEBUG_DLSYM(vector, vector_size);
	u32 vecSize = vector_size(foundBucket.entries);
	for(u32 i = 0; i < vecSize; ++i){
		BucketEntry bucketEntry;
		/* NOTE: since 'i' only goes up to vector_size this error can be ignored */
		e = vector_at(foundBucket.entries, i, &bucketEntry);

		if(bucketEntry.keySize != keySize)
			continue;
		if(memcmp(bucketEntry.key, key, keySize) == 0){
			DEBUG_DLCLOSE(vector);
			return hashTableErrorKeyAlreadyExists;
		}
	}

	BucketEntry bucketEntry = { .keySize = keySize, .key = malloc(keySize), .value = malloc(ht->sz) };

	if(!bucketEntry.key || !bucketEntry.value)
		goto error_free_memory;

	memcpy(bucketEntry.key, key, keySize);
	memcpy(bucketEntry.value, value, ht->sz);

	if(!foundBucket.entries){ /* A new bucket entry list needs to be created */
		DEBUG_DLSYM(vector, vector_init);
		foundBucket.entries = vector_init(sizeof(BucketEntry));
		if(!foundBucket.entries)
			goto error_free_memory;
	}

	DEBUG_DLSYM(vector, vector_push_back);
	e = vector_push_back(foundBucket.entries, &bucketEntry);
	if(e != vectorErrorSuccess)
		goto error_free_memory;

	if(hash_table_load_factor(ht) > ht->maxLoadFactor){
		HashTableError he = hash_table_reserve(ht, vector_capacity(ht->buckets) * 2);
		if(he != hashTableErrorSuccess){
			DEBUG_DLCLOSE(vector);
			return he;
		}
	}

	++ht->n;

	return hashTableErrorSuccess;

error_free_memory:
	DEBUG_DLCLOSE(vector);
	if(bucketEntry.key)
		free(bucketEntry.key);
	if(bucketEntry.value)
		free(bucketEntry.value);
	return hashTableErrorOutOfMemory;
}

HashTableError hash_table_erase(HashTable ht[static 1], const void *key, const usize keySize)
{
	u32 hash = 0;
	hash_table_half_siphash(keySize, key, (u8*)&ht->hashKey, sizeof hash, (u8*)&hash);

	Bucket foundBucket;
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_at);
	DEBUG_DLSYM(vector, vector_capacity);
	/* NOTE: since this is guaranteed to be within range of the vector, this error can be ignored */
	[[maybe_unused]] VectorError e = vector_at(ht->buckets, hash % vector_capacity(ht->buckets), &foundBucket);

	if(!foundBucket.entries){
		DEBUG_DLCLOSE(vector);
		return hashTableErrorKeyNotFound;
	}

	DEBUG_DLSYM(vector, vector_size);
	u32 vecSize = vector_size(foundBucket.entries);
	DEBUG_DLSYM(vector, vector_erase_at);
	for(u32 i = 0; i < vecSize; ++i){
		BucketEntry bucketEntry;
		/* NOTE: since 'i' only goes up to vector_size this error can be ignored */
		e = vector_at(foundBucket.entries, i, &bucketEntry);

		if(bucketEntry.keySize != keySize)
			continue;
		if(memcmp(bucketEntry.key, key, keySize) == 0){
			e = vector_erase_at(foundBucket.entries, i);
			if(e == vectorErrorOutOfMemory){
				DEBUG_DLCLOSE(vector);
				return hashTableErrorOutOfMemory;
			}
			free(bucketEntry.value);
			free(bucketEntry.key);
			--ht->n;
			DEBUG_DLCLOSE(vector);
			return hashTableErrorSuccess;
		}
	}

	/* TODO: maybe resize down? */

	DEBUG_DLCLOSE(vector);
	return hashTableErrorKeyNotFound;
}

HashTableError hash_table_clear(HashTable ht[static 1])
{
	hash_table_uninit_table(ht);
	Bucket emptyBucket = {};
	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_init);
	ht->buckets = vector_init(sizeof emptyBucket);
	if(ht->buckets == nullptr){
		DEBUG_DLCLOSE(vector);
		return hashTableErrorOutOfMemory;
	}
	DEBUG_DLSYM(vector, vector_assign_at);
	/* No need to check for errors since we know this won't be out of bounds */
	[[maybe_unused]] VectorError e = vector_assign_at(ht->buckets, vectorInitialSize - 1, &emptyBucket);
	/* Now the table is filled with empty buckets */

	ht->n = 0;

	DEBUG_DLCLOSE(vector);
	return hashTableErrorSuccess;
}

