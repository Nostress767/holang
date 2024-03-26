#pragma once

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "log.h"

#include "btree.h"

INTERNAL u32 btree_min(TREE bt[static 1])
{
	return (bt->order + 1) / 2 - 1;
}

INTERNAL u32 btree_max(TREE bt[static 1])
{
	return bt->order - 1;
}

/* Where is this being defined ??? */
#undef max

INTERNAL i64 max(i64 a, i64 b)
{
	return a > b ? a : b;
}

INTERNAL bool _btree_root(TREE bt[static 1])
{
	return bt->root != 0;
}

INTERNAL i64 _btree_bsearch (const void *key, NODE *node, TREE bt[static 1]);
INTERNAL bool _btree_insert (const void *key, usize n, TREE bt[static 1]);
INTERNAL NODE *_malloc_node (TREE bt[static 1], bool index);
INTERNAL void _load_btree_aux (TREE bt[static 1], const void *key, NODE *node);
INTERNAL void _btree_split (NODE *node, TREE bt[static 1], i64 pos);
INTERNAL void _btree_uinit (TREE bt[static 1], usize n);
INTERNAL bool _btree_erase (const void *key, usize n, TREE bt[static 1]);
INTERNAL bool _btree_borrow (NODE *node, TREE bt[static 1], i64 pos);
INTERNAL bool _btree_merge (NODE *node, TREE bt[static 1], i64 pos);
INTERNAL void *_btree_search (const void *key, usize n, TREE bt[static 1]);
INTERNAL void _btree_move_foward (u8 *data, i64 pos, u32 n, usize sz);
INTERNAL void _btree_move_backwards (u8 *data, i64 pos, u32 n, usize sz);
INTERNAL void _btree_clear_space (u8 *data, u32 maxS, u32 n, usize sz);
INTERNAL NODE *_clone_node_index (usize n, TREE bt[static 1]);
INTERNAL void _btree_iterator_load_data (BTreeIterator it[restrict const static 1]);

INTERNAL const void* _btree_set_element (const void *key, NODE *node, TREE bt[static 1], i64 pos);
INTERNAL NODE *_btree_read_node (TREE bt[static 1], usize n);
INTERNAL void _btree_write_node (TREE bt[static 1], NODE *node);


INTERNAL const void* _btree_set_element (const void *key, NODE *node, TREE bt[static 1], i64 pos)
{
	return bt->customAllocator && !node->index ?
	bt->customAllocator(key, node->data + pos * bt->sz)
	: key ? memcpy ((node->data + pos * bt->sz), key, bt->sz) : memset ((node->data + pos * bt->sz), 0, bt->sz);
}

INTERNAL NODE *_btree_read_node (TREE bt[static 1], usize n)
{
	return (NODE*)n;
}

INTERNAL void _btree_write_node (TREE bt[static 1], NODE *node)
{
	node->modified = false;
	
	if (node && node->n == 0) {
		free (node->data);
		if (node->index)
			free (node->child);
		free (node);
	}
	
	return;
}


INTERNAL void _btree_iterator_load_data (BTreeIterator it[restrict const static 1])
{
	memcpy (it->data, it->node->data + it->pos * it->bt->sz, it->bt->sz);
}

INTERNAL NODE *_clone_node_index (usize n, TREE bt[static 1]) {
	
	NODE *node = _btree_read_node (bt, n);
	
	if (!node)
		return nullptr;
	
	NODE *new = _malloc_node (bt, node->index);
	
	if (!new)
		return nullptr;
	
	
	memcpy (new->data, node->data, btree_max(bt) * bt->sz);
	new->n = node->n;
	
	new->next = (usize)_clone_node_index ((usize)node->next, bt);
	
	return new;
}


INTERNAL void *_btree_search (const void *key, usize n, TREE bt[static 1])
{
	NODE *node = _btree_read_node (bt, n);
	i64 pos = _btree_bsearch (key, node, bt);
	
	if (node->index)
		return _btree_search (key, (usize)node->child[pos], bt);
	else
		if (bt->compar (key, node->data + pos * bt->sz) == 0)
			return node->data + pos * bt->sz;
	return NULL;
}

INTERNAL bool _btree_erase (const void *key, usize n, TREE bt[static 1])
{
	NODE *node = _btree_read_node (bt, n);
	i64 pos = _btree_bsearch (key, node, bt);

	if (node->index) {
		if (!_btree_erase (key, node->child[pos], bt)) {
			
			if (bt->auxIndex && bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0) {
				node->modified = true;
				memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
			}
			bt->auxIndex = true;

			if (!_btree_borrow (node, bt, pos)) {
				
				if (bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0) {
					node->modified = true;
					memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
				}
				
				if (!_btree_merge (node, bt, pos)) {
					node->modified = true;
					if (bt->root == n) {
						if (node->n == 0) {
							bt->root = node->child[0];
							_btree_write_node (bt, node);
						}
						else {
							_btree_write_node (bt, node);
							return true;
						}
					}
					
					return false;
				}
			}
			
			node->modified = true;
			if (bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0)
				memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
			
			_btree_write_node (bt, node);
			return true;
			
		} else {
			if (bt->compar (key, node->data + max ((pos - 1), 0) * bt->sz) == 0)
				memcpy (node->data + max ((pos - 1), 0)  * bt->sz, bt->auxData, bt->sz);
			
			_btree_write_node (bt, node);
			return true;
		}
	} else {
		
		if (bt->compar (key, node->data + pos * bt->sz) != 0) {
			_btree_write_node (bt, node);
			return true;
		}
		
		_btree_set_element (nullptr, node, bt, pos);
		_btree_move_backwards (node->data, pos, node->n, bt->sz);
		node->n--;
		node->modified = true;
		
		_load_btree_aux (bt, node->data, NULL);
		bt->auxIndex = false;
		
		if (bt->root == n && node->n == 0) {
			bt->root = 0;
			_btree_write_node (bt, node);
			return true;
		}
		
		if (node->n >= btree_min(bt)) {
			_btree_write_node (bt, node);
			return true;
		}
	}
	
	return false;
}

INTERNAL bool _btree_borrow (NODE *node, TREE bt[static 1], i64 pos)
{
	if (pos < node->n) {
		NODE *right = _btree_read_node (bt, (usize)node->child[pos+1]);
		
		if (right->n > btree_min(bt)) {
			NODE *target = _btree_read_node (bt, (usize)node->child[pos]);
			
			memcpy (target->data + target->n * bt->sz, node->data + pos * bt->sz, bt->sz);
			target->n++;
			
			if (target->index) {
				memcpy (node->data + pos * bt->sz, right->data, bt->sz);
				_btree_move_backwards (right->data, 0, right->n, bt->sz);
				
				target->child[target->n] = right->child[0];
				_btree_move_backwards ((u8*)right->child, 0, right->n+1, sizeof(usize));

			} else {
				_btree_move_backwards (right->data, 0, right->n, bt->sz);
				memcpy (node->data + pos * bt->sz, right->data, bt->sz);
				_load_btree_aux (bt, target->data, NULL);
			}
			
			right->n--;
			
			_btree_write_node (bt, target);
			_btree_write_node (bt, right);
			return true;
		}
		_btree_write_node (bt, right);
	}
	
	if (pos > 0) {
		NODE *left = _btree_read_node (bt, (usize)node->child[pos-1]);
		
		if (left->n > btree_min(bt)) {
			NODE *target = _btree_read_node (bt, (usize)node->child[pos]);

			_btree_move_foward (target->data, 0, target->n, bt->sz);
			target->n++;
			
			
			if (target->index) {
				_btree_move_foward ((u8*)target->child, 0, target->n+1, sizeof(usize));
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
			
			_btree_write_node (bt, target);
			_btree_write_node (bt, left);
			return true;
		}
		_btree_write_node (bt, left);
	}

	return false;
}

INTERNAL bool _btree_merge (NODE *node, TREE bt[static 1], i64 pos)
{
	NODE *target = _btree_read_node (bt, (usize)node->child[pos-1 + !pos]);
	NODE *right = _btree_read_node (bt, (usize)node->child[pos + !pos]);

	memcpy (target->data + (target->n + target->index) * bt->sz, right->data, right->n * bt->sz);

	
	if (target->index) {
		memcpy (target->data + target->n * bt->sz, node->data + (pos + !pos - 1) * bt->sz, bt->sz);
		memcpy (target->child + target->n + 1, right->child, (right->n + 1) * sizeof (usize));
	} else {
		target->next = right->next;
		_load_btree_aux (bt, target->data, NULL);
	}
	target->n += right->n + target->index;
	
	
	
	_btree_move_backwards (node->data, pos-1, node->n, bt->sz);
	_btree_move_backwards ((u8*)node->child, pos+!pos, node->n+1, sizeof (usize));
	
	node->n--;
	
	_btree_clear_space (node->data, btree_max(bt), node->n,  bt->sz);
	_btree_clear_space ((u8*)node->child, btree_max(bt)+1, node->n+1,  sizeof (usize));
	
	right->n = 0;
	_btree_write_node (bt, target);
	_btree_write_node (bt, right);
	
	if (node->n < btree_min(bt)) {
		return false;
	}

	return true;
}

INTERNAL void _btree_uinit (TREE bt[static 1], usize n)
{
	NODE *node = _btree_read_node (bt, n);
	if (node->index) {
		for (u32 i = 0; i <= node->n; i++)
			_btree_uinit (bt, (usize)node->child[i]);
		free (node->child);
	} else {
		for (u32 i = 0; i < node->n; i++)
			_btree_set_element (nullptr, node, bt, i);
	}

	
	free (node->data);
	free (node);
}


INTERNAL NODE *_malloc_node (TREE bt[static 1], bool index)
{
	
	NODE *out = malloc(sizeof(NODE));
	
	if (!out) {
		bt->lastError = bTreeErrorOutOfMemory;
		return nullptr;
	}
	
	
	out->n = 0;
	out->data = malloc(btree_max(bt) * bt->sz);
	
	if (!out->data) {
		free (out);
		bt->lastError = bTreeErrorOutOfMemory;
		return nullptr;
	}
	
	_btree_clear_space (out->data, btree_max(bt), 0, bt->sz);
	
	if (index) {
		out->child = malloc (sizeof(usize) * bt->order);
		_btree_clear_space ((u8*)out->child, btree_max(bt) + 1, 0, sizeof (usize));
	} else {
		out->next = 0;
	}

	out->index = index;
	out->modified = true;
	
	bt->lastError = bTreeErrorSuccess;
	return out;
}

INTERNAL void _load_btree_aux (TREE bt[static 1], const void *key, NODE *node)
{
	memset (bt->auxData, 0, bt->sz);
	memcpy (bt->auxData, key, bt->sz);
	bt->auxNode = (usize)node;
}

INTERNAL i64 _btree_bsearch (const void *key, NODE *node, TREE bt[static 1])
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

INTERNAL void _btree_move_foward (u8 *data, i64 pos, u32 n, usize sz)
{
	memmove (data + max (pos, 0)  * sz + sz, data + max (pos, 0)  * sz, max ((i64)(n - pos) * sz, 0));
}

INTERNAL void _btree_move_backwards (u8 *data, i64 pos, u32 n, usize sz)
{
	memmove (data + max (pos, 0) * sz, data + max (pos, 0)  * sz + sz, max ((i64)(n - max (pos, 0) - 1) * sz, 0));
	memset (data + n * sz - sz, 0, sz);
}

INTERNAL void _btree_clear_space (u8 *data, u32 maxS, u32 n, usize sz)
{
	memset (data + n * sz, 0, max ((i64)(maxS - n) * sz, 0));
}




INTERNAL bool _btree_insert (const void *key, usize n, TREE bt[static 1])
{
	NODE *node = _btree_read_node (bt, n);
	i64 pos = _btree_bsearch (key, node, bt);

	if (node->index) {
		if (!_btree_insert (key, node->child[pos], bt)) {
			node->modified = true;
			if (node->n == btree_max(bt)) {
				_btree_split (node, bt, pos);
				_btree_write_node (bt, node);
				return false;
			} else {
				_btree_move_foward (node->data, pos, node->n, bt->sz);
				_btree_move_foward ((u8*)node->child, pos+1, node->n+1, sizeof (usize));
				_btree_set_element (bt->auxData, node, bt, pos);

				node->child[pos+1] = bt->auxNode;
				node->n++;
			}
		}
	} else {
		node->modified = true;
		if (node->n == btree_max(bt)) {
			_load_btree_aux (bt, key, nullptr);
			_btree_split (node, bt, pos);
			_btree_write_node (bt, node);
			return false;
		} else {
			_btree_move_foward (node->data, pos, node->n, bt->sz);
			_btree_set_element (key, node, bt, pos);
			node->n++;
		}
	}
	
	_btree_write_node (bt, node);
	return true;
}


INTERNAL void _btree_split (NODE *node, TREE bt[static 1], i64 pos)
{
	NODE *new = _malloc_node (bt, node->index);
	if (!new)
		return;
	
	
	bt->n++;

	node->n = bt->order/2;
	new->n = bt->order - node->n;

	if (pos < bt->order/2) {
		memcpy (new->data, node->data + (node->n-1) * bt->sz, (bt->order-node->n) * bt->sz);

		_btree_move_foward (node->data, pos, node->n, bt->sz);
		_btree_set_element (bt->auxData, node, bt, pos);
		
		if (node->index) {
			memcpy (new->child, node->child + node->n, (bt->order-node->n) * sizeof (usize));
			
			_btree_move_foward ((u8*)node->child, pos + 1, node->n + 1, sizeof (usize));
			node->child[pos+1] = bt->auxNode;
		}
		
	} else {
		memcpy (new->data, node->data + (node->n) * bt->sz, (bt->order-node->n-1) * bt->sz);
		
		pos -= node->n;
		
		_btree_move_foward (new->data, pos, new->n, bt->sz);
		_btree_set_element (bt->auxData, new, bt, pos);

		if (node->index) {
			memcpy (new->child, node->child + (new->n + !(bt->order%2)), (bt->order - new->n - !(bt->order%2)) * sizeof (usize));
			_btree_move_foward ((u8*)new->child, pos, node->n, sizeof (usize));
			new->child[pos] = bt->auxNode;
		}
	}

	_btree_clear_space (node->data, btree_max(bt), node->n, bt->sz);
	_load_btree_aux (bt, new->data, new);
	
	if (!node->index) {
		new->next = node->next;
		node->next = bt->auxNode;
	} else {
		_btree_move_backwards (new->data, 0, new->n, bt->sz);
		new->n--;

		_btree_clear_space ((u8*)node->child, bt->order, node->n+1, sizeof (usize));
		_btree_clear_space ((u8*)new->child, bt->order, new->n+1, sizeof (usize));
	}

	_btree_write_node (bt, new);
}