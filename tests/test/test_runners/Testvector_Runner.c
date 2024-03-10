#include "external/unity.h"
#include "external/unity_memory.h"
#include "external/unity_fixture.h"

TEST_GROUP_RUNNER(vector)
{
	RUN_TEST_CASE(vector, vector_init);
	RUN_TEST_CASE(vector, vector_push_back);
	RUN_TEST_CASE(vector, vector_front);
	RUN_TEST_CASE(vector, vector_back);
	RUN_TEST_CASE(vector, vector_at);
	RUN_TEST_CASE(vector, vector_reserve);
	RUN_TEST_CASE(vector, vector_clone);
	RUN_TEST_CASE(vector, vector_swap);
	RUN_TEST_CASE(vector, vector_assign_at);
	RUN_TEST_CASE(vector, vector_insert_at);
	RUN_TEST_CASE(vector, vector_erase_at);
	RUN_TEST_CASE(vector, vector_pop_back);
	RUN_TEST_CASE(vector, vector_clear);
	RUN_TEST_CASE(vector, vector_sort);
}

