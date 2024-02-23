#pragma once

#include "types.h"

constexpr usize vectorInitialSize = 128;

#ifndef VECTOR_INTERNAL
typedef Handle Vector;
#endif

typedef enum [[nodiscard("Required to avoid segfaults")]]
{
	vectorErrorSuccess,
	vectorErrorOutOfBounds,
	vectorErrorEmpty,
	vectorErrorOutOfMemory,
} VectorError;

#undef DLL_FUNCTIONS
#define DLL_FUNCTIONS \
DLL_X(vector_init    , Vector*, const usize sz)\
DLL_X(vector_uninit  , void, Vector *vec)\
DLL_X(vector_swap    , void, Vector vecA[restrict static 1], Vector vecB[restrict static 1])\
\
DLL_X(vector_at      , VectorError, Vector vec[restrict const static 1], const usize index, void *out)\
DLL_X(vector_front   , VectorError, Vector vec[restrict const static 1], void *out)\
DLL_X(vector_back    , VectorError, Vector vec[restrict const static 1], void *out)\
DLL_X(vector_get_data, const u8*, Vector vec[const static 1])\
\
DLL_X(vector_empty      , bool, Vector vec[const static 1])\
DLL_X(vector_size       , u32, Vector vec[const static 1])\
DLL_X(vector_struct_size, usize, Vector vec[const static 1])\
DLL_X(vector_capacity   , usize, Vector vec[const static 1])\
DLL_X(vector_reserve    , VectorError, Vector vec[static 1], u32 reserveSize)\
\
DLL_X(vector_push_back, VectorError, Vector vec[restrict static 1], const void *data)\
DLL_X(vector_insert_at, VectorError, Vector vec[restrict static 1], const usize index, const void *data)\
DLL_X(vector_erase_at , VectorError, Vector vec[static 1], const usize index)\
DLL_X(vector_pop_back , VectorError, Vector vec[static 1])\
DLL_X(vector_clear    , VectorError, Vector vec[static 1])\
DLL_X(vector_sort     , void, Vector vec[static 1], int (*compar)(const void*, const void*))

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(vector)
#include "dll/plugger.h"

