#define DLL_HEADER_SOURCE
#include "btree/btree_internal.h"
DEBUG_DEFINE_VTABLE(btree)
#undef DLL_HEADER_SOURCE

#include "log.h"

#include <stdio.h>

bool btree_search (TREE bt[restrict const static 1], const void *key, void *out)
{
	if (!bt->root)
		return false;
	if (out) {
		void *tmp = _btree_search (key, bt->root, bt);
		if(!tmp)
			return false;
		memcpy(out, tmp, bt->sz);
	}
	return true;
}

void btree_insert (TREE bt[restrict static 1], const void *key)
{
	if (bt->root == NULL) {
		
		NODE *new = _malloc_node (bt, false);
		
		new->n = 1;
		memcpy (new->data, key, bt->sz);
		
		bt->n++;
		bt->root = new;
		
		return;
	}
	
	if (!_btree_insert (key, bt->root, bt)) {
		NODE *new = _malloc_node (bt, true);
		bt->n++;
		
		new->child[0] = bt->root;
		new->child[1] = bt->auxNode;
		memcpy (new->data, bt->auxData, bt->sz);
		new->n = 1;
		
		bt->root = new;
	}
}

TREE *btree_init (const usize sz, const u32 order, int (*compar)(const void*, const void*))
{
	LOGF_TRACE("BTree = { .root = %p, .order = %u, .n = %u, .sz = %zu }", NULL, order, 0, sz);
	TREE *bt = malloc (sizeof (TREE));
	
	bt->root = NULL;
	bt->order = order;
	bt->n = 0;
	bt->sz = sz;
	bt->compar = compar;
	
	
	bt->auxData = malloc (sz);
	bt->auxNode = NULL;
	
	return bt;
}

void btree_erase (TREE bt[restrict static 1], const void *key)
{
	if (!bt->root)
		return;
	_btree_erase (key, bt->root, bt);
}

void btree_uninit (TREE *bt)
{
	if (bt->root)
		_btree_uinit (bt->root);
	free (bt->auxData);
	free (bt);
}

void btree_print (TREE bt[const static 1])
{
	if (!bt->root)
		return;
	_btree_print (bt->root, bt);
}
