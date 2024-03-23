#define DLL_HEADER_SOURCE
#include "btree/btree_internal.h"
#include "btree/btree.h"
DEBUG_DEFINE_VTABLE(btree)
#undef DLL_HEADER_SOURCE

#include "log.h"

BTreeIterator *btree_iterator_begin (TREE bt[restrict const static 1])
{
	BTreeIterator *it = malloc(sizeof(BTreeIterator));
	if (!it)
		return nullptr;

	it->bt = bt;
	it->node = _btree_read_node (bt, bt->root);
	
	while (it->node && it->node->index)
		it->node = _btree_read_node (bt, it->node->child[0]);
	it->node = _btree_read_node (bt, (usize)_clone_node_index ((usize)it->node, bt));
	
	if (bt->lastError == bTreeErrorOutOfMemory) {
		btree_iterator_uninit (it);
		return nullptr;
	}
	
	it->pos = 0;
	
	if (it->node) {
		it->data = malloc (bt->sz);
		if (!it->data) {
			btree_iterator_uninit (it);
			return nullptr;
		}
		_btree_iterator_load_data (it);
	} else {
		it->data = nullptr;
	}

	return it;
}

void btree_iterator_uninit (BTreeIterator *it)
{
	if (it->node) {
		NODE *next = _btree_read_node(it->bt, it->node->next);
		while (next) {
			free (it->node->data);
			free (it->node);
			it->node = next;
			next = _btree_read_node(it->bt, it->node->next);
		}
		free (it->node->data);
		free (it->node);
	}
	
	free (it->data);
	free (it);
}

void *btree_iterator_get_data (BTreeIterator it[restrict const static 1])
{
	return it->data;
}

bool btree_iterator_end (BTreeIterator *it)
{
	if (it->node) {
		if (it->pos != it->node->n)
			return false;
	}
	
	btree_iterator_uninit (it);
	return true;
}

void btree_iterator_next (BTreeIterator it[restrict const static 1])
{
	
	if (it->pos < it->node->n-1) {
		it->pos++;
		_btree_iterator_load_data (it);
	} else if (it->node->next) {
		NODE *next = _btree_read_node(it->bt, it->node->next);
		free (it->node->data);
		free (it->node);
		it->node = next;
		it->pos = 0;
		_btree_iterator_load_data (it);
	} else 
		it->pos++;
}

void* btree_search (TREE bt[restrict const static 1], const void *key)
{
	if (!_btree_root(bt))
		return nullptr;
	return _btree_search (key, bt->root, bt);
}

void btree_insert (TREE bt[restrict static 1], const void *key)
{
	if (!_btree_root(bt)) {
		NODE *new = _malloc_node (bt, false);
		
		if (!new)
			return;
		
		new->n = 1;
		_btree_set_element (key, new, bt, 0);
		
		bt->n++;
		bt->root = (usize)new;
		
		_btree_write_node (bt, new);
		return;
	}
	
	if (!_btree_insert (key, (usize)bt->root, bt)) {
		NODE *new = _malloc_node (bt, true);
		
		if (!new)
			return;
		
		new->child[0] = bt->root;
		new->child[1] = bt->auxNode;
		new->n = 1;
		_btree_set_element (bt->auxData, new, bt, 0);
		
		bt->n++;
		bt->root = (usize)new;
		
		_btree_write_node (bt, new);
	}
}

TREE *btree_init (const usize sz, const u32 order, int (*compar)(const void*, const void*))
{
	return btree_init_with_allocator (sz, order, compar, nullptr);
}

TREE *btree_init_with_allocator (const usize sz, const u32 order, int (*compar)(const void*, const void*), const void* (*customAllocator)(const void*, u8*))
{
	LOGF_TRACE("BTree = { .root = %p, .order = %u, .n = %u, .sz = %zu }", NULL, order, 0, sz);
	TREE *bt = malloc(sizeof *bt);

	if(!bt)
		return nullptr;
	
	
	bt->root = 0;
	bt->order = order;
	bt->n = 0;
	bt->sz = sz;
	bt->compar = compar;
	
	
	bt->auxData = malloc (sz);
	if(!bt->auxData){
		free(bt);
		return nullptr;
	}
	bt->auxNode = 0;
	
	bt->customAllocator = customAllocator;
	
	return bt;
}

void btree_erase (TREE bt[restrict static 1], const void *key)
{
	if (!_btree_root(bt))
		return;
	_btree_erase (key, (usize)bt->root, bt);
}

void btree_uninit (TREE *bt)
{
	if (_btree_root(bt))
		_btree_uinit (bt, bt->root);
	free (bt->auxData);
	free (bt);
}