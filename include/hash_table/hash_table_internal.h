#pragma once

#include <stdlib.h>

#include "hash_table.h"

INTERNAL u32 rotl(u32 x, u32 b)
{
	return (u32)(((x) << (b)) | ((x) >> (32 - (b))));
}

INTERNAL void u32_to_8_le(u8 p[static 4], u32 v)
{
	p[0] = (u8)((v));
    	p[1] = (u8)((v) >> 8);
    	p[2] = (u8)((v) >> 16);
    	p[3] = (u8)((v) >> 24);
}

INTERNAL u32 u8_to_32_le(const u8 p[static 4])
{
	return (((u32)(p[0])) | ((u32)(p[1]) << 8) | ((u32)(p[2]) << 16) | ((u32)(p[3]) << 24));
}

INTERNAL void sipround(u32 v[4])
{
        v[0] += v[1];
        v[1] = rotl(v[1], 5);
        v[1] ^= v[0];
        v[0] = rotl(v[0], 16);
        v[2] += v[3];
        v[3] = rotl(v[3], 8);
        v[3] ^= v[2];
        v[0] += v[3];
        v[3] = rotl(v[3], 7);
        v[3] ^= v[0];
        v[2] += v[1];
        v[1] = rotl(v[1], 13);
        v[1] ^= v[2];
        v[2] = rotl(v[2], 16);
}

INTERNAL usize _usize_bit_ceil(usize value)
{
	usize i = 1;
	while(i < value)
		i <<= 1;
	return i;
}

