#pragma once

// bootstrap
#include "utils/utils.h"

#define VLIST(Type, Name, Dtor, Copy, Move)                                                                            \
	static uint8_t Name##_empty_impl[sizeof(Type)] = { 0 };                                                            \
	static struct vlist const Name = {                                                                                 \
		.elem_size = sizeof(Type),                                                                                     \
		.empty_impl = Name##_empty_impl,                                                                               \
		.dtor = (dtor_t*)(Dtor),                                                                                       \
		.copy = (copy_t*)(Copy),                                                                                       \
		.move = (move_t*)(Move),                                                                                       \
	}
#define LIST_GET(List, Type, Idx) ((Type*)list_get((List), (Idx)))
#define LIST_GETC(List, Type, Idx) ((Type const*)list_getc((List), (Idx)))
#define LIST_FRONT(List, Type) ((Type*)list_front((List)))
#define LIST_FRONTC(List, Type) ((Type const*)list_frontc((List)))
#define LIST_BACK(List, Type) ((Type*)list_back((List)))
#define LIST_BACKC(List, Type) ((Type const*)list_backc((List)))

typedef void dtor_t(void* ptr);
typedef bool copy_t(void* ptr, void const* other);
typedef bool move_t(void* ptr, void* other);

struct vlist
{
	size_t elem_size;
	uint8_t* empty_impl;
	dtor_t* dtor;
	copy_t* copy;
	move_t* move;
};

struct list
{
	struct vlist const* vlist;
	void* data;
	size_t size;
	size_t capacity;
};

bool list_ctor(struct list* list, struct vlist const* vlist, size_t capacity);
void list_dtor(struct list* list);
bool list_copy(struct list* list, struct list const* other);
bool list_move(struct list* list, struct list* other);

bool list_reserve(struct list* list, size_t capacity);
bool list_push_copy(struct list* list, void const* data);
bool list_push_move(struct list* list, void* data);
static inline void* list_get(struct list* list, size_t idx);
static inline void const* list_getc(struct list const* list, size_t idx);

static inline void* list_front(struct list* list);
static inline void const* list_frontc(struct list const* list);

static inline void* list_back(struct list* list);
static inline void const* list_backc(struct list const* list);

bool copy_fail(UNUSED void* ptr, UNUSED void const* other);
bool move_fail(UNUSED void* ptr, UNUSED void* other);

// impl

VLIST(struct list, vlist_list, &list_dtor, &list_copy, &list_move);

static inline void* list_get(struct list* list, size_t idx)
{
	// const cast is ok because list is non const here
	// this is just to avoid duplicate code
	return (void*)list_getc(list, idx);
}

static inline void const* list_getc(struct list const* list, size_t idx)
{
	uint8_t const* data = list->data;
	return data + list->vlist->elem_size * idx;
}

static inline void* list_front(struct list* list)
{
	DBG_ASSERT(list->size > 0 && "list_front: list must not be empty");
	return list_get(list, 0);
}

static inline void const* list_frontc(struct list const* list)
{
	DBG_ASSERT(list->size > 0 && "list_frontc: list must not be empty");
	return list_getc(list, 0);
}

static inline void* list_back(struct list* list)
{
	DBG_ASSERT(list->size > 0 && "list_back: list must not be empty");
	return list_get(list, list->size - 1);
}

static inline void const* list_backc(struct list const* list)
{
	DBG_ASSERT(list->size > 0 && "list_backc: list must not be empty");
	return list_getc(list, list->size - 1);
}
