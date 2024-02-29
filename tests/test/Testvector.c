#include <stdlib.h>

#include "vector/vector.h"
#include "external/unity.h"
#include "external/unity_memory.h"
#include "external/unity_fixture.h"

TEST_GROUP(vector);

TEST_SETUP(vector)
{
	UnityMalloc_StartTest();
}

TEST_TEAR_DOWN(vector)
{
	UnityMalloc_EndTest();
}

TEST(vector, vector_init)
{ /* 100% coverage */
	constexpr usize vecDataSize = sizeof(u8);
	Vector *vec = vector_init(vecDataSize);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(true, vector_empty(vec));
	TEST_ASSERT_EQUAL(vectorInitialSize, vector_capacity(vec));
	TEST_ASSERT_EQUAL(vecDataSize, vector_struct_size(vec));
	vector_uninit(vec);

	UnityMalloc_MakeMallocFailAfterCount(1);
	vec = vector_init(vecDataSize);
	TEST_ASSERT_NULL(vec);

	UnityMalloc_MakeMallocFailAfterCount(0);
	vec = vector_init(vecDataSize);
	TEST_ASSERT_NULL(vec);
}

TEST(vector, vector_push_back)
{ /* 100% coverage */
	Vector *vec = vector_init(sizeof(usize));
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e = vector_push_back(vec, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullInputData, e);

	for(usize i = 0; i < vectorInitialSize; ++i){
		u32 vectorPreviousSize = vector_size(vec);
		e = vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
		TEST_ASSERT_EQUAL(vectorPreviousSize + 1, vector_size(vec));
	}

	UnityMalloc_MakeMallocFailAfterCount(0);
	usize n = vectorInitialSize + 1;
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, e);
	UnityMalloc_MakeMallocFailAfterCount(-1); /* To make it so malloc doesn't fail (intentionally) anymore */

	u32 vectorPreviousCapacity = vector_capacity(vec);
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(vectorPreviousCapacity * 2, vector_capacity(vec));

	vector_uninit(vec);
}

TEST(vector, vector_front)
{ /* 100% coverage */
	usize n;
	Vector *vec = vector_init(sizeof n);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e = vector_front(vec, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullOutputData, e);

	e = vector_front(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, e);

	n = 1;
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);

	n = 2;
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);

	n = 3;
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);

	e = vector_front(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(1, n);

	vector_uninit(vec);
}

TEST(vector, vector_back)
{ /* 100% coverage */
	usize n;
	Vector *vec = vector_init(sizeof n);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e = vector_back(vec, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullOutputData, e);

	e = vector_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, e);

	n = 1;
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);

	n = 2;
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);

	n = 3;
	e = vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);

	e = vector_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(3, n);

	vector_uninit(vec);
}

TEST(vector, vector_at)
{ /* 100% coverage */
	u32 randValues[vectorInitialSize + 1];

	Vector *vec = vector_init(sizeof *randValues);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e;
	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		randValues[i] = rand();
		e = vector_push_back(vec, &randValues[i]);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	e = vector_at(vec, 0, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullOutputData, e);

	u32 outData;
	e = vector_at(vec, vectorInitialSize + 2, &outData);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, e);

	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		e = vector_at(vec, i, &outData);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
		TEST_ASSERT_EQUAL(randValues[i], outData);
	}

	vector_uninit(vec);
}

TEST(vector, vector_reserve)
{ /* 100% coverage */
	constexpr usize vecDataSize = sizeof(u8);
	Vector *vec = vector_init(vecDataSize);
	TEST_ASSERT_NOT_NULL(vec);

	UnityMalloc_MakeMallocFailAfterCount(0);
	usize vectorPreviousCapacity = vector_capacity(vec);
	VectorError e = vector_reserve(vec, vectorPreviousCapacity * 2);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, e);
	TEST_ASSERT_EQUAL(vectorPreviousCapacity, vector_capacity(vec));
	UnityMalloc_MakeMallocFailAfterCount(-1);

	vectorPreviousCapacity = vector_capacity(vec);
	e = vector_reserve(vec, vector_capacity(vec) * 2);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(vectorPreviousCapacity * 2, vector_capacity(vec));

	vectorPreviousCapacity = vector_capacity(vec);
	e = vector_reserve(vec, vectorPreviousCapacity / 2);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(vectorPreviousCapacity, vector_capacity(vec));

	vector_uninit(vec);
}

TEST(vector, vector_clone)
{ /* 100% coverage */
	u32 randValues[vectorInitialSize + 1];

	Vector *vecA = vector_init(sizeof *randValues);
	TEST_ASSERT_NOT_NULL(vecA);

	VectorError e;
	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		randValues[i] = rand();
		e = vector_push_back(vecA, &randValues[i]);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	Vector *vecB = vector_clone(vecA);
	TEST_ASSERT_NOT_NULL(vecB);
	TEST_ASSERT_NOT_EQUAL(vector_get_data(vecA), vector_get_data(vecB));

	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		u32 outData;
		e = vector_at(vecB, i, &outData);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
		TEST_ASSERT_EQUAL(randValues[i], outData);
	}

	vector_uninit(vecA);
	vector_uninit(vecB);
}

TEST(vector, vector_swap)
{ /* 100% coverage */
	u32 randValues[vectorInitialSize + 1];

	Vector *vecA = vector_init(sizeof *randValues);
	TEST_ASSERT_NOT_NULL(vecA);

	VectorError e;
	for(u32 i = 0; i < vectorInitialSize + 1; ++i){
		randValues[i] = rand();
		e = vector_push_back(vecA, &randValues[i]);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	Vector *vecB = vector_init(sizeof(u16));
	TEST_ASSERT_NOT_NULL(vecB);
	TEST_ASSERT_EQUAL(true, vector_empty(vecB));

	const u8 *oldVecAData = vector_get_data(vecA);
	const u8 *oldVecBData = vector_get_data(vecB);
	vector_swap(vecA, vecB);
	TEST_ASSERT_EQUAL(oldVecAData, vector_get_data(vecB));
	TEST_ASSERT_EQUAL(oldVecBData, vector_get_data(vecA));
	TEST_ASSERT_NOT_EQUAL(vector_get_data(vecA), vector_get_data(vecB));
	TEST_ASSERT_NOT_EQUAL(vector_size(vecA), vector_size(vecB));
	TEST_ASSERT_NOT_EQUAL(vector_struct_size(vecA), vector_struct_size(vecB));
	TEST_ASSERT_NOT_EQUAL(vector_capacity(vecA), vector_capacity(vecB));

	TEST_ASSERT_EQUAL(true, vector_empty(vecA));
	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		u32 outData;
		e = vector_at(vecB, i, &outData);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
		TEST_ASSERT_EQUAL(randValues[i], outData);
	}

	vector_uninit(vecA);
	vector_uninit(vecB);
}

TEST(vector, vector_assign_at)
{ /* 100% coverage */
	u32 knownValue, knownValueFromVector;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e;
	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		e = vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	e = vector_assign_at(vec, 0, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullInputData, e);

	knownValue = vectorInitialSize + 767;
	e = vector_assign_at(vec, vectorInitialSize * 2, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, e);

	e = vector_assign_at(vec, 7, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	e = vector_at(vec, 7, &knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(knownValue, knownValueFromVector);

	knownValue = 768 - vectorInitialSize;
	e = vector_assign_at(vec, vectorInitialSize + 1, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	e = vector_at(vec, vectorInitialSize + 1, &knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(knownValue, knownValueFromVector);
	e = vector_at(vec, vectorInitialSize, &knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(vectorInitialSize, knownValueFromVector); /* Checking that previous value is still the same */

	++knownValue;
	e = vector_assign_at(vec, vectorInitialSize + 3, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	e = vector_at(vec, vectorInitialSize + 3, &knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(knownValue, knownValueFromVector);
	e = vector_at(vec, vectorInitialSize + 2, &knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(0, knownValueFromVector); /* Checking that this (in-between values) was zero-initialized */

	vector_uninit(vec);
}

TEST(vector, vector_insert_at)
{ /* 100% coverage */
	usize knownValue;
	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e = vector_insert_at(vec, 0, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullInputData, e);

	knownValue = 0;
	e = vector_insert_at(vec, 1, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, e);

	for(usize i = 0; i < vectorInitialSize; ++i){
		u32 vectorPreviousSize = vector_size(vec);
		e = vector_insert_at(vec, i, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
		TEST_ASSERT_EQUAL(vectorPreviousSize + 1, vector_size(vec));
	}

	UnityMalloc_MakeMallocFailAfterCount(0);
	knownValue = vectorInitialSize + 1;
	e = vector_insert_at(vec, 0, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, e);
	UnityMalloc_MakeMallocFailAfterCount(-1);

	u32 vectorPreviousCapacity = vector_capacity(vec);
	e = vector_insert_at(vec, 0, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(vectorPreviousCapacity * 2, vector_capacity(vec));

	typeof(knownValue) knownValueFromVector;
	e = vector_front(vec, &knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(knownValue, knownValueFromVector);
	e = vector_at(vec, 1, &knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(0, knownValueFromVector);

	vector_uninit(vec);
}

TEST(vector, vector_erase_at)
{ /* 100% coverage */
	usize knownValue, knownValueFromVector;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e = vector_erase_at(vec, 0);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, e);

	for(usize i = 0; i < vectorInitialSize * 2; ++i){
		e = vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	e = vector_erase_at(vec, vectorInitialSize * 2);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, e);

	for(usize i = 0; i < (vectorInitialSize * 6) / 4; ++i){
		e = vector_erase_at(vec, 0);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	u32 vectorPreviousSize = vector_size(vec);
	UnityMalloc_MakeMallocFailAfterCount(0);
	e = vector_erase_at(vec, 0);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, e);
	TEST_ASSERT_EQUAL(vectorPreviousSize, vector_size(vec));

	for(usize i = 0; i < vectorInitialSize / 4; ++i){
		e = vector_at(vec, i, &knownValueFromVector);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
		TEST_ASSERT_EQUAL(i + (vectorInitialSize * 6) / 4, knownValueFromVector);
	}

	vector_uninit(vec);
}

TEST(vector, vector_pop_back)
{ /* 100% coverage */
	usize knownValue;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e = vector_pop_back(vec);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, e);

	for(usize i = 0; i < vectorInitialSize * 2; ++i){
		e = vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	for(usize i = 0; i < (vectorInitialSize * 6) / 4; ++i){
		e = vector_pop_back(vec);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	u32 vectorPreviousSize = vector_size(vec);
	UnityMalloc_MakeMallocFailAfterCount(0);
	e = vector_erase_at(vec, 0);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, e);
	TEST_ASSERT_EQUAL(vectorPreviousSize, vector_size(vec));

	vector_uninit(vec);
}

TEST(vector, vector_clear)
{ /* 100% coverage */
	usize knownValue;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);

	VectorError e;
	for(usize i = 0; i < vectorInitialSize * 2; ++i){
		e = vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	}

	u32 vectorPreviousSize = vector_size(vec);
	UnityMalloc_MakeMallocFailAfterCount(0);
	e = vector_clear(vec);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, e);
	TEST_ASSERT_EQUAL(vectorPreviousSize, vector_size(vec));

	e = vector_clear(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, e);
	TEST_ASSERT_EQUAL(0, vector_size(vec));

	vector_uninit(vec);
}

