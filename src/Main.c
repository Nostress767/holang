#include "log.h"
#include "vector/vector.h"
#include "btree/btree.h"

#include <stdlib.h>

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
	
	Vector vecTest = vector_init(sizeof intTest);
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
	
	DEBUG_DLCLOSE(btree);
	
	//printf("Hello, world!\n");
}