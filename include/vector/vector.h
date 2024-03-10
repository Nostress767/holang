#pragma once

#include "types.h"

constexpr usize vectorInitialSize = 1 << 7;

enum VectorError : u32
{
	vectorErrorOutOfMemory,
	vectorErrorSuccess,
	vectorErrorOutOfBounds,
	vectorErrorEmpty,
	vectorErrorNullInputData,
	vectorErrorNullOutputData,
};

typedef struct Vector
{
	usize vectorSize; 	    /* Internal vector size */
	usize sz;	  	    /* Size of element      */
	u8 *data;	  	    /* Raw data of elements */
	u32 n;		  	    /* Amount of elements   */
	enum VectorError lastError; /* Last occured error   */

	/* Function used to manage element memory in the vector */
	const void* (*customAllocator)(const void *el, u8 *vecData);
} Vector;

#undef DLL_FUNCTIONS
#define DLL_FUNCTIONS \
DLL_X(vector_init    , Vector*, const usize sz)\
DLL_X(vector_init_with_allocator    , Vector*, const usize sz, const void* (*customAllocator)(const void *el, u8 *vecData))\
DLL_X(vector_uninit  , void, Vector *vec)\
DLL_X(vector_clone   , Vector*, Vector vec[static 1])\
DLL_X(vector_swap    , void, Vector vecA[restrict static 1], Vector vecB[restrict static 1])\
\
DLL_X(vector_at            , void*, Vector vec[restrict const static 1], const usize index)\
DLL_X(vector_front         , void*, Vector vec[restrict const static 1])\
DLL_X(vector_back          , void*, Vector vec[restrict const static 1])\
\
DLL_X(vector_reserve    , void, Vector vec[static 1], u32 reserveSize)\
\
DLL_X(vector_push_back, void, Vector vec[restrict static 1], const void *data)\
DLL_X(vector_assign_at, void, Vector vec[restrict static 1], const usize index, const void *data)\
DLL_X(vector_insert_at, void, Vector vec[restrict static 1], const usize index, const void *data)\
DLL_X(vector_erase_at , void, Vector vec[static 1], const usize index)\
DLL_X(vector_pop_back , void, Vector vec[static 1])\
DLL_X(vector_clear    , void, Vector vec[static 1])\
DLL_X(vector_sort     , void, Vector vec[static 1], int (*compar)(const void*, const void*))

static inline const u8* vector_get_data(Vector vec[const static 1])
{
	return vec->data;
}

static inline enum VectorError vector_get_last_error(Vector vec[const static 1])
{
	return vec->lastError;
}

static inline bool vector_empty(Vector vec[const static 1])
{
	return vec->n == 0;
}

static inline u32 vector_size(Vector vec[const static 1])
{
	return vec->n;
}

static inline usize vector_element_sz(Vector vec[const static 1])
{
	return vec->sz;
}

static inline usize vector_capacity(Vector vec[const static 1])
{
	return vec->vectorSize;
}

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(vector)
#include "dll/plugger.h"

