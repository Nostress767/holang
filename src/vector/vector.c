#define DLL_HEADER_SOURCE
#include "vector/vector_internal.h"
DEBUG_DEFINE_VTABLE(vector)
#undef DLL_HEADER_SOURCE

#include "log.h"

#include <stdlib.h>
#include <string.h>

Vector* vector_init(const usize sz)
{
	LOGF_TRACE("sz = %zu", sz);
	Vector *vec = malloc(sizeof *vec);
	if(!vec)
		return nullptr;
	vec->data = malloc(sizeof(u8[vectorInitialSize]) * sz);
	if(!vec->data){
		free(vec);
		return nullptr;
	}
	vec->sz = sz;
	vec->vectorSize = vectorInitialSize;
	vec->n = 0;
	return vec;
}

void vector_uninit(Vector *vec)
{
	LOGF_TRACE("Vector = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data);
	free(vec->data);
	free(vec);
}

void vector_swap(Vector vecA[restrict static 1], Vector vecB[restrict static 1])
{
	Vector tmp = *vecA;
	*vecA = *vecB;
	*vecB = tmp;
}

VectorError vector_at(Vector vec[restrict const static 1], const usize index, void *out)
{
	/* LOGF_TRACE("index = %zu", index); */
	if(index < vec->n){
		memcpy(out, &vec->data[index], vec->sz);
		return vectorErrorSuccess;
	}
	return vectorErrorOutOfBounds;
}

VectorError vector_front(Vector vec[restrict const static 1], void *out)
{
	if(vec->n){
		memcpy(out, &vec->data[0], vec->sz);
		return vectorErrorSuccess;
	}
	return vectorErrorEmpty;
}

VectorError vector_back(Vector vec[restrict const static 1], void *out)
{
	if(vec->n){
		memcpy(out, &vec->data[vec->n - 1], vec->sz);
		return vectorErrorSuccess;
	}
	return vectorErrorEmpty;
}

const u8* vector_get_data(Vector vec[const static 1])
{
	return vec->data;
}

bool vector_empty(Vector vec[const static 1])
{
	return vec->n == 0;
}

u32 vector_size(Vector vec[const static 1])
{
	return vec->n;
}

usize vector_struct_size(Vector vec[const static 1])
{
	return vec->sz;
}

usize vector_capacity(Vector vec[const static 1])
{
	return vec->vectorSize;
}

VectorError vector_reserve(Vector vec[static 1], u32 reserveSize)
{
	if(reserveSize > vec->vectorSize){
		vec->vectorSize = reserveSize;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
		if(!vec->data)
			return vectorErrorOutOfMemory;
	}

	return vectorErrorSuccess;
}

VectorError vector_push_back(Vector vec[static 1], const void *data)
{
	/* LOGF_TRACE("Vector (BEFORE) = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data); */
	memcpy(&vec->data[vec->n++], data, vec->sz);

	if(vec->n == vec->vectorSize){
		vec->vectorSize *= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
		if(!vec->data)
			return vectorErrorOutOfMemory;
	}
	/* LOGF_TRACE("Vector (AFTER) = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data); */
	return vectorErrorSuccess;
}

VectorError vector_insert_at(Vector vec[restrict static 1], const usize index, const void *data)
{
	if(index >= vec->n)
		return vectorErrorOutOfBounds;

	memmove(&vec->data[index + vec->sz], &vec->data[index], (vec->n++) - index);
	memcpy(&vec->data[index], data, vec->sz);

	if(vec->n == vec->vectorSize){
		vec->vectorSize *= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
		if(!vec->data)
			return vectorErrorOutOfMemory;
	}
	return vectorErrorSuccess;
}

VectorError vector_erase_at(Vector vec[static 1], const usize index)
{
	if(vec->n == 0)
		return vectorErrorEmpty;

	--vec->n;

	if(index >= vec->n)
		return vectorErrorOutOfBounds;

	memmove(&vec->data[index], &vec->data[index + vec->sz], vec->n - index);

	if(vec->n < vec->vectorSize / 4){
		vec->vectorSize /= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
		if(!vec->data)
			return vectorErrorOutOfMemory; /* This seems ridiculous, but apparently can happen (https://stackoverflow.com/questions/12125308/can-realloc-fail-return-null-when-trimming#:~:text=Yes%2C%20it%20can.&text=For%20example%2C%20if%20a%20particular,will%20fail%20and%20return%20NULL%20.) */
	}
	return vectorErrorSuccess;
}

VectorError vector_pop_back(Vector vec[static 1])
{
	if(vec->n == 0)
		return vectorErrorEmpty;

	--vec->n;

	if(vec->n < vec->vectorSize / 4){
		vec->vectorSize /= 2;
		vec->data = realloc(vec->data, sizeof(u8[vec->vectorSize]) * vec->sz);
		if(!vec->data)
			return vectorErrorOutOfMemory; /* Same reason as vector_erase_at */
	}
	return vectorErrorSuccess;
}

VectorError vector_clear(Vector vec[static 1])
{
	vec->n = 0;
	vec->vectorSize = vectorInitialSize;
	free(vec->data);
	vec->data = malloc(sizeof(u8[vectorInitialSize]) * vec->sz);
	if(!vec->data)
		return vectorErrorOutOfMemory;
	return vectorErrorSuccess;
}

void vector_sort(Vector vec[static 1], int (*compar)(const void*, const void*))
{
	qsort(vec->data, vec->n, vec->sz, compar);
}

