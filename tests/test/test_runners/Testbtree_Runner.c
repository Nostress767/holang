#include "external/unity.h"
#include "external/unity_memory.h"
#include "external/unity_fixture.h"

TEST_GROUP_RUNNER(btree)
{
	
	RUN_TEST_CASE(btree, btree_init);
	RUN_TEST_CASE(btree, btree_iterator);
	RUN_TEST_CASE(btree, btree_allocator);
	RUN_TEST_CASE(btree, btree_search);
	RUN_TEST_CASE(btree, btree_insert);
	RUN_TEST_CASE(btree, btree_erase);
	
	RUN_TEST_CASE(btree, btree_all);
}

