#pragma once

#include "types.h"

typedef struct NODE
{
	u32 n;			     /* Quantity of elements on the node 						       */
	union {
		struct NODE **child; /* If the node is used for the index, it has child 				       */
		struct NODE *next;   /* If the node is used for the linked list, it has next 				       */
	};
	bool index;		     /* The node can be used for the index or for the linked list, it depends on this variable */
	u8 *data;		     /* Raw data of elements 								       */
} NODE;

typedef struct TREE
{
	NODE *root;				     /* Pointer to the root 			       */
	u32 order;				     /* Order of the btree 			       */
	u32 n;					     /* Quantity of nodes 			       */
	usize sz;				     /* Size of data 				       */
	int (*compar)(const void *a, const void *b); /* Comparation function used to sort the elements */
	void *auxData;				     /* Used for internal operations 		       */
	NODE *auxNode;				     /* Used for internal operations 		       */
} TREE;

enum BTreeError : u8
{
	bTreeErrorOutOfMemory,
	bTreeErrorSuccess,
	/* TODO */
};

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
DLL_X(btree_search, void*, TREE bt[restrict const static 1], const void *key)

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(btree)
#include "dll/plugger.h"