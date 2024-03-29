#include <string.h>

#include "hash_table/hash_table.h"
#include "external/unity.h"
#include "external/unity_memory.h"
#include "external/unity_fixture.h"

u32 string_key_hash(const void *key)
{
	if(!key)
		return 0;

	char *strKey = *(char**)key;
	const usize strLen = strlen(strKey);
	if(strLen == 0)
		return 1;

	return hash_table_half_siphash(strLen, (u8*)strKey, (u8*)&hashTableDefaultHashKey);
}

int string_key_comp(const void *a, const void *b)
{
	char *strKeyA = *(char**)a;
	char *strKeyB = *(char**)b;

	return strcmp(strKeyA, strKeyB);
}

const void* string_key_allocator(const void *key, const void *value, usize entrySz, u8 *bucketData)
{
	const usize keySz = sizeof(char*);
	const usize valueSz = entrySz - keySz;
	if(key){
		char *strKey = *(char**)key;
		usize strSz = strlen(strKey);
		char *newStr = malloc(strSz + 1);
		if(!newStr)
			return nullptr;
		strncpy(newStr, strKey, strSz + 1);
		newStr[strSz] = '\0';
		memcpy(bucketData, &newStr, keySz);
		return newStr;
	}
	else{
		char *toFree;
		memcpy(&toFree, bucketData, keySz);
		free(toFree);
		return nullptr;
	}
	memcpy(bucketData + keySz, value, valueSz);
}

/* NOTE: these test vectors were taken directly from the reference implementation's tests */
/* Which can be found here: https://github.com/veorq/SipHash/blob/master/vectors.h */
constexpr u8 hSip32OutSize = 4;
const u8 vectorsHSip32[64][hSip32OutSize] = {
    { 0xa9, 0x35, 0x9f, 0x5b, },
    { 0x27, 0x47, 0x5a, 0xb8, },
    { 0xfa, 0x62, 0xa6, 0x03, },
    { 0x8a, 0xfe, 0xe7, 0x04, },
    { 0x2a, 0x6e, 0x46, 0x89, },
    { 0xc5, 0xfa, 0xb6, 0x69, },
    { 0x58, 0x63, 0xfc, 0x23, },
    { 0x8b, 0xcf, 0x63, 0xc5, },
    { 0xd0, 0xb8, 0x84, 0x8f, },
    { 0xf8, 0x06, 0xe7, 0x79, },
    { 0x94, 0xb0, 0x79, 0x34, },
    { 0x08, 0x08, 0x30, 0x50, },
    { 0x57, 0xf0, 0x87, 0x2f, },
    { 0x77, 0xe6, 0x63, 0xff, },
    { 0xd6, 0xff, 0xf8, 0x7c, },
    { 0x74, 0xfe, 0x2b, 0x97, },
    { 0xd9, 0xb5, 0xac, 0x84, },
    { 0xc4, 0x74, 0x64, 0x5b, },
    { 0x46, 0x5b, 0x8d, 0x9b, },
    { 0x7b, 0xef, 0xe3, 0x87, },
    { 0xe3, 0x4d, 0x10, 0x45, },
    { 0x61, 0x3f, 0x62, 0xb3, },
    { 0x70, 0xf3, 0x67, 0xfe, },
    { 0xe6, 0xad, 0xb8, 0xbd, },
    { 0x27, 0x40, 0x0c, 0x63, },
    { 0x26, 0x78, 0x78, 0x75, },
    { 0x4f, 0x56, 0x7b, 0x5f, },
    { 0x3a, 0xb0, 0xe6, 0x69, },
    { 0xb0, 0x64, 0x40, 0x00, },
    { 0xff, 0x67, 0x0f, 0xb4, },
    { 0x50, 0x9e, 0x33, 0x8b, },
    { 0x5d, 0x58, 0x9f, 0x1a, },
    { 0xfe, 0xe7, 0x21, 0x12, },
    { 0x33, 0x75, 0x32, 0x59, },
    { 0x6a, 0x43, 0x4f, 0x8c, },
    { 0xfe, 0x28, 0xb7, 0x29, },
    { 0xe7, 0x5c, 0xc6, 0xec, },
    { 0x69, 0x7e, 0x8d, 0x54, },
    { 0x63, 0x68, 0x8b, 0x0f, },
    { 0x65, 0x0b, 0x62, 0xb4, },
    { 0xb6, 0xbc, 0x18, 0x40, },
    { 0x5d, 0x07, 0x45, 0x05, },
    { 0x24, 0x42, 0xfd, 0x2e, },
    { 0x7b, 0xb7, 0x86, 0x3a, },
    { 0x77, 0x05, 0xd5, 0x48, },
    { 0xd7, 0x52, 0x08, 0xb1, },
    { 0xb6, 0xd4, 0x99, 0xc8, },
    { 0x08, 0x92, 0x20, 0x2e, },
    { 0x69, 0xe1, 0x2c, 0xe3, },
    { 0x8d, 0xb5, 0x80, 0xe5, },
    { 0x36, 0x97, 0x64, 0xc6, },
    { 0x01, 0x6e, 0x02, 0x04, },
    { 0x3b, 0x85, 0xf3, 0xd4, },
    { 0xfe, 0xdb, 0x66, 0xbe, },
    { 0x1e, 0x69, 0x2a, 0x3a, },
    { 0xc6, 0x89, 0x84, 0xc0, },
    { 0xa5, 0xc5, 0xb9, 0x40, },
    { 0x9b, 0xe9, 0xe8, 0x8c, },
    { 0x7d, 0xbc, 0x81, 0x40, },
    { 0x7c, 0x07, 0x8e, 0xc5, },
    { 0xd4, 0xe7, 0x6c, 0x73, },
    { 0x42, 0x8f, 0xcb, 0xb9, },
    { 0xbd, 0x83, 0x99, 0x7a, },
    { 0x59, 0xea, 0x4a, 0x74, },
};

constexpr u8 hSip64OutSize = 8;
const u8 vectorsHSip64[64][hSip64OutSize] = {
    { 0x21, 0x8d, 0x1f, 0x59, 0xb9, 0xb8, 0x3c, 0xc8, },
    { 0xbe, 0x55, 0x24, 0x12, 0xf8, 0x38, 0x73, 0x15, },
    { 0x06, 0x4f, 0x39, 0xef, 0x7c, 0x50, 0xeb, 0x57, },
    { 0xce, 0x0f, 0x1a, 0x45, 0xf7, 0x06, 0x06, 0x79, },
    { 0xd5, 0xe7, 0x8a, 0x17, 0x5b, 0xe5, 0x2e, 0xa1, },
    { 0xcb, 0x9d, 0x7c, 0x3f, 0x2f, 0x3d, 0xb5, 0x80, },
    { 0xce, 0x3e, 0x91, 0x35, 0x8a, 0xa2, 0xbc, 0x25, },
    { 0xff, 0x20, 0x27, 0x28, 0xb0, 0x7b, 0xc6, 0x84, },
    { 0xed, 0xfe, 0xe8, 0x20, 0xbc, 0xe4, 0x85, 0x8c, },
    { 0x5b, 0x51, 0xcc, 0xcc, 0x13, 0x88, 0x83, 0x07, },
    { 0x95, 0xb0, 0x46, 0x9f, 0x06, 0xa6, 0xf2, 0xee, },
    { 0xae, 0x26, 0x33, 0x39, 0x94, 0xdd, 0xcd, 0x48, },
    { 0x7b, 0xc7, 0x1f, 0x9f, 0xae, 0xf5, 0xc7, 0x99, },
    { 0x5a, 0x23, 0x52, 0xd7, 0x5a, 0x0c, 0x37, 0x44, },
    { 0x3b, 0xb1, 0xa8, 0x70, 0xea, 0xe8, 0xe6, 0x58, },
    { 0x21, 0x7d, 0x0b, 0xcb, 0x4e, 0x81, 0xc9, 0x02, },
    { 0x73, 0x36, 0xaa, 0xd2, 0x5f, 0x7b, 0xf3, 0xb5, },
    { 0x37, 0xad, 0xc0, 0x64, 0x1c, 0x4c, 0x4f, 0x6a, },
    { 0xc9, 0xb2, 0xdb, 0x2b, 0x9a, 0x3e, 0x42, 0xf9, },
    { 0xf9, 0x10, 0xe4, 0x80, 0x20, 0xab, 0x36, 0x3c, },
    { 0x1b, 0xf5, 0x2b, 0x0a, 0x6f, 0xee, 0xa7, 0xdb, },
    { 0x00, 0x74, 0x1d, 0xc2, 0x69, 0xe8, 0xb3, 0xef, },
    { 0xe2, 0x01, 0x03, 0xfa, 0x1b, 0xa7, 0x76, 0xef, },
    { 0x4c, 0x22, 0x10, 0xe5, 0x4b, 0x68, 0x1d, 0x73, },
    { 0x70, 0x74, 0x10, 0x45, 0xae, 0x3f, 0xa6, 0xf1, },
    { 0x0c, 0x86, 0x40, 0x37, 0x39, 0x71, 0x40, 0x38, },
    { 0x0d, 0x89, 0x9e, 0xd8, 0x11, 0x29, 0x23, 0xf0, },
    { 0x22, 0x6b, 0xf5, 0xfa, 0xb8, 0x1e, 0xe1, 0xb8, },
    { 0x2d, 0x92, 0x5f, 0xfb, 0x1e, 0x00, 0x16, 0xb5, },
    { 0x36, 0x19, 0x58, 0xd5, 0x2c, 0xee, 0x10, 0xf1, },
    { 0x29, 0x1a, 0xaf, 0x86, 0x48, 0x98, 0x17, 0x9d, },
    { 0x86, 0x3c, 0x7f, 0x15, 0x5c, 0x34, 0x11, 0x7c, },
    { 0x28, 0x70, 0x9d, 0x46, 0xd8, 0x11, 0x62, 0x6c, },
    { 0x24, 0x84, 0x77, 0x68, 0x1d, 0x28, 0xf8, 0x9c, },
    { 0x83, 0x24, 0xe4, 0xd7, 0x52, 0x8f, 0x98, 0x30, },
    { 0xf9, 0xef, 0xd4, 0xe1, 0x3a, 0xea, 0x6b, 0xd8, },
    { 0x86, 0xd6, 0x7a, 0x40, 0xec, 0x42, 0x76, 0xdc, },
    { 0x3f, 0x62, 0x92, 0xec, 0xcc, 0xa9, 0x7e, 0x35, },
    { 0xcb, 0xd9, 0x2e, 0xe7, 0x24, 0xd4, 0x21, 0x09, },
    { 0x36, 0x8d, 0xf6, 0x80, 0x8d, 0x40, 0x3d, 0x79, },
    { 0x5b, 0x38, 0xc8, 0x1c, 0x67, 0xc8, 0xae, 0x4c, },
    { 0x95, 0xab, 0x71, 0x89, 0xd4, 0x39, 0xac, 0xb3, },
    { 0xa9, 0x1a, 0x52, 0xc0, 0x25, 0x32, 0x70, 0x24, },
    { 0x5b, 0x00, 0x87, 0xc6, 0x95, 0x28, 0xac, 0xea, },
    { 0x1e, 0x30, 0xf3, 0xad, 0x27, 0xdc, 0xb1, 0x5a, },
    { 0x69, 0x7f, 0x5c, 0x9a, 0x90, 0x32, 0x4e, 0xd4, },
    { 0x49, 0x5c, 0x0f, 0x99, 0x55, 0x57, 0xdc, 0x38, },
    { 0x94, 0x27, 0x20, 0x2a, 0x3c, 0x29, 0xf9, 0x4d, },
    { 0xa9, 0xea, 0xa8, 0xc0, 0x4b, 0xa9, 0x3e, 0x3e, },
    { 0xee, 0xa4, 0xc1, 0x73, 0x7d, 0x01, 0x12, 0x18, },
    { 0x91, 0x2d, 0x56, 0x8f, 0xd8, 0xf6, 0x5a, 0x49, },
    { 0x56, 0x91, 0x95, 0x96, 0xb0, 0xff, 0x5c, 0x97, },
    { 0x02, 0x44, 0x5a, 0x79, 0x98, 0xf5, 0x50, 0xe1, },
    { 0x86, 0xec, 0x46, 0x6c, 0xe7, 0x1d, 0x1f, 0xb2, },
    { 0x35, 0x95, 0x69, 0xe7, 0xd2, 0x89, 0xe3, 0xbc, },
    { 0x87, 0x1b, 0x05, 0xca, 0x62, 0xbb, 0x7c, 0x96, },
    { 0xa1, 0xa4, 0x92, 0xf9, 0x42, 0xf1, 0x5f, 0x1d, },
    { 0x12, 0xec, 0x26, 0x7f, 0xf6, 0x09, 0x5b, 0x6e, },
    { 0x5d, 0x1b, 0x5e, 0xa1, 0xb2, 0x31, 0xd8, 0x9d, },
    { 0xd8, 0xcf, 0xb4, 0x45, 0x3f, 0x92, 0xee, 0x54, },
    { 0xd6, 0x76, 0x28, 0x90, 0xbf, 0x26, 0xe4, 0x60, },
    { 0x31, 0x35, 0x63, 0xa4, 0xb7, 0xed, 0x5c, 0xf3, },
    { 0xf9, 0x0b, 0x3a, 0xb5, 0x72, 0xd4, 0x66, 0x93, },
    { 0x2e, 0xa6, 0x3c, 0x71, 0xbf, 0x32, 0x60, 0x87, },
};

TEST_GROUP(hash_table);

TEST_SETUP(hash_table)
{
	UnityMalloc_StartTest();
}

TEST_TEAR_DOWN(hash_table)
{
	UnityMalloc_EndTest();
}

TEST(hash_table, hash_table_half_siphash)
{
	constexpr u8 inLen = 64;
	constexpr u8 kLen = 8;
	u8 in[inLen];
	u8 k[kLen];
	for(u8 i = 0; i < inLen; ++i)
		in[i] = i;
	for(u8 i = 0; i < kLen; ++i)
		k[i] = i;
	for(usize i = 0; i < sizeof vectorsHSip32 / sizeof *vectorsHSip32; ++i){
		u32 out = hash_table_half_siphash(i, in, k);
		TEST_ASSERT_EQUAL(0, memcmp(&out, vectorsHSip32[i], hSip32OutSize));
	}
	/*
	for(usize i = 0; i < sizeof vectorsHSip64 / sizeof *vectorsHSip64; ++i){
		u32 out = hash_table_half_siphash(i, in, k);
		TEST_ASSERT_EQUAL(0, memcmp(&out, vectorsHSip64[i], hSip64OutSize));
	}
	*/
}

TEST(hash_table, hash_table_init)
{
	u32 someKey = 0, someValue = 0;
	constexpr usize keySz = sizeof someKey;
	constexpr usize valueSz = sizeof someValue;
	HashTable *ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	hash_table_uninit(ht);

	UnityMalloc_MakeMallocFailAfterCount(0);
	ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NULL(ht);

	UnityMalloc_MakeMallocFailAfterCount(1);
	ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NULL(ht);
}

TEST(hash_table, hash_table_reserve)
{
	/* u32 -> u32 */
	u32 someKey = 0, someValue = 0;
	usize keySz = sizeof someKey, valueSz = sizeof someValue;

	HashTable *ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	usize htCapacity = hash_table_capacity(ht);
	hash_table_reserve(ht, SIZE_MAX);
	TEST_ASSERT_EQUAL(hashTableErrorOutOfMemory, hash_table_get_last_error(ht));
	TEST_ASSERT_EQUAL(htCapacity, hash_table_capacity(ht));

	hash_table_reserve(ht, hash_table_capacity(ht)); /* Do nothing */
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	UnityMalloc_MakeMallocFailAfterCount(0); /* First calloc */
	hash_table_reserve(ht, 2 * htCapacity);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(hashTableErrorOutOfMemory, hash_table_get_last_error(ht));

	for(u32 i = 0; i <= hashTableInitialSize; ++i){
		someKey = i;
		hash_table_insert(ht, &someKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	UnityMalloc_MakeMallocFailAfterCount(1); /* New entry in bucket */
	hash_table_reserve(ht, 4 * htCapacity);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(hashTableErrorOutOfMemory, hash_table_get_last_error(ht));

	UnityMalloc_MakeMallocFailAfterCount(2); /* Second new entry in bucket */
	hash_table_reserve(ht, 4 * htCapacity);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(hashTableErrorOutOfMemory, hash_table_get_last_error(ht));

	hash_table_uninit(ht);
}

TEST(hash_table, hash_table_insert)
{
	/* u32 -> u32 */
	u32 someKey = 0, someValue = 0;
	usize keySz = sizeof someKey, valueSz = sizeof someValue;

	HashTable *ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	hash_table_insert(ht, nullptr, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorNullKeyInput, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someKey, nullptr);
	TEST_ASSERT_EQUAL(hashTableErrorNullValueInput, hash_table_get_last_error(ht));

	UnityMalloc_MakeMallocFailAfterCount(0);
	hash_table_insert(ht, &someKey, &someValue);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(hashTableErrorOutOfMemory, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorKeyAlreadyExists, hash_table_get_last_error(ht));

	for(u32 i = 1; i <= hashTableInitialSize; ++i){
		someKey = i;
		someValue = i + hashTableInitialSize + 2;
		hash_table_insert(ht, &someKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	TEST_ASSERT_EQUAL(1 + hashTableInitialSize, hash_table_size(ht));

	someKey = 2 + hashTableInitialSize;
	UnityMalloc_MakeMallocFailAfterCount(0);
	hash_table_insert(ht, &someKey, &someValue);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(hashTableErrorOutOfMemory, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	TEST_ASSERT_EQUAL(2 * hashTableInitialSize, hash_table_capacity(ht));

	for(u32 i = someKey + 1; i < 1'000'000; ++i){
		someKey = i;
		hash_table_insert(ht, &someKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	hash_table_uninit(ht);

	/* literal char* -> u32 */
	char *someStrKey = "Key";
	keySz = sizeof someStrKey, valueSz = sizeof someValue;

	ht = hash_table_init_with_hash_and_keycomp(keySz, valueSz, string_key_hash, string_key_comp);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someStrKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someStrKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorKeyAlreadyExists, hash_table_get_last_error(ht));

	someStrKey = "Another key";
	hash_table_insert(ht, &someStrKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	hash_table_uninit(ht);

	/* char* -> u32 */
	someStrKey = malloc(256);
	keySz = sizeof someStrKey, valueSz = sizeof someValue;

	ht = hash_table_init_with_hash_keycomp_and_allocator(keySz, valueSz, string_key_hash, string_key_comp, string_key_allocator);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	strcpy(someStrKey, "Key 0");
	hash_table_insert(ht, &someStrKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someStrKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorKeyAlreadyExists, hash_table_get_last_error(ht));

	for(u32 i = 1; i < 1'000'000; ++i){
		sprintf(someStrKey, "Key %u", i);
		hash_table_insert(ht, &someStrKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	strcpy(someStrKey, "Another key");
	UnityMalloc_MakeMallocFailAfterCount(0);
	hash_table_insert(ht, &someStrKey, &someValue);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(hashTableErrorOutOfMemory, hash_table_get_last_error(ht));

	hash_table_insert(ht, &someStrKey, &someValue);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	free(someStrKey);

	hash_table_uninit(ht);

}

TEST(hash_table, hash_table_at)
{
	/* u32 -> u32 */
	u32 someKey, someValue;
	usize keySz = sizeof someKey, valueSz = sizeof someValue;

	HashTable *ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	constexpr u32 pairAmount = 1'000'000;
	u32 *someValues = malloc(pairAmount * sizeof someValue);
	for(u32 i = 0; i < pairAmount; ++i){
		someKey = i;
		someValue = rand();
		someValues[i] = someValue;
		hash_table_insert(ht, &someKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	for(u32 i = 0; i < pairAmount; ++i){
		u32 *someValueFromHashTable = hash_table_at(ht, &i);
		TEST_ASSERT_NOT_NULL(someValueFromHashTable);
		TEST_ASSERT_EQUAL(someValues[i], *someValueFromHashTable);
	}

	free(someValues);

	hash_table_uninit(ht);
}

TEST(hash_table, hash_table_erase)
{
	/* u32 -> u32 */
	u32 someKey, someValue;
	usize keySz = sizeof someKey, valueSz = sizeof someValue;

	HashTable *ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	hash_table_erase(ht, nullptr);
	TEST_ASSERT_EQUAL(hashTableErrorNullKeyInput, hash_table_get_last_error(ht));

	constexpr u32 pairAmount = 1'000'000;
	for(u32 i = 0; i < pairAmount; ++i){
		someKey = i;
		someValue = rand();
		hash_table_insert(ht, &someKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	u32 somePlace = 767;
	u32 *someValueFromHashTable = hash_table_at(ht, &somePlace);
	TEST_ASSERT_NOT_NULL(someValueFromHashTable);

	somePlace = pairAmount + 1;
	hash_table_erase(ht, &somePlace);
	TEST_ASSERT_EQUAL(hashTableErrorKeyNotFound, hash_table_get_last_error(ht));

	for(u32 i = 0; i < pairAmount; ++i){
		someKey = i;
		hash_table_erase(ht, &someKey);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	someValueFromHashTable = hash_table_at(ht, &somePlace);
	TEST_ASSERT_NULL(someValueFromHashTable);

	hash_table_uninit(ht);

	/* char* -> u32 */
	char* someStrKey = malloc(256);
	keySz = sizeof someStrKey, valueSz = sizeof someValue;

	ht = hash_table_init_with_hash_keycomp_and_allocator(keySz, valueSz, string_key_hash, string_key_comp, string_key_allocator);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	for(u32 i = 0; i < pairAmount; ++i){
		sprintf(someStrKey, "Key %u", i);
		hash_table_insert(ht, &someStrKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	for(u32 i = 0; i < pairAmount; ++i){
		sprintf(someStrKey, "Key %u", i);
		hash_table_erase(ht, &someStrKey);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	free(someStrKey);

	hash_table_uninit(ht);
}

TEST(hash_table, hash_table_clear)
{
	/* u32 -> u32 */
	u32 someKey, someValue;
	usize keySz = sizeof someKey, valueSz = sizeof someValue;

	HashTable *ht = hash_table_init(keySz, valueSz);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	for(u32 i = 0; i < 1'000'000; ++i){
		someKey = i;
		someValue = rand();
		hash_table_insert(ht, &someKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	hash_table_clear(ht);
	TEST_ASSERT_EQUAL(0, hash_table_size(ht));

	hash_table_uninit(ht);

	/* char* -> u32 */
	char* someStrKey = malloc(256);
	keySz = sizeof someStrKey, valueSz = sizeof someValue;

	ht = hash_table_init_with_hash_keycomp_and_allocator(keySz, valueSz, string_key_hash, string_key_comp, string_key_allocator);
	TEST_ASSERT_NOT_NULL(ht);
	TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));

	for(u32 i = 0; i < 1'000'000; ++i){
		sprintf(someStrKey, "Key %u", i);
		hash_table_insert(ht, &someStrKey, &someValue);
		TEST_ASSERT_EQUAL(hashTableErrorSuccess, hash_table_get_last_error(ht));
	}

	hash_table_clear(ht);
	TEST_ASSERT_EQUAL(0, hash_table_size(ht));

	free(someStrKey);

	hash_table_uninit(ht);
}

