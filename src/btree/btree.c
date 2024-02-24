#define DLL_HEADER_SOURCE
#include "btree/btree_internal.h"
#undef DLL_HEADER_SOURCE

#undef DEBUG_X
#define DEBUG_X(fName, ret, ...) fName,
DEBUG_DEFINE_VTABLE(btree)

#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
void btree_print (TREE *bt)
{
	NODE *node = bt->root;
	while (node->index)
		node = node->child[0];
	while (node) {
		printf ("qttKeys: %d\n", node->n);
		for (int i = 0; i < node->n; i++)
			printf ("%d ", *(int*)((char*)node->data+i*bt->sz));
		printf ("\n");
		node = node->next;
	}
}
*/

TREE *btree_init (usize sz, u32 order, int (*compar)(const void*, const void*))
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

void btree_uinit (TREE *bt)
{
	_btree_uinit (bt->root);
	free (bt->auxData);
	free (bt);
}

void btree_insert(void *key, TREE *bt)
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





void _btree_uinit (NODE *node)
{
	if (node->index) {
		for (u32 i = 0; i <= node->n; i++)
			_btree_uinit (node->child[i]);
		free (node->child);
	} else
		free (node->next);
	free (node->data);
}

bool _btree_insert (void *key, NODE *node, TREE *bt)
{
	uint16_t pos = _btree_bsearch (key, node, bt);
	
	if (node->index) {
		if (!_btree_insert (key, node->child[pos], bt)) {
			if (node->n == bt->order-1) {
				_btree_split (node, bt, pos);
				return false;
			} else {
				if (node->n != pos) {
					memmove (node->data + (pos + 1) * bt->sz, node->data + pos * bt->sz, ((node->n - pos) * bt->sz));
					memmove (&node->child[pos+2], &node->child[pos+1], (node->n - pos + 1) * sizeof (NODE *));
				}
				memcpy ((node->data + pos * bt->sz), bt->auxData, bt->sz);
				node->child[pos+1] = bt->auxNode;
				node->n++;
			}
		}
	} else {
		if (node->n == bt->order-1) {
			_load_btree_aux (bt, key, NULL);
			_btree_split (node, bt, pos);
			return false;
		} else {
			if (node->n != pos) 
				memmove (node->data + (pos + 1) * bt->sz, node->data + pos * bt->sz, ((node->n - pos) * bt->sz));
			memcpy ((node->data + pos * bt->sz), key, bt->sz);
			node->n++;
		}
	}
	
	return true;
}

void _btree_split (NODE *node, TREE *bt, u32 pos)
{
	NODE *new = _malloc_node (bt, node->index);
	bt->n++;

	node->n = bt->order/2;
	new->n = bt->order - node->n;
	
	if (pos < bt->order/2) {
		memcpy (new->data, node->data + (node->n-1) * bt->sz, (bt->order-node->n) * bt->sz);

		memmove (node->data + (pos + 1) * bt->sz, node->data + pos * bt->sz, (node->n - pos) * bt->sz);
		memcpy (node->data + pos * bt->sz, bt->auxData, bt->sz);
		
		if (node->index) {
			memcpy (new->child, node->child + node->n, (bt->order - node->n) * sizeof (NODE*));
			memmove (node->child + pos + 2, node->child + pos + 1, (node->n - pos - 1) * sizeof (NODE*));
			node->child[pos+1] = bt->auxNode;
		}
		
	} else {
		memcpy (new->data, node->data + (node->n) * bt->sz, (bt->order-node->n-1) * bt->sz);
		
		pos -= node->n;

		memmove (new->data + (pos + 1) * bt->sz, new->data + pos * bt->sz, (new->n - pos) * bt->sz);
		memcpy (new->data + pos * bt->sz, bt->auxData, bt->sz);

		if (node->index) {
			memcpy (new->child, node->child + new->n + !(bt->order%2), (bt->order - new->n - !(bt->order%2)) * sizeof (NODE*));
			memmove (new->child + pos + 1, new->child + pos, (new->n - pos) * sizeof (NODE*));
			new->child[pos] = bt->auxNode;
		}
		
	}

	
	memset (node->data + node->n * bt->sz, 0, (bt->order - 1 - node->n) * bt->sz);
	_load_btree_aux (bt, new->data, new);
	
	if (!node->index) {
		new->next = node->next;
		node->next = new;
	} else {
		new->n--;
		memmove (new->data, new->data + bt->sz, new->n * bt->sz);
		memset (new->data + new->n * bt->sz, 0, new->n * bt->sz);

		memset (node->child + (node->n+1), 0, (bt->order - node->n - 1) * sizeof (NODE*));
		memset (new->child + (new->n+1), 0, (bt->order - new->n - 1) * sizeof (NODE*));
	}
}

NODE *_malloc_node (TREE *bt, bool index)
{
	
	NODE *out = malloc(sizeof(NODE));
	out->n = 0;
	out->data = malloc((bt->order-1) * bt->sz);
	memset (out->data, 0, (bt->order-1) * bt->sz);
	
	if (index) {
		out->child = malloc (sizeof(NODE*) * bt->order);
		memset (out->child, 0, sizeof(NODE*) * bt->order);
	} else {
		out->next = NULL;
	}

	out->index = index;
	
	return out;
}

void _load_btree_aux (TREE *bt, void *key, NODE *node)
{
	if (!key && !node) {
		memset (bt->auxData, 0, bt->sz);
		bt->auxNode = NULL;
		return;
	}
	
	memset (bt->auxData, 0, bt->sz);
	memcpy (bt->auxData, key, bt->sz);
	bt->auxNode = node;
}

u32 _btree_bsearch (void *key, NODE *node, TREE *bt)
{
	
	u32 pos = 0;
	for (; pos < node ->n && bt->compar (key, node->data + pos*bt->sz) > 0; pos++);
	
	//uint64_t pos = b_search (key, node->data, node->n, bt->sz, bt->compar);
	//if ((pos == node->n-1 || pos == 0) && bt->compar (key, (char*)node->data+pos*bt->sz) > 0)
	//	pos++;
	return pos;
}