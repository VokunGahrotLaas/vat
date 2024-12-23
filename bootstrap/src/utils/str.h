#pragma once

// libc
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// bootstrap
#include "utils/utils.h"

#define CV_EMPTY                                                                                                       \
	(struct cv) { .data = NULL, .size = 0, }

extern char str_empty_impl[1];

struct cv
{
	char const* data;
	size_t size;
};

struct str
{
	char* data;
	size_t size;
	size_t capacity;
};

static inline struct cv cv_str(struct str const* str);
static inline struct cv cv_cstr(char const* cstr);
static inline struct cv cv_sub(struct cv cv, size_t offset, size_t size);
static inline void cv_print(struct cv cv, FILE* stream);
static inline ptrdiff_t cv_cmp(struct cv lhs, struct cv rhs);

static inline bool str_ctor(struct str* str, size_t capacity);
static inline bool str_of_cv(struct str* str, struct cv cv);
static inline void str_dtor(struct str* str);

static inline bool str_reserve(struct str* str, size_t capacity);
static inline bool str_resize(struct str* str, size_t size);
static inline bool str_pushcv(struct str* str, struct cv cv);
static inline bool str_pushc(struct str* str, char c);

bool str_copy(struct str* str, struct str const* other);
bool str_move(struct str* str, struct str* other);

// impl

static inline struct cv cv_str(struct str const* str)
{
	return (struct cv){
		.data = str->data,
		.size = str->size,
	};
}

static inline struct cv cv_cstr(char const* cstr)
{
	return (struct cv){
		.data = cstr,
		.size = cstr != NULL ? strlen(cstr) : 0,
	};
}

static inline struct cv cv_sub(struct cv cv, size_t offset, size_t size)
{
	DBG_ASSERT(offset + size <= cv.size && "cv_sub: invalid offset/size");
	return (struct cv){
		.data = size > 0 ? cv.data + offset : NULL,
		.size = size,
	};
}

static inline void cv_print(struct cv cv, FILE* stream)
{
	for (size_t i = 0; i < cv.size; ++i)
		if (isprint(cv.data[i]))
			fputc(cv.data[i], stream);
		else
			fprintf(stream, "\\x%02x", (unsigned)(unsigned char)cv.data[i]);
}

static inline ptrdiff_t cv_cmp(struct cv lhs, struct cv rhs)
{
	size_t min = lhs.size < rhs.size ? lhs.size : rhs.size;
	int r = min > 0 ? memcmp(lhs.data, rhs.data, min) : 0;
	return r != 0 ? r : (ptrdiff_t)lhs.size - (ptrdiff_t)rhs.size;
}

static inline bool str_ctor(struct str* str, size_t capacity)
{
	str->data = capacity != 0 ? malloc((capacity + 1) * sizeof(char)) : str_empty_impl;
	str->capacity = capacity;
	str->size = 0;
	return str->data != NULL;
}

static inline bool str_of_cv(struct str* str, struct cv cv)
{
	if (!str_ctor(str, cv.size)) return false;
	return str_pushcv(str, cv);
}

static inline void str_dtor(struct str* str)
{
	if (str->data != NULL && str->data != str_empty_impl) free(str->data);
	str->data = str_empty_impl;
	str->capacity = str->size = 0;
}

static inline bool str_reserve(struct str* str, size_t capacity)
{
	if (capacity <= str->capacity) return true;
	size_t new_capacity = str->capacity;
	while (new_capacity < capacity)
		new_capacity *= 2;
	void* ptr = str->data != NULL && str->data != str_empty_impl ? realloc(str->data, (new_capacity + 1) * sizeof(char))
																 : malloc((new_capacity + 1) * sizeof(char));
	if (!ptr) return false;
	str->data = ptr;
	str->capacity = new_capacity;
	str->data[str->capacity] = '\0';
	return true;
}

static inline bool str_resize(struct str* str, size_t size)
{
	if (!str_reserve(str, size)) return false;
	if (str->size < size) memset(str->data + str->size + 1, 0, size * sizeof(char));
	str->size = size;
	return true;
}

static inline bool str_pushcv(struct str* str, struct cv cv)
{
	if (!str_reserve(str, str->size + cv.size)) return false;
	memcpy(str->data + str->size, cv.data, cv.size);
	str->size += cv.size;
	str->data[str->size] = '\0';
	return true;
}

static inline bool str_pushc(struct str* str, char c)
{
	if (!str_reserve(str, str->size + 1)) return false;
	str->data[str->size++] = c;
	str->data[str->size] = '\0';
	return true;
}
