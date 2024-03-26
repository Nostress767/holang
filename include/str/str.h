#pragma once

#include "types.h"

constexpr usize stringInitialSize = 1 << 7;

enum StringError : u32
{
	stringErrorOutOfMemory,
	stringErrorSuccess,
	stringErrorOutOfBounds,
	stringErrorEmpty,
	stringErrorNullStrInput,
	stringErrorEmptyStrInput,
	stringErrorStrInputNotFound,
};

typedef struct String
{
	usize stringSize; 	    /* Internal string size   */
	char *data;	  	    /* Raw data of characters */
	u32 n;		  	    /* Amount of characters   */
	enum StringError lastError; /* Last occured error     */
} String;

#undef DLL_FUNCTIONS
#define DLL_FUNCTIONS \
DLL_X(string_init    , String*, void)\
DLL_X(string_init_with_string    , String*, const char *string)\
DLL_X(string_uninit  , void, String *restrict str)\
DLL_X(string_clone   , String*, String str[restrict static 1])\
DLL_X(string_swap    , void, String strA[restrict static 1], String strB[restrict static 1])\
\
DLL_X(string_at            , const char*, String str[restrict static 1], const usize index)\
DLL_X(string_front         , const char*, String str[restrict static 1])\
DLL_X(string_back          , const char*, String str[restrict static 1])\
\
DLL_X(string_find          , usize, String str[restrict static 1], const char *subStr)\
DLL_X(string_find_from          , usize, String str[restrict static 1], const char *subStr, const usize startIndex)\
DLL_X(string_comp          , int, String strA[restrict const static 1], String strB[restrict const static 1])\
DLL_X(string_starts_with          , bool, String str[restrict const static 1], const char *subStr)\
DLL_X(string_ends_with          , bool, String str[restrict const static 1], const char *subStr)\
DLL_X(string_contains          , bool, String str[restrict const static 1], const char *subStr)\
\
DLL_X(string_reserve    , void, String str[restrict static 1], u32 reserveSize)\
\
DLL_X(string_push_back, void, String str[restrict static 1], const char ch)\
DLL_X(string_append, void, String str[restrict static 1], const char *appendedStr)\
DLL_X(string_assign, void, String str[restrict static 1], const char *newStr)\
DLL_X(string_assign_at, void, String str[restrict static 1], const usize index, const char ch)\
DLL_X(string_insert_at, void, String str[restrict static 1], const usize index, const char *insertedStr)\
DLL_X(string_replace, void, String str[restrict static 1], const char *subStr, const char *replacer)\
DLL_X(string_erase_at , void, String str[restrict static 1], const usize startIndex, const usize amount)\
DLL_X(string_pop_back , void, String str[restrict static 1])\
DLL_X(string_clear    , void, String str[restrict static 1])

static inline const char* string_get_data(String str[restrict const static 1])
{
	return str->data;
}

static inline char* string_get_mut_data(String str[restrict const static 1])
{
	return str->data;
}

static inline enum StringError string_get_last_error(String str[restrict const static 1])
{
	return str->lastError;
}

static inline bool string_empty(String str[restrict const static 1])
{
	return str->n == 0;
}

static inline u32 string_size(String str[restrict const static 1])
{
	return str->n;
}

static inline u32 string_length(String str[restrict const static 1])
{
	return str->n;
}

static inline usize string_capacity(String str[restrict const static 1])
{
	return str->stringSize;
}

#include "dll/generator.h"
DEBUG_DECLARE_VTABLE(str)
#include "dll/plugger.h"


