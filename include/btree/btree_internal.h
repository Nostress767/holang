#pragma once

#include "types.h"

typedef struct NODE
{
	u32 n;	/* Quantity of elements on the node */
	union {
		struct NODE **child;	/*If the node is used for the index, it has child */
		struct NODE *next;	/*If the node is used for the linked list, it has next */
	};
	bool index;	/* The node can be used for the index or for the linked list, it depends on this variable */
	u8 *data;	/* Raw data of elements */
} NODE;

typedef struct TREE
{
	NODE *root;	/* Pointer to the root */
	u32 order;		/* Order of the btree */
	u32 n;	/* Quantity of nodes */
	usize sz;	/* Size of data */
	int (*compar)(const void *a, const void *b);	/* Comparation function used to sort the elements */
	void *auxData;	/* Used for internal operations */
	NODE *auxNode;	/* Used for internal operations */
} TREE;

i64 _btree_bsearch (void *key, NODE *node, TREE *bt);
bool _btree_insert (void *key, NODE *node, TREE *bt);
NODE *_malloc_node (TREE *bt, bool index);
void _load_btree_aux (TREE *bt, void *key, NODE *node);
void _btree_split (NODE *node, TREE *bt, u32 pos);
void _btree_uinit (NODE *node);
bool _btree_erase (void *key, NODE *node, TREE *bt);
bool _btree_borrow (NODE *node, TREE *bt, u32 pos);
bool _btree_merge (NODE *node, TREE *bt, i64 pos);
void _print_node (NODE *node);
void *_btree_search (void *key, NODE *node, TREE *bt);
void _btree_print (NODE *node, TREE *bt);
void _btree_move_foward (u8 *data, i64 pos, u32 n, usize sz);
void _btree_move_backwards (u8 *data, i64 pos, u32 n, usize sz);
void _btree_clear_space (u8 *data, u32 maxS, u32 n, usize sz);

#define BTREE_INTERNAL
#include "btree.h"