#include <stdlib.h>
#include <time.h>

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

typedef struct {
	u32 num;
	u64 key;
} testStruct;

int compFunction (const void *key, const void *elem) {
	return ((testStruct*)key)->num - ((testStruct*)elem)->num;
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

TEST(btree, btree_all)
{
	
	TREE *bt[13];
	usize btSize = 1000;
	usize btN = 13;
	testStruct insert;
	testStruct search;
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
		
		shuffleTestStruct (data, btSize);
		
		for (usize j = 0; j < btSize; j++) {
			insert = data[j];
			btree_erase (bt[ (i / (btN / 13))], &insert);
			
			for (usize k = 0; k < btSize; k++) {
				insert = data[k];
				bool result = btree_search (bt[ (i / (btN / 13))], &insert, &search);

				if (k <= j)
					TEST_ASSERT_EQUAL(false, result);
				else {
					TEST_ASSERT_EQUAL (data[k].num, search.num);
					TEST_ASSERT_EQUAL (data[k].key, search.key);
				}
			}
		}
	}
	
	for (usize i = 0; i < 13; i++) {
		btree_uninit (bt[i]);
	}
}
