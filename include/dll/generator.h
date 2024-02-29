#undef DLL_X
#ifdef DEBUG
	#define DEBUG_DECLARE_VTABLE(name) \
		typedef struct\
		{\
			DLL_FUNCTIONS\
		} VTable##name;
	#ifdef DLL_HEADER_SOURCE
		/* DEBUG_DEFINE_VTABLE should only be used on source files */
		#define DEBUG_DEFINE_VTABLE(name) \
			volatile VTable##name vTable##name =\
			{\
				DLL_FUNCTIONS\
			};
		#define DLL_X(fName, ret, ...) ret fName(__VA_ARGS__);
	#else
		#define DLL_X(fName, ret, ...) ret (*fName)(__VA_ARGS__);
	#endif
#else
	#define DLL_X(fName, ret, ...) ret fName(__VA_ARGS__);
	#define DEBUG_DECLARE_VTABLE(name)
	#define DEBUG_DEFINE_VTABLE(name)
#endif

DLL_FUNCTIONS

#undef DLL_X
#define DLL_X(fName, ret, ...) ret (*fName)(__VA_ARGS__);

