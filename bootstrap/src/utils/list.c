#include "utils/list.h"

// libc
#include <stdlib.h>
#include <string.h>

bool list_ctor(struct list* list, struct vlist const* vlist, size_t capacity)
{
	list->vlist = vlist;
	list->data = capacity > 0 ? malloc((capacity + 1) * vlist->elem_size) : vlist->empty_impl;
	list->capacity = capacity;
	list->size = 0;
	return list->data != NULL;
}

void list_dtor(struct list* list)
{
	if (list->data != NULL && list->data != list->vlist->empty_impl)
	{
		if (list->vlist->dtor)
			for (size_t i = 0; i < list->size; ++i)
				(*list->vlist->dtor)(list_get(list, i));
		free(list->data);
	}
	list->data = list->vlist->empty_impl;
	list->size = list->capacity = 0;
}

bool list_copy(struct list* list, struct list const* other)
{
	if (!list_ctor(list, other->vlist, other->size)) return false;
	for (size_t i = 0; i < other->size; ++i)
		if (!list_push_copy(list, list_getc(other, i))) return false;
	return true;
}

bool list_move(struct list* list, struct list* other)
{
	*list = *other;
	other->data = other->vlist->empty_impl;
	other->capacity = other->size = 0;
	return true;
}

bool list_reserve(struct list* list, size_t capacity)
{
	if (capacity <= list->capacity) return true;
	size_t new_capacity = list->capacity;
	while (new_capacity < capacity)
		new_capacity *= 2;
	void* ptr = list->data != NULL && list->data != list->vlist->empty_impl
		? realloc(list->data, (new_capacity + 1) * list->vlist->elem_size)
		: malloc((new_capacity + 1) * list->vlist->elem_size);
	if (!ptr) return false;
	list->data = ptr;
	list->capacity = new_capacity;
	uint8_t* data = list->data;
	memset(data + list->capacity, 0, list->vlist->elem_size);
	return true;
}

bool list_push_copy(struct list* list, void const* data)
{
	if (!list_reserve(list, list->size + 1)) return false;
	void* dest = list_get(list, list->size);
	if (list->vlist->copy)
	{
		if (!(*list->vlist->copy)(dest, data)) return false;
	}
	else
		memcpy(dest, data, list->vlist->elem_size);
	++list->size;
	return true;
}

bool list_push_move(struct list* list, void* data)
{
	if (!list_reserve(list, list->size + 1)) return false;
	void* dest = list_get(list, list->size);
	if (list->vlist->move)
	{
		if (!(*list->vlist->move)(dest, data)) return false;
	}
	else
	{
		memcpy(dest, data, list->vlist->elem_size);
		memset(data, 0, list->vlist->elem_size);
	}
	++list->size;
	return true;
}
