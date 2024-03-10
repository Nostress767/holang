#pragma once

#include <string.h>

#include "vector.h"

INTERNAL const void* _vector_set_element(Vector vec[restrict static 1], const void *data, usize pos)
{
	return vec->customAllocator ?
		vec->customAllocator(data, &vec->data[pos])
		: memcpy(&vec->data[pos], data, vector_element_sz(vec));
}

