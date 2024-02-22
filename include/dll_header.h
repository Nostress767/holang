#ifdef DEBUG
	#define DEBUG_DECLARE_VTABLE(name) \
		typedef struct\
		{\
			FUNCTIONS\
		} VTable##name;
	#define DEBUG_DEFINE_VTABLE(name) \
		volatile VTable##name vTable##name =\
		{\
			FUNCTIONS\
		};
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#define DEBUG_DLOPEN(dir, libName) \
			HMODULE libName##debugHandle;\
			struct\
			{\
				union\
				{\
					VTable##libName *obj;\
					void (*temp)();\
				};\
			} vTable##libName;\
			do\
			{\
				libName##debugHandle = LoadLibraryA("obj/debug/src/" dir #libName ".dll");\
				vTable##libName.temp = (void(*)())GetProcAddress(libName##debugHandle, "vTable" #libName);\
			} while(0)
		#define DEBUG_DLSYM(libName, symName) \
			do\
			{\
				symName = vTable##libName.obj->symName;\
			} while(0)
		#define DEBUG_DLCLOSE(libName) \
			do\
			{\
				FreeLibrary(libName##debugHandle);\
			} while(0)
	#else /* !defined(_WIN32) */
		#include <dlfcn.h>
		#define DEBUG_DLOPEN(dir, libName) \
			void* libName##debugHandle;\
			VTable##libName *vTable##libName;\
			do\
			{\
				libName##debugHandle = dlopen("obj/debug/src/" dir #libName ".dll", RTLD_NOW);\
				vTable##libName = (VTable##libName*)dlsym(libName##debugHandle, "vTable" #libName);\
			} while(0)
		#define DEBUG_DLSYM(libName, symName) \
			do\
			{\
				symName = vTable##libName->symName;\
			} while(0)
		#define DEBUG_DLCLOSE(libName) \
			do\
			{\
				dlclose(libName##debugHandle);\
			} while(0)
	#endif /* _WIN32 */
#else /* !defined(DEBUG) */
	#undef DEBUG_X
	#define DEBUG_X(fName, ret, ...) ret fName(__VA_ARGS__);
	#define DEBUG_DECLARE_VTABLE(name)
	#define DEBUG_DEFINE_VTABLE(name)
	#define DEBUG_DLOPEN(dir, libName) while(0)
	#define DEBUG_DLSYM(libName, symName) while(0)
	#define DEBUG_DLCLOSE(libName) while(0)
#endif /* DEBUG */

