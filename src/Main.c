#include "log.h"
#include "vector/vector.h"
#include "btree/btree.h"

#include <time.h>

#include <stdlib.h>

void btree_test (void);

int testFunction (const void *key, const void *elem) {
	return*((u64*)key) - *((u64*)elem);
}

void shuffle(u32 *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          u32 t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

int main(int argc, char* argv[])
{
	
	//LOG_ERROR("test");
	//LOG_INFO("test");
	//LOG_TRACE("test");
	//LOG_WARN("test");

	//LOGF_ERROR("Nice! %u", rand());
	//LOGF_INFO("Nice! %u", rand());
	//LOGF_TRACE("Nice! %u", rand());
	//LOGF_WARN("Nice! %u", rand());

	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_init);
	DEBUG_DLSYM(vector, vector_push_back);
	DEBUG_DLSYM(vector, vector_at);
	DEBUG_DLSYM(vector, vector_uninit);

	constexpr usize testSize = 767;
	u32 sum = 0;
	u32 intTest;
	Vector *vecTest = vector_init(sizeof intTest);
	
	for(usize j = 0; j < testSize; ++j){
		intTest = rand();
		VectorError r = vector_push_back(vecTest, &intTest);
		if(r != vectorErrorSuccess)
			break;
		r = vector_at(vecTest, j, &intTest);
		if(r != vectorErrorSuccess)
			break;
		sum += intTest;
	}
	
	vector_uninit(vecTest);

	DEBUG_DLCLOSE(vector);
	//LOGF_INFO("Vector sum: %u", sum);

	DEBUG_DLOPEN("btree/", btree);
	DEBUG_DLSYM(btree, btree_init);
	DEBUG_DLSYM(btree, btree_insert);
	DEBUG_DLSYM(btree, btree_uinit);
	DEBUG_DLSYM(btree, btree_erase);
	DEBUG_DLSYM(btree, btree_search);
	DEBUG_DLSYM(btree, btree_print);
	
	//usize btSize = 10000;
	//usize btN = 13000;
	//usize btOrder = 4;
	
	//u32 testArray[btSize];
	//for (int i = 0; i < btSize; i++)
	//	testArray[i] = i;
	
	//TREE *bts[btN];
	
	/*
	for (int i = 0; i < btN; i++) {
		bts[i] = btree_init (sizeof intTest, btOrder + i/1000, testFunction);
		
		//srand(time(NULL));
		shuffle (testArray, btSize);
		
		for (usize j = 0; j < btSize; ++j){
			intTest = testArray[j];
			btree_insert (&intTest, bts[i]);
		}
		
		shuffle (testArray, btSize);
		
		for (usize j = 0; j < btSize; ++j){
			intTest = testArray[j];
			btree_erase (&intTest, bts[i]);
		}
		
		
		btree_uinit (bts[i]);
	}
	*/
	
	//UNITY_BEGIN();
	
	printf ("\n");
	btree_test();
	
	//RUN_TEST(btree_test);
	
	DEBUG_DLCLOSE(btree);
	
	//printf("Hello, world!\n");
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

void btree_test (void) {
	
	TREE *bt[13];
	
	for (usize i = 4; i <= 16; i++)
		bt[i-4] = btree_init (sizeof (testStruct), i, compFunction);
	
	usize btSize = 1000;
	usize btN = 65;
	
	testStruct data[btSize];
	
	for (usize i = 0; i < btSize; i++) {
		data[i].num = i;
		data[i].key = i * 10;
	}
	
	testStruct insert;
	testStruct *search;
	
	shuffleTestStruct (data, btSize);
	
	for (usize i = 0; i < btN; i++) {
		srand (time(NULL));
		shuffleTestStruct (data, btSize);
		
		for (usize j = 0; j < btSize; j++) {
			insert = data[j];
			btree_insert (&insert, bt[ (i / (btN / 13))]);
		}
		
		shuffleTestStruct (data, btSize);
		
		for (usize j = 0; j <= btSize; j++) {
			insert = data[j];
			btree_erase (&insert, bt[ (i / (btN / 13))]);
			
			for (usize k = 0; k < btSize; k++) {
				insert = data[k];
				search = btree_search (&insert, bt[ (i / (btN / 13))]);
				if (!search && k > j) {
					printf ("fodeu %d %d %d\n", i, j, k);
					//printf ("%u, %llu\n", insert.num, insert.key);
					//btree_print (bt[ (i / (btN / 13))]);
					return;
				}
			}
		}
	}
}