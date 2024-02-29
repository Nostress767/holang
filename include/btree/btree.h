#pragma once

#include "types.h"

#ifndef BTREE_INTERNAL
typedef Handle TREE;
#endif

#undef DLL_FUNCTIONS
#define DLL_FUNCTIONS \
DLL_X(btree_init, TREE*, usize, u32, int (*compar)(const void *, const void *))\
DLL_X(btree_uinit, void, TREE*)\
\
DLL_X(btree_insert, void, void*, TREE*)\
\
DLL_X(btree_erase, void, void*, TREE*)\
\
DLL_X(btree_print, void, TREE*)\
\
DLL_X(btree_search, void*, void*, TREE*)

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(btree)
#include "dll/plugger.h"