#ifdef DEBUG
	#undef DEBUG_X
	#ifdef DLL_HEADER_SOURCE
		#define DEBUG_X(fName, ret, ...) ret fName(__VA_ARGS__);
	#else
		#define DEBUG_X(fName, ret, ...) ret (*fName)(__VA_ARGS__);
	#endif
#endif

FUNCTIONS

#undef DEBUG_X
#define DEBUG_X(fName, ret, ...) ret (*fName)(__VA_ARGS__);

