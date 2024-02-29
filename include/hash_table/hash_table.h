#pragma once

#include "types.h"

constexpr u64 hashTableDefaultHashKey = 0x0123456789ABCDEF;
constexpr f32 hashTableDefaultMaxLoadFactor = 1.f;

#ifndef HASH_TABLE_INTERNAL
typedef Handle HashTable;
#endif

typedef enum [[nodiscard("Required to avoid segfaults")]]
{
	hashTableErrorOutOfMemory,
	hashTableErrorSuccess,
	hashTableErrorKeyNotFound,
	hashTableErrorKeyAlreadyExists,
	hashTableErrorOutOfBounds,
} HashTableError;

#undef DLL_FUNCTIONS
#define DLL_FUNCTIONS \
DLL_X(hash_table_half_siphash, void, const usize inLen, const u8 in[restrict static inLen], const u8 k[restrict static 8], const usize outLen, u8 out[restrict static outLen]) \
DLL_X(hash_table_init, HashTable*, const usize sz) \
DLL_X(hash_table_init_with_hash_key, HashTable*, const usize sz, u64 customHashKey) \
DLL_X(hash_table_uninit, void, HashTable *ht) \
DLL_X(hash_table_at, bool, HashTable ht[restrict const static 1], const void *key, const usize keySize, void *outValue) \
DLL_X(hash_table_empty, bool, HashTable ht[const static 1]) \
DLL_X(hash_table_size, u32, HashTable ht[const static 1]) \
DLL_X(hash_table_struct_size, usize, HashTable ht[const static 1]) \
DLL_X(hash_table_capacity, usize, HashTable ht[const static 1]) \
DLL_X(hash_table_load_factor, f32, HashTable ht[const static 1]) \
DLL_X(hash_table_set_max_load_factor, void, HashTable ht[static 1], f32 maxLoadFactor) \
DLL_X(hash_table_reserve, HashTableError, HashTable ht[static 1], u32 reserveSize) \
DLL_X(hash_table_insert, HashTableError, HashTable ht[restrict static 1], const void *key, const usize keySize, const void *value) \
DLL_X(hash_table_erase, HashTableError, HashTable ht[static 1], const void *key, const usize keySize) \
DLL_X(hash_table_clear, HashTableError, HashTable ht[static 1])

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(hash_table)
#include "dll/plugger.h"

