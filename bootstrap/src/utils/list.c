#include "utils/list.h"

// libc
#include <stdlib.h>
#include <string.h>

bool list_ctor(struct list* list, struct vlist const* vlist, size_t capacity)
{
	list->vlist = vlist;
	list->data = capacity > 0 ? calloc(capacity + 1, vlist->vtype.size) : vlist->empty_impl;
	list->capacity = capacity;
	list->size = 0;
	return list->data != NULL;
}

void list_dtor(struct list* list)
{
	if (list->data != NULL && list->data != list->vlist->empty_impl)
	{
		if (list->vlist->vtype.dtor)
			for (size_t i = 0; i < list->size; ++i)
				vdtor(&list->vlist->vtype, list_get(list, i));
		free(list->data);
	}
	list->data = list->vlist->empty_impl;
	list->size = list->capacity = 0;
}

bool list_copy(struct list* list, struct list const* other)
{
	if (!list_ctor(list, other->vlist, other->size)) return false;
	for (size_t i = 0; i < other->size; ++i)
		if (!list_push_copy(list, list_cget(other, i))) return false;
	return true;
}

bool list_move(struct list* list, struct list* other)
{
	*list = *other;
	other->data = other->vlist->empty_impl;
	other->capacity = other->size = 0;
	return true;
}

uint64_t list_hash(UNUSED struct list const* list, uint64_t seed)
{
	uint64_t h = seed;
	for (size_t i = 0; i < list->size; ++i)
		h = vhash(&list->vlist->vtype, list_cget(list, i), h);
	return h;
}

enum cmp_result list_cmp(struct list const* list, struct list const* other)
{
	enum cmp_result r = CMP_EQ;
	for (size_t i = 0; r == CMP_EQ && i < list->size && i < other->size; ++i)
		r = vcmp(&list->vlist->vtype, list_cget(list, i), list_cget(other, i));
	if (r == CMP_EQ) return list->size == other->size ? CMP_EQ : list->size < other->size ? CMP_LT : CMP_GT;
	return r;
}

bool list_reserve(struct list* list, size_t capacity)
{
	if (capacity <= list->capacity) return true;
	size_t new_capacity = list->capacity;
	while (new_capacity < capacity)
		new_capacity *= 2;
	void* ptr = list->data != NULL && list->data != list->vlist->empty_impl
		? realloc(list->data, (new_capacity + 1) * list->vlist->vtype.size)
		: malloc((new_capacity + 1) * list->vlist->vtype.size);
	if (!ptr) return false;
	list->data = ptr;
	list->capacity = new_capacity;
	uint8_t* data = list->data;
	memset(data + list->capacity, 0, list->vlist->vtype.size);
	return true;
}

bool list_push_copy(struct list* list, void const* data)
{
	if (!list_reserve(list, list->size + 1)) return false;
	void* dest = list_get(list, list->size);
	if (!vcopy(&list->vlist->vtype, dest, data)) return false;
	++list->size;
	return true;
}

bool list_push_move(struct list* list, void* data)
{
	if (!list_reserve(list, list->size + 1)) return false;
	void* dest = list_get(list, list->size);
	if (!vmove(&list->vlist->vtype, dest, data)) return false;
	++list->size;
	return true;
}
