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

Vector* vector_clone(Vector vec[static 1])
{
	Vector *clonedVec = vector_init(vec->sz);
	if(!clonedVec)
		return nullptr;

	VectorError e = vector_reserve(clonedVec, vec->n);
	if(e != vectorErrorSuccess){
		vector_uninit(clonedVec);
		return nullptr;
	}

	clonedVec->n = vec->n;
	memcpy(clonedVec->data, vec->data, vec->n * vec->sz);

	return clonedVec;
}

void vector_swap(Vector vecA[restrict static 1], Vector vecB[restrict static 1])
{
	Vector tmp = *vecA;
	*vecA = *vecB;
	*vecB = tmp;
}

VectorError vector_at(Vector vec[restrict const static 1], const usize index, void *out)
{
	if(!out)
		return vectorErrorNullOutputData;
	/* LOGF_TRACE("index = %zu", index); */
	if(index >= vec->n)
		return vectorErrorOutOfBounds;

	memcpy(out, &vec->data[index * vec->sz], vec->sz);
	return vectorErrorSuccess;
}

VectorError vector_front(Vector vec[restrict const static 1], void *out)
{
	if(!out)
		return vectorErrorNullOutputData;
	if(vec->n){
		memcpy(out, &vec->data[0], vec->sz);
		return vectorErrorSuccess;
	}
	return vectorErrorEmpty;
}

VectorError vector_back(Vector vec[restrict const static 1], void *out)
{
	if(!out)
		return vectorErrorNullOutputData;
	if(vec->n){
		memcpy(out, &vec->data[(vec->n - 1) * vec->sz], vec->sz);
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
		void *tmp = realloc(vec->data, sizeof(u8[reserveSize]) * vec->sz);
		if(!tmp)
			return vectorErrorOutOfMemory;
		vec->vectorSize = reserveSize;
		vec->data = tmp;
	}

	return vectorErrorSuccess;
}

VectorError vector_push_back(Vector vec[static 1], const void *data)
{
	if(!data)
		return vectorErrorNullInputData;
	/* LOGF_TRACE("Vector (BEFORE) = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data); */
	if(vec->n == vec->vectorSize){
		void *tmp = realloc(vec->data, sizeof(u8[vec->vectorSize * 2]) * vec->sz);
		if(!tmp)
			return vectorErrorOutOfMemory;
		vec->vectorSize *= 2;
		vec->data = tmp;
	}
	memcpy(&vec->data[(vec->n++) * vec->sz], data, vec->sz);

	/* LOGF_TRACE("Vector (AFTER) = { .vectorSize = %zu, .sz = %zu, .n = %u, .data = %p }", vec->vectorSize, vec->sz, vec->n, vec->data); */
	return vectorErrorSuccess;
}

VectorError vector_assign_at(Vector vec[restrict static 1], const usize index, const void *data)
{
	if(!data)
		return vectorErrorNullInputData;
	if(index >= vec->vectorSize)
		return vectorErrorOutOfBounds;

	if(index > vec->n){
		memset(&vec->data[vec->n * vec->sz], 0, (index - vec->n) * vec->sz);
		vec->n = index + 1;
	}
	else if(index == vec->n)
		++vec->n;

	memcpy(&vec->data[index * vec->sz], data, vec->sz);

	return vectorErrorSuccess;
}

VectorError vector_insert_at(Vector vec[restrict static 1], const usize index, const void *data)
{
	if(!data)
		return vectorErrorNullInputData;
	if(index > vec->n)
		return vectorErrorOutOfBounds;
	if(vec->n == vec->vectorSize){
		void *tmp = realloc(vec->data, sizeof(u8[vec->vectorSize * 2]) * vec->sz);
		if(!tmp)
			return vectorErrorOutOfMemory;
		vec->vectorSize *= 2;
		vec->data = tmp;
	}

	memmove(&vec->data[index * vec->sz + vec->sz], &vec->data[index * vec->sz], (vec->n++) - index);
	memcpy(&vec->data[index * vec->sz], data, vec->sz);

	return vectorErrorSuccess;
}

VectorError vector_erase_at(Vector vec[static 1], const usize index)
{
	if(vec->n == 0)
		return vectorErrorEmpty;

	if(index >= vec->n)
		return vectorErrorOutOfBounds;

	--vec->n; /* Prepare removal */

	if(vec->vectorSize > vectorInitialSize && vec->n < vec->vectorSize / 4){
		void *tmp = realloc(vec->data, (sizeof(u8[vec->vectorSize]) / 2) * vec->sz);
		if(!tmp){
			++vec->n; /* Cancel removal */
			return vectorErrorOutOfMemory; /* This seems ridiculous, but apparently can happen (https://stackoverflow.com/questions/12125308/can-realloc-fail-return-null-when-trimming#:~:text=Yes%2C%20it%20can.&text=For%20example%2C%20if%20a%20particular,will%20fail%20and%20return%20NULL%20.) */
		}
		vec->vectorSize /= 2;
		vec->data = tmp;
	}

	memmove(&vec->data[index * vec->sz], &vec->data[index * vec->sz + vec->sz], (vec->n - index) * vec->sz); /* Remove */

	return vectorErrorSuccess;
}

VectorError vector_pop_back(Vector vec[static 1])
{
	if(vec->n == 0)
		return vectorErrorEmpty;

	--vec->n;

	if(vec->n > vectorInitialSize && vec->n < vec->vectorSize / 4){
		void *tmp = realloc(vec->data, (sizeof(u8[vec->vectorSize]) / 2) * vec->sz);
		if(!tmp){
			++vec->n;
			return vectorErrorOutOfMemory; /* Same reason as vector_erase_at */
		}
		vec->vectorSize /= 2;
		vec->data = tmp;
	}

	return vectorErrorSuccess;
}

VectorError vector_clear(Vector vec[static 1])
{
	void *tmp = malloc(sizeof(u8[vectorInitialSize]) * vec->sz);
	if(!tmp)
		return vectorErrorOutOfMemory;
	free(vec->data);
	vec->data = tmp;
	vec->n = 0;
	vec->vectorSize = vectorInitialSize;
	return vectorErrorSuccess;
}

void vector_zero_all(Vector vec[static 1])
{
	vec->n = 0;
	memset(vec->data, 0, sizeof(u8[vec->vectorSize]) * vec->sz);
}

void vector_sort(Vector vec[static 1], int (*compar)(const void*, const void*))
{
	qsort(vec->data, vec->n, vec->sz, compar);
}

