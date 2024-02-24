#pragma once

#include "dll_header.h"
#include "types.h"

#ifndef BTREE_INTERNAL
typedef Handle TREE;
#endif

#undef FUNCTIONS
#define FUNCTIONS \
DEBUG_X(btree_init, TREE*, usize sz, u32 order, int (*compar)(const void *key, const void *elem))\
DEBUG_X(btree_uinit, void, TREE *bt)\
\
DEBUG_X(btree_insert, void, void *key, TREE *bt)\

#include "common_dll.h"

DEBUG_DECLARE_VTABLE(btree)