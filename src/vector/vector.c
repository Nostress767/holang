#define DLL_HEADER_SOURCE
#include "vector/vector_internal.h"
DEBUG_DEFINE_VTABLE(vector)
#undef DLL_HEADER_SOURCE

#include <stdlib.h>
#include <string.h>

Vector* vector_init(const usize sz)
{
	return vector_init_with_allocator(sz, nullptr);
}

Vector* vector_init_with_allocator(const usize sz, const void* (*customAllocator)(const void *el, u8 *vecData))
{
	Vector *vec = malloc(sizeof *vec);
	if(!vec)
		return nullptr;

	vec->data = malloc(vectorInitialSize * sz);
	if(!vec->data){
		free(vec);
		return nullptr;
	}

	vec->sz = sz;
	vec->vectorSize = vectorInitialSize;
	vec->n = 0;
	vec->lastError = vectorErrorSuccess;
	vec->customAllocator = customAllocator;

	return vec;
}

void vector_uninit(Vector *vec)
{
	if(vec){
		if(vec->customAllocator){
			for(u32 i = 0; i < vector_size(vec); ++i){
				vec->customAllocator(nullptr, &vec->data[i * vector_element_sz(vec)]);
			}
		}
		free(vec->data);
		free(vec);
	}
}

Vector* vector_clone(Vector vec[static 1])
{
	Vector *newVec = malloc(sizeof *newVec);
	if(!newVec){
		vec->lastError = vectorErrorOutOfMemory;
		return nullptr;
	}

	void *tmp = malloc(vector_capacity(vec) * vector_element_sz(vec));
	if(!tmp){
		free(newVec);
		vec->lastError = vectorErrorOutOfMemory;
		return nullptr;
	}

	if(vec->customAllocator){
		for(u32 i = 0; i < vector_size(vec); ++i){
			const usize pos = i * vector_element_sz(vec);
			const void *newData = vec->customAllocator(&vec->data[pos], &((u8*)tmp)[pos]);
			if(!newData){
				for(u32 j = 0; j < i; ++j){
					vec->customAllocator(nullptr, &((u8*)tmp)[j * vector_element_sz(vec)]);
				}
				free(newVec);
				free(tmp);
				vec->lastError = vectorErrorOutOfMemory;
				return nullptr;
			}
		}
	}
	else{
		memcpy(tmp, vec->data, vector_size(vec) * vector_element_sz(vec));
	}

	newVec->vectorSize = vector_capacity(vec);
	newVec->sz = vector_element_sz(vec);
	newVec->n = vector_size(vec);
	newVec->data = tmp;
	newVec->customAllocator = vec->customAllocator;

	vec->lastError = vectorErrorSuccess;
	return newVec;
}

void vector_swap(Vector vecA[restrict static 1], Vector vecB[restrict static 1])
{
	Vector tmp = *vecA;
	*vecA = *vecB;
	*vecB = tmp;
}

void* vector_at(Vector vec[restrict const static 1], const usize index)
{
	if(index >= vector_size(vec)){
		vec->lastError = vectorErrorOutOfBounds;
		return nullptr;
	}

	vec->lastError = vectorErrorSuccess;
	return &vec->data[index * vector_element_sz(vec)];
}

void* vector_front(Vector vec[restrict const static 1])
{
	if(vector_empty(vec)){
		vec->lastError = vectorErrorEmpty;
		return nullptr;
	}

	vec->lastError = vectorErrorSuccess;
	return &vec->data[0];
}

void* vector_back(Vector vec[restrict const static 1])
{
	if(vector_empty(vec)){
		vec->lastError = vectorErrorEmpty;
		return nullptr;
	}

	vec->lastError = vectorErrorSuccess;
	return &vec->data[(vector_size(vec) - 1) * vector_element_sz(vec)];
}

void vector_reserve(Vector vec[static 1], u32 reserveAmount)
{
	if(reserveAmount > vector_capacity(vec)){
		void *tmp = realloc(vec->data, reserveAmount * vector_element_sz(vec));
		if(!tmp){
			vec->lastError = vectorErrorOutOfMemory;
			return;
		}
		vec->vectorSize = reserveAmount;
		vec->data = tmp;
	}

	vec->lastError = vectorErrorSuccess;
}

void vector_push_back(Vector vec[static 1], const void *data)
{
	if(!data){
		vec->lastError = vectorErrorNullInputData;
		return;
	}

	if(vector_size(vec) == vector_capacity(vec)){
		void *tmp = realloc(vec->data, 2 * vector_capacity(vec) * vector_element_sz(vec));
		if(!tmp){
			vec->lastError = vectorErrorOutOfMemory;
			return;
		}
		vec->vectorSize *= 2;
		vec->data = tmp;
	}

	if(!_vector_set_element(vec, data, vector_size(vec) * vector_element_sz(vec))){
		vec->lastError = vectorErrorOutOfMemory; /* Only when using customAllocator */
		return;
	}

	vec->n++;

	vec->lastError = vectorErrorSuccess;
}

void vector_assign_at(Vector vec[restrict static 1], const usize index, const void *data)
{
	if(!data){
		vec->lastError = vectorErrorNullInputData;
		return;
	}

	if(index >= vector_capacity(vec)){
		vec->lastError = vectorErrorOutOfBounds;
		return;
	}

	const usize pos = index * vector_element_sz(vec);

	if(index > vector_size(vec)){
		/* NOTE: This can cause problems with customAllocator */
		/* The user will need to check even successful vector_at for nullptr */
		memset(&vec->data[vector_size(vec) * vector_element_sz(vec)], 0, (index - vector_size(vec)) * vector_element_sz(vec));
		vec->n = index + 1;
	}
	else if(index == vector_size(vec))
		++vec->n;
	else if(vec->customAllocator){
		/* We could be replacing another element */
		/* But we need to make sure first */
		void *exists;
		memcpy(&exists, &vec->data[pos], vector_element_sz(vec));

		if(exists){ /* We need to free this before inserting the new values */
			/* NOTE: if you wanted to replace without destroying and constructing everything again, */
			/* just use vector_at and modify the pointer in place */
			vec->customAllocator(nullptr, &vec->data[pos]);
		}
	}

	if(!_vector_set_element(vec, data, pos)){
		vec->lastError = vectorErrorOutOfMemory; /* Only when using customAllocator */
		return;
	}

	vec->lastError = vectorErrorSuccess;
}

void vector_insert_at(Vector vec[restrict static 1], const usize index, const void *data)
{
	if(!data){
		vec->lastError = vectorErrorNullInputData;
		return;
	}
	if(index > vector_size(vec)){
		vec->lastError = vectorErrorOutOfBounds;
		return;
	}
	if(vector_size(vec) == vector_capacity(vec)){
		void *tmp = realloc(vec->data, 2 * vector_capacity(vec) * vector_element_sz(vec));
		if(!tmp){
			vec->lastError = vectorErrorOutOfMemory;
			return;
		}
		vec->vectorSize *= 2;
		vec->data = tmp;
	}

	const usize pos = index * vector_element_sz(vec);

	memmove(&vec->data[pos + vector_element_sz(vec)], &vec->data[pos], (vector_size(vec) - index) * vector_element_sz(vec));
	vec->n++;

	if(!_vector_set_element(vec, data, pos)){
		/* NOTE: the cost of moving back this data can be avoided if necessary */
		/* Just save as a temporary value, and then move into position */
		memmove(&vec->data[pos], &vec->data[pos + vector_element_sz(vec)], (vector_size(vec) - index) * vector_element_sz(vec));
		vec->n--;
		vec->lastError = vectorErrorOutOfMemory; /* Only when using customAllocator */
		return;
	}

	vec->lastError = vectorErrorSuccess;
}

void vector_erase_at(Vector vec[static 1], const usize index)
{
	if(vector_empty(vec)){
		vec->lastError = vectorErrorEmpty;
		return;
	}

	if(index >= vector_size(vec)){
		vec->lastError = vectorErrorOutOfBounds;
		return;
	}

	--vec->n; /* Prepare removal */

	if(vector_capacity(vec) > vectorInitialSize && vector_size(vec) < vector_capacity(vec) / 4){
		void *tmp = realloc(vec->data, (vector_capacity(vec) / 2) * vector_element_sz(vec));
		if(!tmp){
			++vec->n; /* Cancel removal */
			vec->lastError = vectorErrorOutOfMemory; /* This seems ridiculous, but apparently can happen (https://stackoverflow.com/questions/12125308/can-realloc-fail-return-null-when-trimming#:~:text=Yes%2C%20it%20can.&text=For%20example%2C%20if%20a%20particular,will%20fail%20and%20return%20NULL%20.) */
			return;
		}
		vec->vectorSize /= 2;
		vec->data = tmp;
	}

	const usize pos = index * vector_element_sz(vec);

	if(vec->customAllocator)
		vec->customAllocator(nullptr, &vec->data[pos]);

	memmove(&vec->data[pos], &vec->data[pos + vector_element_sz(vec)], (vector_size(vec) - index) * vector_element_sz(vec)); /* Remove */

	vec->lastError = vectorErrorSuccess;
}

void vector_pop_back(Vector vec[static 1])
{
	if(vector_empty(vec)){
		vec->lastError = vectorErrorEmpty;
		return;
	}

	--vec->n;

	if(vec->customAllocator)
		vec->customAllocator(nullptr, &vec->data[vector_size(vec) * vector_element_sz(vec)]);

	if(vector_capacity(vec) > vectorInitialSize && vector_size(vec) < vector_capacity(vec) / 4){
		void *tmp = realloc(vec->data, (vector_capacity(vec) / 2) * vector_element_sz(vec));
		if(!tmp){
			++vec->n;
			vec->lastError = vectorErrorOutOfMemory; /* Same reason as vector_erase_at */
			return;
		}
		vec->vectorSize /= 2;
		vec->data = tmp;
	}

	vec->lastError = vectorErrorSuccess;
}

void vector_clear(Vector vec[static 1])
{
	void *tmp = malloc(vectorInitialSize * vector_element_sz(vec));
	if(!tmp){
		vec->lastError = vectorErrorOutOfMemory;
		return;
	}

	if(vec->customAllocator)
		for(u32 i = 0; i < vector_size(vec); ++i)
			vec->customAllocator(nullptr, &vec->data[i * vector_element_sz(vec)]);

	free(vec->data);
	vec->data = tmp;
	vec->n = 0;
	vec->vectorSize = vectorInitialSize;
	vec->lastError = vectorErrorSuccess;
}

void vector_sort(Vector vec[static 1], int (*compar)(const void*, const void*))
{
	qsort(vec->data, vector_size(vec), vector_element_sz(vec), compar);
}

