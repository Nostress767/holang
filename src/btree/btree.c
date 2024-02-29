#define DLL_HEADER_SOURCE
#include "btree/btree_internal.h"
DEBUG_DEFINE_VTABLE(btree)
#undef DLL_HEADER_SOURCE

#include "log.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define btree_min(bt) (bt->order+1)/2-1
#define btree_max(bt) (bt->order-1)

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

void *btree_search (void *key, TREE *bt) {
	return _btree_search (key, bt->root, bt);
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

void btree_erase (void *key, TREE *bt)
{
	_btree_erase (key, bt->root, bt);
}

void btree_uinit (TREE *bt)
{
	_btree_uinit (bt->root);
	free (bt->auxData);
	free (bt);
}

void btree_print (TREE *bt)
{
	_btree_print (bt->root, bt);
}

void *_btree_search (void *key, NODE *node, TREE *bt)
{
	i64 pos = _btree_bsearch (key, node, bt);
	
	if (node->index)
		return _btree_search (key, node->child[pos], bt);
	else
		if (bt->compar (key, node->data + pos * bt->sz) == 0)
			return node->data + pos * bt->sz;
	return NULL;
}

bool _btree_insert (void *key, NODE *node, TREE *bt)
{
	i64 pos = _btree_bsearch (key, node, bt);

	if (node->index) {
		if (!_btree_insert (key, node->child[pos], bt)) {
			if (node->n == btree_max(bt)) {
				_btree_split (node, bt, pos);
				return false;
			} else {
				_btree_move_foward (node->data, pos, node->n, bt->sz);
				_btree_move_foward ((u8*)node->child, pos+1, node->n+1, sizeof (NODE*));
				
				memcpy ((node->data + pos * bt->sz), bt->auxData, bt->sz);
				node->child[pos+1] = bt->auxNode;
				node->n++;
			}
		}
	} else {
		if (node->n == btree_max(bt)) {
			_load_btree_aux (bt, key, NULL);
			_btree_split (node, bt, pos);
			return false;
		} else {
			_btree_move_foward (node->data, pos, node->n, bt->sz);
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

		_btree_move_foward (node->data, pos, node->n, bt->sz);
		memcpy (node->data + pos * bt->sz, bt->auxData, bt->sz);
		
		if (node->index) {
			memcpy (new->child, node->child + node->n, (bt->order-node->n) * sizeof (NODE*));
			
			_btree_move_foward ((u8*)node->child, pos + 1, node->n + 1, sizeof (NODE*));
			node->child[pos+1] = bt->auxNode;
		}
		
	} else {
		memcpy (new->data, node->data + (node->n) * bt->sz, (bt->order-node->n-1) * bt->sz);
		
		pos -= node->n;
		
		_btree_move_foward (new->data, pos, new->n, bt->sz);
		memcpy (new->data + pos * bt->sz, bt->auxData, bt->sz);

		if (node->index) {
			memcpy (new->child, node->child + (new->n + !(bt->order%2)), (bt->order - new->n - !(bt->order%2)) * sizeof (NODE*));
			_btree_move_foward ((u8*)new->child, pos, node->n, sizeof (NODE*));
			new->child[pos] = bt->auxNode;
		}
		
	}

	_btree_clear_space (node->data, btree_max(bt), node->n, bt->sz);
	_load_btree_aux (bt, new->data, new);
	
	if (!node->index) {
		new->next = node->next;
		node->next = new;
	} else {
		_btree_move_backwards (new->data, 0, new->n, bt->sz);
		new->n--;

		_btree_clear_space ((u8*)node->child, bt->order, node->n+1, sizeof (NODE*));
		_btree_clear_space ((u8*)new->child, bt->order, new->n+1, sizeof (NODE*));
	}
}

void _btree_print (NODE *node, TREE *bt)
{
	_print_node (node);
	if (node->index)
		for (u32 i = 0; i <= node->n; i++)
			_btree_print (node->child[i], bt);
}

bool _btree_erase (void *key, NODE *node, TREE *bt)
{
	
	i64 pos = _btree_bsearch (key, node, bt);

	if (node->index) {
		NODE *child = node->child[pos];
		
		if (!_btree_erase (key, child, bt)) {
			
			if (child->index && bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0)
				memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
			
			if (!_btree_borrow (node, bt, pos)) {

				if (bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0)
					memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
				
				if (!_btree_merge (node, bt, pos)) {
					
					if (bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0)
						memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
					
					if (bt->root == node) {
						
						if (node->n == 0) {
							bt->root = node->child[0];
							
							free (node->data);
							free (node->child);
							free (node);

						} else
							return true;
					}
					return false;
				}
			}

			if (bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0)
				memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
			
			return true;
			
		} else {
			
			if (bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0)
				memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
			
			return true;
			
		}
		
	} else {
		
		if (bt->compar (key, node->data + pos * bt->sz) != 0)
			return true;
		
		_btree_move_backwards (node->data, pos, node->n, bt->sz);
		node->n--;
		
		_load_btree_aux (bt, node->data, NULL);
		
		if (node->n >= btree_min(bt)) {
			return true;
		}
	}
	return false;
}

bool _btree_borrow (NODE *node, TREE *bt, u32 pos)
{
	
	if (pos < node->n) {
		NODE *right = node->child[pos+1];
		
		if (right->n > btree_min(bt)) {
			NODE *target = node->child[pos];
			
			memcpy (target->data + target->n * bt->sz, node->data + pos * bt->sz, bt->sz);
			target->n++;
			
			if (target->index) {
				memcpy (node->data + pos * bt->sz, right->data, bt->sz);
				_btree_move_backwards (right->data, 0, right->n, bt->sz);
				
				target->child[target->n] = right->child[0];
				_btree_move_backwards ((u8*)right->child, 0, right->n+1, sizeof(NODE*));

			} else {
				_btree_move_backwards (right->data, 0, right->n, bt->sz);
				memcpy (node->data + pos * bt->sz, right->data, bt->sz);
				_load_btree_aux (bt, target->data, NULL);
			}
			
			right->n--;

			return true;
		}
	}
	
	if (pos > 0) {
		NODE *left = node->child[pos-1];
		
		if (left->n > btree_min(bt)) {
			NODE *target = node->child[pos];

			_btree_move_foward (target->data, 0, target->n, bt->sz);
			target->n++;
			
			
			if (target->index) {
				_btree_move_foward ((u8*)target->child, 0, target->n+1, sizeof(NODE*));
				target->child[0] = left->child[left->n];
				left->n--;
				
				memcpy (target->data, node->data + (pos-1) * bt->sz, bt->sz);
				memcpy (node->data + (pos-1) * bt->sz, left->data + left->n * bt->sz, bt->sz);
			} else {
				left->n--;
				
				memcpy (node->data + (pos-1) * bt->sz, left->data + left->n * bt->sz, bt->sz);
				memcpy (target->data, node->data + (pos-1) * bt->sz, bt->sz);
				_load_btree_aux (bt, target->data, NULL);
			}
			
			_btree_clear_space (left->data, btree_max(bt), left->n, bt->sz);
			
			return true;
		}
	}

	return false;
}


bool _btree_merge (NODE *node, TREE *bt, i64 pos)
{
	NODE *target = node->child[pos-1 + !pos];
	NODE *right = node->child[pos + !pos];

	memcpy (target->data + (target->n + target->index) * bt->sz, right->data, right->n * bt->sz);

	
	if (target->index) {

		memcpy (target->data + target->n * bt->sz, node->data + (pos + !pos - 1) * bt->sz, bt->sz);
		memcpy (target->child + target->n + 1, right->child, (right->n + 1) * sizeof (NODE*));
		
		free (right->child);
	} else {
		target->next = right->next;
		_load_btree_aux (bt, target->data, NULL);
	}
	target->n += right->n + target->index;
	
	
	
	_btree_move_backwards (node->data, pos-1, node->n, bt->sz);
	_btree_move_backwards ((u8*)node->child, pos+!pos, node->n+1, sizeof (NODE*));
	
	node->n--;
	
	_btree_clear_space (node->data, btree_max(bt), node->n,  bt->sz);
	_btree_clear_space ((u8*)node->child, btree_max(bt)+1, node->n+1,  sizeof (NODE*));

	free (right->data);
	free (right);
	
	if (node->n < btree_min(bt))
		return false;

	return true;
}

void _print_node (NODE *node)
{
	printf ("qttKeys: %d\n", node->n);
	for (int i = 0; i < node->n; i++)
		printf ("%d ", *(u32*)(node->data+i*16));
	printf ("\n");
	if (node->index) {
		for (int i = 0; i <= node->n; i++)
			printf ("%p ", node->child[i]);
		printf ("\n");
	}
	printf ("\n");
}

void _btree_uinit (NODE *node)
{
	//_print_node (node);
	if (node->index) {
		for (u32 i = 0; i <= node->n; i++)
			_btree_uinit (node->child[i]);
		free (node->child);
	}
	
	free (node->data);
	free (node);
}


NODE *_malloc_node (TREE *bt, bool index)
{
	
	NODE *out = malloc(sizeof(NODE));
	out->n = 0;
	out->data = malloc(btree_max(bt) * bt->sz);
	_btree_clear_space (out->data, btree_max(bt), 0, bt->sz);
	
	if (index) {
		out->child = malloc (sizeof(NODE*) * bt->order);
		_btree_clear_space ((u8*)out->child, btree_max(bt) + 1, 0, sizeof (NODE*));
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

i64 _btree_bsearch (void *key, NODE *node, TREE *bt)
{
	
	i64 pos = 0;
	if (node->index)
		for (; pos < node->n && bt->compar (key, node->data + pos*bt->sz) >= 0; pos++);
	else
		for (; pos < node->n && bt->compar (key, node->data + pos*bt->sz) > 0; pos++);
	
	
	
	//uint64_t pos = b_search (key, node->data, node->n, bt->sz, bt->compar);
	//if ((pos == node->n-1 || pos == 0) && bt->compar (key, (char*)node->data+pos*bt->sz) > 0)
	//	pos++;
	return pos;
}

void _btree_move_foward (u8 *data, i64 pos, u32 n, usize sz)
{
	memmove (data + max (pos, 0)  * sz + sz, data + max (pos, 0)  * sz, max ((i64)(n - pos) * sz, 0));
}

void _btree_move_backwards (u8 *data, i64 pos, u32 n, usize sz)
{
	memmove (data + max (pos, 0) * sz, data + max (pos, 0)  * sz + sz, max ((i64)(n - pos - 1) * sz, 0));
	memset (data + n * sz - sz, 0, sz);
}

void _btree_clear_space (u8 *data, u32 maxS, u32 n, usize sz)
{
	memset (data + n * sz, 0, max ((i64)(maxS - n) * sz, 0));
}