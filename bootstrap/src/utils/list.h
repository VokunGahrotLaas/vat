#pragma once

// bootstrap
#include "utils/utils.h"
#include "utils/vtype.h"

#define VLIST_DYN(Name, Size, VType)                                                                                   \
	static uint8_t CATX(Name, _empty_impl)[(Size)] = { 0 };                                                            \
	static struct vlist const Name = {                                                                                 \
		.vtype = *(VType),                                                                                             \
		.empty_impl = CATX(Name, _empty_impl),                                                                         \
	}
#define VLIST(Name, Type, VType) VLIST_DYN(Name, sizeof(Type), VType)
#define LIST_GET(List, Type, Idx) ((Type*)list_get((List), (Idx)))
#define LIST_CGET(List, Type, Idx) ((Type const*)list_cget((List), (Idx)))
#define LIST_FRONT(List, Type) ((Type*)list_front((List)))
#define LIST_CFRONT(List, Type) ((Type const*)list_cfront((List)))
#define LIST_BACK(List, Type) ((Type*)list_back((List)))
#define LIST_CBACK(List, Type) ((Type const*)list_cback((List)))

struct vlist
{
	struct vtype vtype;
	uint8_t* empty_impl;
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
enum cmp_result list_cmp(struct list const* list, struct list const* other);
uint64_t list_hash(struct list const* list, uint64_t seed);

bool list_reserve(struct list* list, size_t capacity);
bool list_push_copy(struct list* list, void const* data);
bool list_push_move(struct list* list, void* data);
static inline void* list_get(struct list* list, size_t idx);
static inline void const* list_cget(struct list const* list, size_t idx);

static inline void* list_front(struct list* list);
static inline void const* list_cfront(struct list const* list);

static inline void* list_back(struct list* list);
static inline void const* list_cback(struct list const* list);

// impl

VTYPE(vtype_list, struct list, &list_dtor, &list_copy, &list_move, &list_cmp, &list_hash);
VLIST(vlist_list, struct list, &vtype_list);

static inline void* list_get(struct list* list, size_t idx)
{
	// const cast is ok because list is non const here
	// this is just to avoid duplicate code
	return (void*)list_cget(list, idx);
}

static inline void const* list_cget(struct list const* list, size_t idx)
{
	uint8_t const* data = list->data;
	return data + list->vlist->vtype.size * idx;
}

static inline void* list_front(struct list* list)
{
	DBG_ASSERT(list->size > 0 && "list_front: list must not be empty");
	return list_get(list, 0);
}

static inline void const* list_cfront(struct list const* list)
{
	DBG_ASSERT(list->size > 0 && "list_frontc: list must not be empty");
	return list_cget(list, 0);
}

static inline void* list_back(struct list* list)
{
	DBG_ASSERT(list->size > 0 && "list_back: list must not be empty");
	return list_get(list, list->size - 1);
}

static inline void const* list_cback(struct list const* list)
{
	DBG_ASSERT(list->size > 0 && "list_backc: list must not be empty");
	return list_cget(list, list->size - 1);
}
