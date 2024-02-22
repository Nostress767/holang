#define DLL_HEADER_SOURCE
#include "vector/vector_internal.h"
#undef DLL_HEADER_SOURCE

#undef DEBUG_X
#define DEBUG_X(fName, ret, ...) fName,
DEBUG_DEFINE_VTABLE(vector)

#include "log.h"

#include <stdlib.h>
#include <string.h>

constexpr usize initialSize = 128;

Vector* vector_init(const usize sz)
{
	LOGF_TRACE("sz = %zu", sz);
	Vector *vec = malloc(sizeof *vec);
	vec->data = malloc(sizeof(u8[initialSize]) * sz);
	vec->sz = sz;
	vec->vectorSize = initialSize;
	vec->n = 0;
	return vec;
}

void vector_uninit(Vector *vec)
{
	LOGF_TRACE("Vector = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data);
	free(vec->data);
	free(vec);
}

void vector_swap(Vector *vecA, Vector *vecB)
{
	Vector tmp = *vecA;
	*vecA = *vecB;
	*vecB = tmp;
}

void vector_at(const Vector *vec, const usize index, void *out)
{
	/* LOGF_TRACE("index = %zu", index); */
	if(index < vec->n)
		memcpy(out, &vec->data[index], vec->sz);
}

void vector_front(const Vector *vec, void *out)
{
	if(vec->n)
		memcpy(out, &vec->data[0], vec->sz);
}

void vector_back(const Vector *vec, void *out)
{
	if(vec->n)
		memcpy(out, &vec->data[vec->n - 1], vec->sz);
}

const u8* vector_get_data(const Vector *vec)
{
	return vec->data;
}

bool vector_empty(const Vector *vec)
{
	return vec->n == 0;
}

u32 vector_size(const Vector *vec)
{
	return vec->n;
}

usize vector_struct_size(const Vector *vec)
{
	return vec->sz;
}

usize vector_capacity(const Vector *vec)
{
	return vec->vectorSize;
}

void vector_reserve(Vector *vec, u32 reserveSize)
{
	if(reserveSize > vec->vectorSize){
		vec->vectorSize = reserveSize;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
	}
}

void vector_push_back(Vector *vec, const void *data)
{
	/* LOGF_TRACE("Vector (BEFORE) = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data); */
	memcpy(&vec->data[vec->n++], data, vec->sz);

	if(vec->n == vec->vectorSize){
		vec->vectorSize *= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
	}
	/* LOGF_TRACE("Vector (AFTER) = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data); */
}

void vector_insert_at(Vector *vec, const usize index, const void *data)
{
	memmove(&vec->data[index + vec->sz], &vec->data[index], (vec->n++) - index);
	memcpy(&vec->data[index], data, vec->sz);

	if(vec->n == vec->vectorSize){
		vec->vectorSize *= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
	}
}

void vector_erase_at(Vector *vec, const usize index)
{
	if(vec->n)
		--vec->n;
	if(index < vec->n)
		memmove(&vec->data[index], &vec->data[index + vec->sz], vec->n - index);

	if(vec->n < vec->vectorSize / 4){
		vec->vectorSize /= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
	}
}

void vector_pop_back(Vector *vec)
{
	if(vec->n)
		--vec->n;

	if(vec->n < vec->vectorSize / 4){
		vec->vectorSize /= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
	}
}

void vector_clear(Vector *vec)
{
	vec->n = 0;
	vec->vectorSize = initialSize;
	free(vec->data);
	vec->data = malloc(sizeof(u8[initialSize]) * vec->sz);
}

void vector_sort(Vector *vec, int (*compar)(const void*, const void*))
{
	qsort(vec->data, vec->n, vec->sz, compar);
}

