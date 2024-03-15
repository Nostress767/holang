#include "external/unity.h"
#include "external/unity_memory.h"
#include "external/unity_fixture.h"

TEST_GROUP_RUNNER(hash_table)
{
	RUN_TEST_CASE(hash_table, hash_table_half_siphash);
	RUN_TEST_CASE(hash_table, hash_table_init);
	RUN_TEST_CASE(hash_table, hash_table_insert);
	RUN_TEST_CASE(hash_table, hash_table_reserve);
	RUN_TEST_CASE(hash_table, hash_table_at);
	RUN_TEST_CASE(hash_table, hash_table_erase);
	RUN_TEST_CASE(hash_table, hash_table_clear);
}

