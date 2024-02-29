#pragma once

#include "types.h"

#ifndef BTREE_INTERNAL
typedef Handle TREE;
#endif

#undef DLL_FUNCTIONS
#define DLL_FUNCTIONS \
DLL_X(btree_init, TREE*, usize sz, u32 order, int (*compar)(const void *, const void *))\
DLL_X(btree_uninit, void, TREE *bt)\
\
DLL_X(btree_insert, void, TREE bt[restrict static 1], const void *key)\
\
DLL_X(btree_erase, void, TREE bt[restrict static 1], const void *key)\
\
DLL_X(btree_print, void, TREE bt[const static 1])\
\
DLL_X(btree_search, bool, TREE bt[restrict const static 1], const void *key, void *out)

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(btree)
#include "dll/plugger.h"