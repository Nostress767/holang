#pragma once

#include "dll_header.h"
#include "types.h"

#ifndef VECTOR_INTERNAL
typedef Handle Vector;
#endif

#undef FUNCTIONS
#define FUNCTIONS \
DEBUG_X(vector_init, Vector*, const usize sz)\
DEBUG_X(vector_uninit, void, Vector *vec)\
DEBUG_X(vector_swap, void, Vector *vecA, Vector *vecB)\
\
DEBUG_X(vector_at, void, const Vector *vec, const usize index, void *out)\
DEBUG_X(vector_front, void, const Vector *vec, void *out)\
DEBUG_X(vector_back, void, const Vector *vec, void *out)\
DEBUG_X(vector_get_data, const u8*, const Vector *vec)\
\
DEBUG_X(vector_empty, bool, const Vector *vec)\
DEBUG_X(vector_size, u32, const Vector *vec)\
DEBUG_X(vector_struct_size, usize, const Vector *vec)\
DEBUG_X(vector_capacity, usize, const Vector *vec)\
DEBUG_X(vector_reserve, void, Vector *vec, u32 reserveSize)\
\
DEBUG_X(vector_push_back, void, Vector *vec, const void *data)\
DEBUG_X(vector_insert_at, void, Vector *vec, const usize index, const void *data)\
DEBUG_X(vector_erase_at, void, Vector *vec, const usize index)\
DEBUG_X(vector_pop_back, void, Vector *vec)\
DEBUG_X(vector_clear, void, Vector *vec)\
DEBUG_X(vector_sort, void, Vector *vec, int (*compar)(const void*, const void*))

#include "common_dll.h"

DEBUG_DECLARE_VTABLE(vector)

