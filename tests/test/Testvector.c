#include <stdlib.h>
#include <string.h>

#include "vector/vector.h"
#include "external/unity.h"
#include "external/unity_memory.h"
#include "external/unity_fixture.h"

/* TODO: fix this mess. Most "unit" tests actually test much more than their functions */

const void* string_allocator(const void *el, u8 *vecData)
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

struct SomeStruct
{
	char *requiredString;
	char *optionalString;
	struct SomeStruct *anotherStruct;
};

const void* some_struct_allocator(const void *el, u8 *vecData)
{
	if(el){
		struct SomeStruct *elStruct = *(struct SomeStruct**)el;
		struct SomeStruct *newStruct = calloc(1, sizeof *elStruct);
		if(!newStruct)
			goto no_memory_free;

		struct SomeStruct *currentElStruct = elStruct;
		struct SomeStruct *currentNewStruct = newStruct;

		while(true){
			usize requiredStrSz = strlen(currentElStruct->requiredString);
			currentNewStruct->requiredString = malloc(requiredStrSz + 1);
			if(!currentNewStruct->requiredString)
				goto no_memory_free;
			strncpy(currentNewStruct->requiredString, currentElStruct->requiredString, requiredStrSz + 1);
			currentNewStruct->requiredString[requiredStrSz] = '\0';

			if(currentElStruct->optionalString){
				usize optionalStrSz = strlen(currentElStruct->optionalString);
				currentNewStruct->optionalString = malloc(optionalStrSz + 1);
				if(!currentNewStruct->optionalString)
					goto no_memory_free;
				strncpy(currentNewStruct->optionalString, currentElStruct->optionalString, optionalStrSz + 1);
				currentNewStruct->optionalString[optionalStrSz] = '\0';
			}
			
			currentElStruct = currentElStruct->anotherStruct;
			if(!currentElStruct){
				currentNewStruct->anotherStruct = nullptr;
				break;
			}
			currentNewStruct->anotherStruct = malloc(sizeof *elStruct);
			currentNewStruct = currentNewStruct->anotherStruct;

			if(!currentNewStruct)
				goto no_memory_free;
		}

		memcpy(vecData, &newStruct, sizeof elStruct);
		return newStruct;
no_memory_free:
		if(newStruct){
			currentNewStruct = newStruct;
			do{
				free(currentNewStruct->requiredString);
				free(currentNewStruct->optionalString);
				struct SomeStruct *tmp = currentNewStruct;
				currentNewStruct = currentNewStruct->anotherStruct;
				free(tmp);
			} while(currentNewStruct);
		}
		return nullptr;
	}
	else{
		struct SomeStruct *toFree;
		memcpy(&toFree, vecData, sizeof toFree);
		struct SomeStruct *currentStruct = toFree;

		do{
			free(currentStruct->requiredString);
			free(currentStruct->optionalString);
			struct SomeStruct *tmp = currentStruct;
			currentStruct = currentStruct->anotherStruct;
			free(tmp);
		} while(currentStruct);

		return nullptr;
	}
}

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
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(true, vector_empty(vec));
	TEST_ASSERT_EQUAL(vectorInitialSize, vector_capacity(vec));
	TEST_ASSERT_EQUAL(vecDataSize, vector_element_sz(vec));
	vector_uninit(nullptr);
	vector_uninit(vec);

	UnityMalloc_MakeMallocFailAfterCount(0);
	vec = vector_init(vecDataSize);
	TEST_ASSERT_NULL(vec);

	UnityMalloc_MakeMallocFailAfterCount(1);
	vec = vector_init(vecDataSize);
	TEST_ASSERT_NULL(vec);
}

TEST(vector, vector_push_back)
{ /* 100% coverage */
	Vector *vec = vector_init(sizeof(usize));
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	vector_push_back(vec, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullInputData, vector_get_last_error(vec));

	for(usize i = 0; i < vectorInitialSize; ++i){
		u32 vectorPreviousSize = vector_size(vec);
		vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
		TEST_ASSERT_EQUAL(vectorPreviousSize + 1, vector_size(vec));
	}

	UnityMalloc_MakeMallocFailAfterCount(0);
	usize n = vectorInitialSize + 1;
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vec));
	UnityMalloc_MakeMallocFailAfterCount(-1); /* To make it so malloc doesn't fail (intentionally) anymore */

	u32 vectorPreviousCapacity = vector_capacity(vec);
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousCapacity * 2, vector_capacity(vec));

	vector_uninit(vec);
}

TEST(vector, vector_front)
{ /* 100% coverage */
	usize n;
	Vector *vec = vector_init(sizeof n);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	usize *data = vector_front(vec);
	TEST_ASSERT_NULL(data);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, vector_get_last_error(vec));

	n = 1;
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	n = 2;
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	n = 3;
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	data = vector_front(vec);
	TEST_ASSERT_NOT_NULL(data);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(1, *data);

	vector_uninit(vec);
}

TEST(vector, vector_back)
{ /* 100% coverage */
	usize n;
	Vector *vec = vector_init(sizeof n);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	usize *data = vector_back(vec);
	TEST_ASSERT_NULL(data);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, vector_get_last_error(vec));

	n = 1;
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	n = 2;
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	n = 3;
	vector_push_back(vec, &n);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	data = vector_back(vec);
	TEST_ASSERT_NOT_NULL(data);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(3, *data);

	vector_uninit(vec);
}

TEST(vector, vector_at)
{ /* 100% coverage */
	u32 randValues[vectorInitialSize + 1];

	Vector *vec = vector_init(sizeof *randValues);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		randValues[i] = rand();
		vector_push_back(vec, &randValues[i]);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	u32 *outData = vector_at(vec, vectorInitialSize + 2);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, vector_get_last_error(vec));
	TEST_ASSERT_NULL(outData);

	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		outData = vector_at(vec, i);
		TEST_ASSERT_NOT_NULL(outData);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
		TEST_ASSERT_EQUAL(randValues[i], *outData);
	}

	vector_uninit(vec);
}

TEST(vector, vector_reserve)
{ /* 100% coverage */
	constexpr usize vecDataSize = sizeof(u8);
	Vector *vec = vector_init(vecDataSize);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	UnityMalloc_MakeMallocFailAfterCount(0);
	usize vectorPreviousCapacity = vector_capacity(vec);
	vector_reserve(vec, vectorPreviousCapacity * 2);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousCapacity, vector_capacity(vec));
	UnityMalloc_MakeMallocFailAfterCount(-1);

	vectorPreviousCapacity = vector_capacity(vec);
	vector_reserve(vec, vector_capacity(vec) * 2);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousCapacity * 2, vector_capacity(vec));

	vectorPreviousCapacity = vector_capacity(vec);
	vector_reserve(vec, vectorPreviousCapacity / 2);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousCapacity, vector_capacity(vec));

	vector_uninit(vec);
}

TEST(vector, vector_clone)
{ /* 100% coverage */
	u32 randValues[vectorInitialSize + 1];

	Vector *vecA = vector_init(sizeof *randValues);
	TEST_ASSERT_NOT_NULL(vecA);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecA));

	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		randValues[i] = rand();
		vector_push_back(vecA, &randValues[i]);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecA));
	}

	Vector *vecB = vector_clone(vecA);
	TEST_ASSERT_NOT_NULL(vecB);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecA));
	TEST_ASSERT_NOT_EQUAL(vector_get_data(vecA), vector_get_data(vecB));

	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		u32 *outData = vector_at(vecB, i);
		TEST_ASSERT_NOT_NULL(outData);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecB));
		TEST_ASSERT_EQUAL(randValues[i], *outData);
	}

	UnityMalloc_MakeMallocFailAfterCount(0);
	Vector *vecC = vector_clone(vecB);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vecB));
	TEST_ASSERT_NULL(vecC);

	UnityMalloc_MakeMallocFailAfterCount(1);
	vecC = vector_clone(vecB);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vecB));
	TEST_ASSERT_NULL(vecC);

	char *someRequiredString = "Some required string value";
	char *someOptionalString = "Some optional string value";

	usize someRequiredStringSz = strlen(someRequiredString);
	usize someOptionalStringSz = strlen(someOptionalString);

	struct SomeStruct someInstance = {
		malloc(someRequiredStringSz + 1),
		malloc(someOptionalStringSz + 1),
		nullptr
	};

	strcpy(someInstance.requiredString, someRequiredString);
	strcpy(someInstance.optionalString, someOptionalString);


	someRequiredString = "Some required string value 2";

	someRequiredStringSz = strlen(someRequiredString);

	struct SomeStruct someOtherInstance = {
		malloc(someRequiredStringSz + 1),
		nullptr,
		&someInstance
	};

	strcpy(someOtherInstance.requiredString, someRequiredString);

	Vector *vecD = vector_init_with_allocator(sizeof &someInstance, some_struct_allocator);

	vecC = vector_clone(vecD);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecD));
	TEST_ASSERT_NOT_NULL(vecC);
	vector_uninit(vecC);

	struct SomeStruct *someInstancePtr = &someOtherInstance;
	vector_push_back(vecD, &someInstancePtr);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecD));
	vector_push_back(vecD, &someInstancePtr);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecD));

	UnityMalloc_MakeMallocFailAfterCount(0);
	vector_push_back(vecD, &someInstancePtr);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vecD));
	vector_insert_at(vecD, 1, &someInstancePtr);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vecD));
	UnityMalloc_MakeMallocFailAfterCount(-1);

	struct SomeStruct *instanceFromVec = *(struct SomeStruct**)vector_front(vecD);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecD));
	TEST_ASSERT_TRUE(someOtherInstance.requiredString != instanceFromVec->requiredString);
	TEST_ASSERT_NULL(someOtherInstance.optionalString);
	TEST_ASSERT_TRUE(someOtherInstance.anotherStruct != instanceFromVec->anotherStruct);
	TEST_ASSERT_TRUE(someOtherInstance.anotherStruct->requiredString != instanceFromVec->anotherStruct->requiredString);
	TEST_ASSERT_TRUE(someOtherInstance.anotherStruct->optionalString != instanceFromVec->anotherStruct->optionalString);
	TEST_ASSERT_NULL(instanceFromVec->anotherStruct->anotherStruct);
	free(someOtherInstance.anotherStruct->requiredString);
	free(someOtherInstance.anotherStruct->optionalString);
	free(someOtherInstance.requiredString);

	UnityMalloc_MakeMallocFailAfterCount(7);
	vecC = vector_clone(vecD);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vecB));
	TEST_ASSERT_NULL(vecC);

	UnityMalloc_MakeMallocFailAfterCount(-1);
	vector_erase_at(vecD, 0);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecD));
	vector_clear(vecD);

	vector_uninit(vecA);
	vector_uninit(vecB);
	vector_uninit(vecD);
}

TEST(vector, vector_swap)
{ /* 100% coverage */
	u32 randValues[vectorInitialSize + 1];

	Vector *vecA = vector_init(sizeof *randValues);
	TEST_ASSERT_NOT_NULL(vecA);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecA));

	for(u32 i = 0; i < vectorInitialSize + 1; ++i){
		randValues[i] = rand();
		vector_push_back(vecA, &randValues[i]);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecA));
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
	TEST_ASSERT_NOT_EQUAL(vector_element_sz(vecA), vector_element_sz(vecB));
	TEST_ASSERT_NOT_EQUAL(vector_capacity(vecA), vector_capacity(vecB));

	TEST_ASSERT_EQUAL(true, vector_empty(vecA));
	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		u32 *outData = vector_at(vecB, i);
		TEST_ASSERT_NOT_NULL(outData);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vecB));
		TEST_ASSERT_EQUAL(randValues[i], *outData);
	}

	vector_uninit(vecA);
	vector_uninit(vecB);
}

TEST(vector, vector_assign_at)
{ /* TODO: 100% coverage */
	u32 knownValue, *knownValueFromVector;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	for(usize i = 0; i < vectorInitialSize + 1; ++i){
		vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	vector_assign_at(vec, 0, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullInputData, vector_get_last_error(vec));

	knownValue = vectorInitialSize + 767;
	vector_assign_at(vec, vectorInitialSize * 2, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, vector_get_last_error(vec));

	vector_assign_at(vec, 7, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	knownValueFromVector = vector_at(vec, 7);
	TEST_ASSERT_NOT_NULL(knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(knownValue, *knownValueFromVector);

	knownValue = 768 - vectorInitialSize;
	vector_assign_at(vec, vectorInitialSize + 1, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	knownValueFromVector = vector_at(vec, vectorInitialSize + 1);
	TEST_ASSERT_NOT_NULL(knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(knownValue, *knownValueFromVector);

	knownValueFromVector = vector_at(vec, vectorInitialSize);
	TEST_ASSERT_NOT_NULL(knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorInitialSize, *knownValueFromVector); /* Checking that previous value is still the same */

	++knownValue;
	vector_assign_at(vec, vectorInitialSize + 3, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	
	knownValueFromVector = vector_at(vec, vectorInitialSize + 3);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(knownValue, *knownValueFromVector);

	knownValueFromVector = vector_at(vec, vectorInitialSize + 2);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(0, *knownValueFromVector); /* Checking that this (in-between values) was zero-initialized */

	vector_uninit(vec);
}

TEST(vector, vector_insert_at)
{ /* 100% coverage */
	usize knownValue;
	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	vector_insert_at(vec, 0, nullptr);
	TEST_ASSERT_EQUAL(vectorErrorNullInputData, vector_get_last_error(vec));

	knownValue = 0;
	vector_insert_at(vec, 1, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, vector_get_last_error(vec));

	for(usize i = 0; i < vectorInitialSize; ++i){
		u32 vectorPreviousSize = vector_size(vec);
		vector_insert_at(vec, i, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
		TEST_ASSERT_EQUAL(vectorPreviousSize + 1, vector_size(vec));
	}

	UnityMalloc_MakeMallocFailAfterCount(0);
	knownValue = vectorInitialSize + 1;
	vector_insert_at(vec, 0, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vec));
	UnityMalloc_MakeMallocFailAfterCount(-1);

	u32 vectorPreviousCapacity = vector_capacity(vec);
	vector_insert_at(vec, 0, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousCapacity * 2, vector_capacity(vec));

	typeof(knownValue) *knownValueFromVector;
	knownValueFromVector = vector_front(vec);
	TEST_ASSERT_NOT_NULL(knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(knownValue, *knownValueFromVector);
	
	knownValueFromVector = vector_at(vec, 1);
	TEST_ASSERT_NOT_NULL(knownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(0, *knownValueFromVector);

	vector_uninit(vec);
}

TEST(vector, vector_erase_at)
{ /* 100% coverage */
	usize knownValue, *knownValueFromVector;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	vector_erase_at(vec, 0);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, vector_get_last_error(vec));

	knownValue = 767;
	vector_push_back(vec, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	vector_erase_at(vec, 0);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	for(usize i = 0; i < vectorInitialSize * 2; ++i){
		vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	vector_erase_at(vec, vectorInitialSize * 2);
	TEST_ASSERT_EQUAL(vectorErrorOutOfBounds, vector_get_last_error(vec));

	for(usize i = 0; i < (vectorInitialSize * 6) / 4; ++i){
		vector_erase_at(vec, 0);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	u32 vectorPreviousSize = vector_size(vec);
	UnityMalloc_MakeMallocFailAfterCount(0);
	vector_erase_at(vec, 0);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousSize, vector_size(vec));

	for(usize i = 0; i < vectorInitialSize / 4; ++i){
		knownValueFromVector = vector_at(vec, i);
		TEST_ASSERT_NOT_NULL(knownValueFromVector);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
		TEST_ASSERT_EQUAL(i + (vectorInitialSize * 6) / 4, *knownValueFromVector);
	}

	vector_erase_at(vec, 0);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	vector_uninit(vec);
}

TEST(vector, vector_pop_back)
{ /* 100% coverage */
	usize knownValue;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	vector_pop_back(vec);
	TEST_ASSERT_EQUAL(vectorErrorEmpty, vector_get_last_error(vec));

	knownValue = 767;
	vector_push_back(vec, &knownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	vector_pop_back(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	for(usize i = 0; i < vectorInitialSize * 2; ++i){
		vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	for(usize i = 0; i < (vectorInitialSize * 6) / 4; ++i){
		vector_pop_back(vec);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	u32 vectorPreviousSize = vector_size(vec);
	UnityMalloc_MakeMallocFailAfterCount(0);
	vector_pop_back(vec);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousSize, vector_size(vec));

	vector_pop_back(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	vector_uninit(vec);

	/* New test using Vector of char* */
	char *newKnownValue, **newKnownValueFromVector;
	vec = vector_init_with_allocator(sizeof newKnownValue, string_allocator);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	{
		char stackString[] = { 'S', 'o', 'm', 'e', ' ', 's', 't', 'r', 'i', 'n', 'g', '\0' };
		char *stackStringPtr = &stackString[0];
		vector_push_back(vec, &stackStringPtr);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	newKnownValueFromVector = vector_back(vec);
	TEST_ASSERT_NOT_NULL(newKnownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL_STRING("Some string", *newKnownValueFromVector);

	newKnownValue = "Some string 2";
	vector_push_back(vec, &newKnownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	newKnownValueFromVector = vector_back(vec);
	TEST_ASSERT_NOT_NULL(newKnownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL_STRING(newKnownValue, *newKnownValueFromVector);

	newKnownValue = "Some string 3";
	vector_push_back(vec, &newKnownValue);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	newKnownValueFromVector = vector_back(vec);
	TEST_ASSERT_NOT_NULL(newKnownValueFromVector);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL_STRING(newKnownValue, *newKnownValueFromVector);

	vector_pop_back(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	vector_pop_back(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	/* Intentionally left one element inside */

	vector_uninit(vec);
}

TEST(vector, vector_clear)
{ /* 100% coverage */
	usize knownValue;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	for(usize i = 0; i < vectorInitialSize * 2; ++i){
		vector_push_back(vec, &i);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	u32 vectorPreviousSize = vector_size(vec);
	UnityMalloc_MakeMallocFailAfterCount(0);
	vector_clear(vec);
	UnityMalloc_MakeMallocFailAfterCount(-1);
	TEST_ASSERT_EQUAL(vectorErrorOutOfMemory, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(vectorPreviousSize, vector_size(vec));

	vector_clear(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	TEST_ASSERT_EQUAL(0, vector_size(vec));

	vector_uninit(vec);
}

int usize_comp(const void *a, const void *b)
{
	usize usize_a = *(usize*)a;
	usize usize_b = *(usize*)b;
	if(usize_a == usize_b)
		return 0;
	return usize_a < usize_b ? -1 : 1;
}

TEST(vector, vector_sort)
{ /* 100% coverage */
	usize knownValue;

	Vector *vec = vector_init(sizeof knownValue);
	TEST_ASSERT_NOT_NULL(vec);
	TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

	for(usize i = 0; i < 767; ++i){
		usize random = rand() % 767;
		vector_push_back(vec, &random);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
	}

	vector_sort(vec, usize_comp);

	for(usize i = 1; i < 767; ++i){
		usize *outDataPrev, *outData;
		outDataPrev = vector_at(vec, i - 1);
		TEST_ASSERT_NOT_NULL(outDataPrev);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));

		outData = vector_at(vec, i);
		TEST_ASSERT_NOT_NULL(outData);
		TEST_ASSERT_EQUAL(vectorErrorSuccess, vector_get_last_error(vec));
		TEST_ASSERT_TRUE(*outDataPrev == *outData ? true : *outDataPrev < *outData);
	}

	vector_uninit(vec);
}

