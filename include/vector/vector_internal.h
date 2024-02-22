#pragma once

#include "types.h"

typedef struct Vector
{
	usize vectorSize; /* Internal vector size */
	usize sz;         /* Size of element      */
	u32 n;            /* Amount of elements   */
	u8 *data;         /* Raw data of elements */
} Vector;

#define VECTOR_INTERNAL
#include "vector.h"

