#define DLL_HEADER_SOURCE
#include "str/str_internal.h"
DEBUG_DEFINE_VTABLE(str)
#undef DLL_HEADER_SOURCE

#include <stdlib.h>
#include <string.h>

String* string_init(void)
{
	return string_init_with_string(nullptr);
}

String* string_init_with_string(const char *string)
{
	String *str = malloc(sizeof *str);
	if(!str)
		return nullptr;

	str->stringSize = string ? strlen(string) + 1 : stringInitialSize;
	str->data = malloc(str->stringSize);
	if(!str->data){
		free(str);
		return nullptr;
	}

	if(string)
		strcpy(str->data, string);
	else
		str->data[0] = '\0';

	str->n = string ? str->stringSize - 1 : 0;
	str->lastError = stringErrorSuccess;

	return str;
}

void string_uninit(String *str)
{
	if(str){
		free(str->data);
		free(str);
	}
}

String* string_clone(String str[restrict static 1])
{
	String *newStr = string_init_with_string(str->data);
	if(!newStr)
		str->lastError = stringErrorOutOfMemory;

	return newStr;
}

void string_swap(String strA[restrict static 1], String strB[restrict static 1])
{
	String tmp = *strA;
	*strA = *strB;
	*strB = tmp;
}

const char* string_at(String str[restrict static 1], const usize index)
{
	if(index >= string_size(str)){
		str->lastError = stringErrorOutOfBounds;
		return nullptr;
	}

	str->lastError = stringErrorSuccess;
	return str->data + index;
}

const char* string_front(String str[restrict static 1])
{
	if(string_empty(str)){
		str->lastError = stringErrorEmpty;
		return nullptr;
	}

	str->lastError = stringErrorSuccess;
	return str->data;
}

const char* string_back(String str[restrict static 1])
{
	if(string_empty(str)){
		str->lastError = stringErrorEmpty;
		return nullptr;
	}

	str->lastError = stringErrorSuccess;
	return str->data + string_size(str) - 1;
}

usize string_find(String str[restrict static 1], const char *subStr)
{
	return string_find_from(str, subStr, 0);
}

usize string_find_from(String str[restrict static 1], const char *subStr, const usize startIndex)
{
	if(string_empty(str)){
		str->lastError = stringErrorEmpty;
		return 0; /* Hmmm, maybe the return should signify failure? */
	}

	if(!subStr){
		str->lastError = stringErrorNullStrInput;
		return 0; /* Same as above */
	}

	if(startIndex >= string_size(str)){
		str->lastError = stringErrorOutOfBounds;
		return 0; /* Same as above */
	}

	usize subStrLen = strlen(subStr);
	if(subStrLen == 0){
		str->lastError = stringErrorEmptyStrInput;
		return 0; /* Same as above */
	}

	const char lastSubStrCh = subStr[subStrLen - 1];
	for(usize cursor = startIndex; cursor + subStrLen <= string_size(str); ++cursor){
		/* Test last character of subStr before trying the entire subStr */
		if(str->data[cursor + subStrLen - 1] == lastSubStrCh && strncmp(&str->data[cursor], subStr, subStrLen) == 0){
			str->lastError = stringErrorSuccess;
			return cursor;
		}
	}

	str->lastError = stringErrorStrInputNotFound;
	return 0; /* Same as above */
}

int string_comp(String strA[restrict const static 1], String strB[restrict const static 1])
{
	return strcmp(string_get_data(strA), string_get_data(strB));
}

bool string_starts_with(String str[restrict const static 1], const char *subStr)
{
	if(string_empty(str) || !subStr)
		return false;

	return strcmp(str->data, subStr) == 0;
}

bool string_ends_with(String str[restrict const static 1], const char *subStr)
{
	if(string_empty(str) || !subStr)
		return false;

	usize subStrLen = strlen(subStr);
	if(string_size(str) < subStrLen)
		return false;

	return strcmp(str->data + string_size(str) - subStrLen, subStr) == 0;
}

bool string_contains(String str[restrict const static 1], const char *subStr)
{
	if(string_empty(str) || !subStr)
		return false;

	usize subStrLen = strlen(subStr);
	if(string_size(str) < subStrLen)
		return false;

	const char lastSubStrCh = subStr[subStrLen - 1];
	for(usize cursor = 0; cursor + subStrLen <= string_size(str); ++cursor)
		/* Test last character of subStr before trying the entire subStr */
		if(str->data[cursor + subStrLen - 1] == lastSubStrCh && strncmp(&str->data[cursor], subStr, subStrLen) == 0)
			return true;

	return false;
}

void string_reserve(String str[static 1], u32 reserveAmount)
{
	if(reserveAmount > string_capacity(str)){
		void *tmp = realloc(str->data, reserveAmount);
		if(!tmp){
			str->lastError = stringErrorOutOfMemory;
			return;
		}
		str->stringSize = reserveAmount;
		str->data = tmp;
	}

	str->lastError = stringErrorSuccess;
}

void string_push_back(String str[static 1], const char ch)
{
	if(string_size(str) + 1 == string_capacity(str)){
		void *tmp = realloc(str->data, 2 * string_capacity(str));
		if(!tmp){
			str->lastError = stringErrorOutOfMemory;
			return;
		}
		str->stringSize *= 2;
		str->data = tmp;
	}

	str->data[str->n++] = ch;
	str->data[str->n] = '\0';

	str->lastError = stringErrorSuccess;
}

void string_append(String str[static 1], const char *appendedStr)
{
	if(!appendedStr){
		str->lastError = stringErrorNullStrInput;
		return;
	}

	const usize appendedStrLen = strlen(appendedStr);

	if(string_size(str) + appendedStrLen + 1 >= string_capacity(str)){
		usize newSize = _usize_bit_ceil(string_size(str) + appendedStrLen + 1);
		void *tmp = realloc(str->data, newSize);
		if(!tmp){
			str->lastError = stringErrorOutOfMemory;
			return;
		}
		str->stringSize = newSize;
		str->data = tmp;
	}
	
	memmove(str->data + string_size(str), appendedStr, appendedStrLen);
	str->n += appendedStrLen;
	str->data[str->n] = '\0';

	str->lastError = stringErrorSuccess;
}

void string_assign(String str[restrict static 1], const char *newStr)
{
	if(!newStr){
		str->lastError = stringErrorNullStrInput;
		return;
	}

	const usize assignedStrLen = strlen(newStr) + 1;
	char *tmp = realloc(str->data, assignedStrLen);
	if(!tmp){
		str->lastError = stringErrorOutOfMemory;
		return;
	}

	str->data = tmp;
	strcpy(str->data, newStr);

	str->stringSize = assignedStrLen;
	str->n = str->stringSize - 1;

	str->lastError = stringErrorSuccess;
}

void string_assign_at(String str[restrict static 1], const usize index, const char ch)
{
	if(index >= string_size(str)){ /* Don't allow overwriting '\0' */
		str->lastError = stringErrorOutOfBounds;
		return;
	}

	str->data[index] = ch;

	str->lastError = stringErrorSuccess;
}

void string_insert_at(String str[restrict static 1], const usize index, const char *insertedStr)
{
	if(!insertedStr){
		str->lastError = stringErrorNullStrInput;
		return;
	}

	if(index > string_size(str)){
		str->lastError = stringErrorOutOfBounds;
		return;
	}

	const usize insertedStrLen = strlen(insertedStr);

	if(insertedStrLen == 0){
		str->lastError = stringErrorSuccess;
		return;
	}

	if(string_size(str) + insertedStrLen + 1 >= string_capacity(str)){
		usize newSize = _usize_bit_ceil(string_size(str) + insertedStrLen + 1);
		void *tmp = realloc(str->data, newSize);
		if(!tmp){
			str->lastError = stringErrorOutOfMemory;
			return;
		}
		str->stringSize = newSize;
		str->data = tmp;
	}
	
	memmove(str->data + index + insertedStrLen, str->data + index, string_size(str) - index);
	memmove(str->data + index, insertedStr, insertedStrLen);
	str->n += insertedStrLen;

	str->lastError = stringErrorSuccess;
}

void string_replace(String str[restrict static 1], const char *subStr, const char *replacer)
{
	if(!subStr || !replacer){
		str->lastError = stringErrorNullStrInput;
		return;
	}

	const usize subStrLen = strlen(subStr);
	const usize replacerLen = strlen(replacer);

	if(subStrLen == 0){
		str->lastError = stringErrorSuccess;
		return;
	}

	const char lastSubStrCh = subStr[subStrLen - 1];
	const usize originalStringLen = string_size(str);

	usize readCursor = 0, n = 0;
	/* Count first the number of occurrences */
	while(readCursor + subStrLen <= originalStringLen){
		/* Test last character of subStr before trying the entire subStr */
		if(str->data[readCursor + subStrLen - 1] == lastSubStrCh && strncmp(&str->data[readCursor], subStr, subStrLen) == 0){
			readCursor += subStrLen;
			++n;
		}
		else
			++readCursor;
	}

	String *tmpStr = string_init();
	if(!tmpStr){
		str->lastError = stringErrorOutOfMemory;
		return;
	}

	const usize finalStringLen = (originalStringLen - n * subStrLen) + n * replacerLen;
	string_reserve(tmpStr, finalStringLen + 1);
	if(string_get_last_error(tmpStr) != stringErrorSuccess){
		str->lastError = string_get_last_error(tmpStr);
		string_uninit(tmpStr);
		return;
	}

	usize writeCursor = 0;
	readCursor = 0;
	while(readCursor + subStrLen <= originalStringLen){
		/* Test last character of subStr before trying the entire subStr */
		if(str->data[readCursor + subStrLen - 1] == lastSubStrCh && strncmp(&str->data[readCursor], subStr, subStrLen) == 0){
			memmove(tmpStr->data + writeCursor, replacer, replacerLen);
			writeCursor += replacerLen;
			readCursor += subStrLen;
		}
		else{
			tmpStr->data[writeCursor] = str->data[readCursor];
			++writeCursor;
			++readCursor;
		}
	}
	memmove(tmpStr->data + writeCursor, str->data + readCursor, originalStringLen - readCursor);
	tmpStr->n = finalStringLen;
	tmpStr->data[finalStringLen] = '\0';

	string_swap(str, tmpStr);
	string_uninit(tmpStr);

	str->lastError = stringErrorSuccess;
}

void string_erase_at(String str[static 1], const usize startIndex, const usize amount)
{
	if(string_empty(str)){
		str->lastError = stringErrorEmpty;
		return;
	}

	if(startIndex + amount > string_size(str)){
		str->lastError = stringErrorOutOfBounds;
		return;
	}

	memmove(str->data + startIndex, str->data + startIndex + amount, string_size(str) - (amount + startIndex));
	str->n -= amount;
	str->data[str->n] = '\0';

	/* NOTE: maybe reduce size? */

	str->lastError = stringErrorSuccess;
}

void string_pop_back(String str[static 1])
{
	if(string_empty(str)){
		str->lastError = stringErrorEmpty;
		return;
	}

	if(string_capacity(str) > stringInitialSize && string_size(str) + 1 < string_capacity(str) / 4){
		void *tmp = realloc(str->data, string_capacity(str) / 2);
		if(!tmp){
			++str->n;
			str->lastError = stringErrorOutOfMemory; /* Same reason as string_erase_at */
			return;
		}
		str->stringSize /= 2;
		str->data = tmp;
	}

	str->data[--str->n] = '\0';

	str->lastError = stringErrorSuccess;
}

void string_clear(String str[static 1])
{
	str->n = 0;
	str->lastError = stringErrorSuccess;
}


