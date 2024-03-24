#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h> 

#include "btree/btree.h"
#include "external/unity.h"
#include "external/unity_memory.h"
#include "external/unity_fixture.h"

TEST_GROUP(btree);

TEST_SETUP(btree)
{
	UnityMalloc_StartTest();
}

TEST_TEAR_DOWN(btree)
{
	UnityMalloc_EndTest();
}

const void* sstring_allocator(const void *el, u8 *vecData)
{
	if(el){
		char *strEl = *(char**)el;
		usize strSz = strlen(strEl);
		char *newStr = malloc(strSz + 1);
		if(!newStr)
			return nullptr;
		strncpy(newStr, strEl, strSz + 1);
		newStr[strSz] = '\0';
		memcpy(vecData, &newStr, sizeof strEl);
		return newStr;
	}
	else{
		char *toFree;
		memcpy(&toFree, vecData, sizeof toFree);
		free(toFree);
		return nullptr;
	}
}

typedef struct {
	u32 num;
	u64 key;
} testStruct;

int compFunction (const void *key, const void *elem) {
	return ((testStruct*)key)->num - ((testStruct*)elem)->num;
}

int compString (const void *key, const void *elem) {
	char *strEl1 = *(char**)key;
	char *strEl2 = *(char**)elem;
	
	return strcmp (strEl1, strEl2);
}

void shuffleTestStruct(testStruct *array, size_t n)
{
    if (n > 1) 
    {
        usize i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          testStruct t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void shuffle (u8 *array, usize n, usize sz, int (*compar)(const void *a, const void *b))
{
	if (n > 1)
	{
		usize i;
		void *data = malloc (sz);
		for (i = 0; i < n - 1; i++)
		{
			usize j = i + rand() / (RAND_MAX / (n - i) + 1);
			memcpy (data, array + j * sz, sz);
			memcpy (array + j * sz, array + i * sz, sz);
			memcpy (array + i * sz, data, sz);
		}
		free (data);
	}
}

int usize_compare(const void *a, const void *b)
{
	return *((usize*)a) - *((usize*)b);
}

int char_compare(const void *a, const void *b)
{
	return *((unsigned char*)a) - *((unsigned char*)b);
}

TEST(btree, btree_init)
{
	usize data;
	TREE *bt = btree_init(sizeof data, 4, usize_compare);
	TEST_ASSERT_NOT_NULL(bt);
	btree_uninit(bt);
	
	UnityMalloc_MakeMallocFailAfterCount(0);
	bt = btree_init(sizeof data, 4, usize_compare);
	TEST_ASSERT_NULL(bt);
	
	UnityMalloc_MakeMallocFailAfterCount(1);
	bt = btree_init(sizeof data, 4, usize_compare);
	TEST_ASSERT_NULL(bt);
}

TEST (btree, btree_allocator)
{
	char **data = nullptr;
	TREE *bt = btree_init_with_allocator(sizeof (char*), 4, compString, sstring_allocator);
	TEST_ASSERT_NOT_NULL(bt);
	
	char string1[] = "test";
	char string2[] = "test2";
	char string3[] = "test3";
	char string4[] = "test4";
	
	char *stringPtr = &string1[0];
	btree_insert(bt, &stringPtr);
	stringPtr = &string2[0];
	btree_insert(bt, &stringPtr);
	stringPtr = &string3[0];
	btree_insert(bt, &stringPtr);
	stringPtr = &string4[0];
	btree_insert(bt, &stringPtr);
	
	stringPtr = &string1[0];
	data = btree_search (bt, &stringPtr);
	TEST_ASSERT_EQUAL_STRING ("test", *data);
	stringPtr = &string2[0];
	data = btree_search (bt, &stringPtr);
	TEST_ASSERT_EQUAL_STRING ("test2", *data);
	stringPtr = &string3[0];
	data = btree_search (bt, &stringPtr);
	TEST_ASSERT_EQUAL_STRING ("test3", *data);
	stringPtr = &string4[0];
	data = btree_search (bt, &stringPtr);
	TEST_ASSERT_EQUAL_STRING ("test4", *data);

	
	btree_uninit(bt);
}

TEST (btree, btree_iterator)
{
	UnityMalloc_MakeMallocFailAfterCount(-1);
	usize data;
	TREE *bt = btree_init(sizeof data, 4, usize_compare);
	TEST_ASSERT_NOT_NULL(bt);
	
	auto it = btree_iterator_begin (bt);
	TEST_ASSERT_NULL (btree_iterator_get_data (it));
	TEST_ASSERT_TRUE (btree_iterator_end (it));
	
	UnityMalloc_MakeMallocFailAfterCount(0);
	it = btree_iterator_begin(bt);
	TEST_ASSERT_NULL(it);
	
	UnityMalloc_MakeMallocFailAfterCount(-1);
	data = 0;
	btree_insert (bt, &data);
	UnityMalloc_MakeMallocFailAfterCount(1);
	it = btree_iterator_begin (bt);
	TEST_ASSERT_NULL(it);
	UnityMalloc_MakeMallocFailAfterCount(2);
	it = btree_iterator_begin (bt);
	TEST_ASSERT_NULL(it);
	UnityMalloc_MakeMallocFailAfterCount(3);
	it = btree_iterator_begin (bt);
	TEST_ASSERT_NULL(it);
	
	
	UnityMalloc_MakeMallocFailAfterCount(-1);
	it = btree_iterator_begin (bt);
	usize *rData = btree_iterator_get_data (it);
	TEST_ASSERT_EQUAL (0, *rData);
	TEST_ASSERT_FALSE (btree_iterator_end (it));
	btree_iterator_next (it);
	TEST_ASSERT_TRUE (btree_iterator_end (it));
	
	data = 1;
	btree_insert (bt, &data);
	data = 2;
	btree_insert (bt, &data);
	data = 3;
	btree_insert (bt, &data);
	
	for (it = btree_iterator_begin (bt), data = 0; !btree_iterator_end (it); btree_iterator_next (it), data++) {
		rData = btree_iterator_get_data (it);
		TEST_ASSERT_EQUAL (data, *rData);
	}
	
	it = btree_iterator_begin (bt);
	btree_iterator_uninit (it);
	
	btree_uninit(bt);
	
}

TEST (btree, btree_search)
{
	unsigned char data;
	char *rData;
	TREE *bt = btree_init (sizeof data, 4, char_compare);
	TEST_ASSERT_NOT_NULL(bt);

	for (int i = 0; i <= 255; i++) {
		data = i;
		rData = btree_search (bt, &data);
		TEST_ASSERT_NULL (rData);
	}
	
	TEST_ASSERT_NULL (btree_search (bt, &data));
	
	data = 0;
	btree_insert (bt, &data);
	
	int maxData = 0;
	for (int i = 0; i <= 255; i++) {
		data = i;
		rData = btree_search (bt, &data);
		if (i <= maxData)
			TEST_ASSERT_EQUAL (i, *rData);
		else
			TEST_ASSERT_NULL (rData);
	}
	
	data = 1;
	btree_insert (bt, &data);
	data = 2;
	btree_insert (bt, &data);
	data = 3;
	btree_insert (bt, &data);
	
	maxData = 3;
	for (int i = 0; i <= 255; i++) {
		data = i;
		rData = btree_search (bt, &data);
		if (i <= maxData)
			TEST_ASSERT_EQUAL (i, *rData);
		else
			TEST_ASSERT_NULL (rData);
	}
	
	btree_uninit(bt);
}

TEST (btree, btree_insert)
{
	usize data;
	TREE *bt = btree_init(sizeof data, 4, usize_compare);
	TEST_ASSERT_NOT_NULL(bt);
	
	usize n = 1000;
	usize array[n];
	for (usize i = 0; i < n; i++) {
		array[i] = i;
	}
	srand(time(NULL));
	shuffle ((u8*)array, n, sizeof data, usize_compare);

	for (usize i = 0; i < n; i++) {
		btree_insert (bt, &array[i]);
		TEST_ASSERT_NOT_NULL (btree_search(bt, &array[i]));
	}
	btree_uninit(bt);
}

TEST (btree, btree_erase)
{
	usize data;
	TREE *bt = btree_init(sizeof data, 4, usize_compare);
	TEST_ASSERT_NOT_NULL(bt);

	btree_erase (bt, &data);
	
	data = 0;
	btree_insert (bt, &data);
	
	data = 1;
	btree_erase (bt, &data);
	data = 0;
	btree_erase (bt, &data);

	btree_uninit(bt);
}

TEST (btree, btree_init_insert_uninit)
{
	TREE *bt;
	bt = btree_init (sizeof (testStruct), 4, compFunction);
	TEST_ASSERT_NOT_NULL(bt);
	
	testStruct data[10000];
	
	for (usize i = 0; i < 10000; i++) {
		data[i].num = i;
		data[i].key = i * 10;
	}
	
	srand (time(NULL));
	shuffleTestStruct (data, 10000);
	
	for (usize j = 0; j < 10000; j++) {
		testStruct insert = data[j];
		btree_insert (bt, &insert);
	}
	
	btree_uninit (bt);
	
}

TEST(btree, btree_all)
{
	
	TREE *bt[13];
	usize btSize = 1000;
	usize btN = 13;
	testStruct insert;
	testStruct data[btSize];
	
	for (usize i = 0; i < 13; i++) {
		bt[i] = btree_init (sizeof (testStruct), i+4, compFunction);
		TEST_ASSERT_NOT_NULL(bt[i]);
	}
	
	for (usize i = 0; i < btSize; i++) {
		data[i].num = i;
		data[i].key = i * 10;
	}

	for (usize i = 0; i < btN; i++) {
		srand (time(NULL));
		shuffleTestStruct (data, btSize);
		
		for (usize j = 0; j < btSize; j++) {
			insert = data[j];
			btree_insert (bt[ (i / (btN / 13))], &insert);
		}
		
		auto it = btree_iterator_begin (bt[ (i / (btN / 13))]);
		if (!it)
			return;
		btree_iterator_uninit (it);

		for (auto it = btree_iterator_begin (bt[ (i / (btN / 13))]); !btree_iterator_end (it); btree_iterator_next (it)) {
			testStruct *data = btree_iterator_get_data (it);
			if (!data)
				return;
			//printf ("%d\n", data->num);
		}
	
		
		
		shuffleTestStruct (data, btSize);
		
		for (usize j = 0; j < btSize; j++) {
			insert = data[j];
			btree_erase (bt[ (i / (btN / 13))], &insert);
			
			for (usize k = 0; k < btSize; k++) {
				insert = data[k];
				testStruct *search = btree_search (bt[ (i / (btN / 13))], &insert);

				if (k <= j)
					TEST_ASSERT_NULL(search);
				else {
					TEST_ASSERT_EQUAL (data[k].num, search->num);
					TEST_ASSERT_EQUAL (data[k].key, search->key);
				}
			}
			
		}
		
	}
	
	for (usize i = 0; i < 13; i++) {
		btree_uninit (bt[i]);
	}
}
