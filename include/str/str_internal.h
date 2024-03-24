#pragma once

#include <string.h>

#include "str.h"

INTERNAL usize _usize_bit_ceil(const usize value)
{
	usize i = 1;
	while(i < value)
		i <<= 1;
	return i;
}

