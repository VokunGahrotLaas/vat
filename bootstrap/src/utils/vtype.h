#pragma once

// libc
#include <alloca.h>
#include <stdio.h>
#include <string.h>
// bootstrap
#include "utils/hash.h"

#define VTYPE_DYN(Name, Type, Dtor, Copy, Move, Cmp, Hash, Print, Size, Align)                                         \
	static struct vtype const Name = {                                                                                 \
		.size = (Size),                                                                                                \
		.align = (Align),                                                                                              \
		.name = STRX(Name),                                                                                            \
		.type_name = STRX(Type),                                                                                       \
		.dtor = (dtor_t*)(Dtor),                                                                                       \
		.copy = (copy_t*)(Copy),                                                                                       \
		.move = (move_t*)(Move),                                                                                       \
		.cmp = (cmp_t*)(Cmp),                                                                                          \
		.hash = (hash_t*)(Hash),                                                                                       \
		.print = (print_t*)(Print),                                                                                    \
	}
#define VTYPE(Name, Type, Dtor, Copy, Move, Cmp, Hash, Print)                                                          \
	VTYPE_DYN(Name, Type, Dtor, Copy, Move, Cmp, Hash, Print, sizeof(Type), alignof(Type))

typedef void self_t;

enum cmp_result
{
	CMP_LT = -1,
	CMP_EQ = 0,
	CMP_GT = 1,
};

typedef void dtor_t(self_t* self);
typedef bool copy_t(self_t* self, self_t const* other);
typedef bool move_t(self_t* self, self_t* other);
typedef enum cmp_result cmp_t(self_t const* self, self_t const* other);
typedef uint64_t hash_t(self_t const* self, uint64_t seed);
typedef void print_t(self_t const* self, FILE* stream);

struct vtype
{
	size_t size;
	size_t align;
	char const* name;
	char const* type_name;
	dtor_t* dtor;
	copy_t* copy;
	move_t* move;
	cmp_t* cmp;
	hash_t* hash;
	print_t* print;
};

static inline void vdtor(struct vtype const* vtype, self_t* self);
static inline bool vcopy(struct vtype const* vtype, self_t* self, self_t const* other);
static inline bool vmove(struct vtype const* vtype, self_t* self, self_t* other);
static inline bool vcopy(struct vtype const* vtype, self_t* self, self_t const* other);
static inline uint64_t vhash(struct vtype const* vtype, self_t const* self, uint64_t seed);
static inline void vprint(struct vtype const* vtype, self_t const* self, FILE* stream);

static inline bool vswap(struct vtype const* vtype, self_t* self, self_t* other);

bool copy_fail(UNUSED void* ptr, UNUSED void const* other);
bool move_fail(UNUSED void* ptr, UNUSED void* other);

// impl

static inline void vdtor(struct vtype const* vtype, self_t* self)
{
	if (vtype->dtor)
		(*vtype->dtor)(self);
	else
		memset(self, 0, vtype->size);
}

static inline bool vcopy(struct vtype const* vtype, self_t* self, self_t const* other)
{
	if (vtype->copy) return (*vtype->copy)(self, other);
	memcpy(self, other, vtype->size);
	return true;
}

static inline bool vmove(struct vtype const* vtype, self_t* self, self_t* other)
{
	if (vtype->move) return (*vtype->move)(self, other);
	memcpy(self, other, vtype->size);
	memset(other, 0, vtype->size);
	return true;
}

static inline enum cmp_result vcmp(struct vtype const* vtype, self_t const* self, self_t const* other)
{
	if (vtype->cmp) return (*vtype->cmp)(self, other);
	return memcmp(self, other, vtype->size);
}

static inline uint64_t vhash(struct vtype const* vtype, self_t const* self, uint64_t seed)
{
	if (vtype->hash) return (*vtype->hash)(self, seed);
	return hash(self, vtype->size, seed);
}

static inline void vprint(struct vtype const* vtype, self_t const* self, FILE* stream)
{
	fprintf(stream, "%s{%s size=%zu align=%zu value=", vtype->name, vtype->type_name, vtype->size, vtype->align);
	if (vtype->print)
		(*vtype->print)(self, stream);
	else
		fprintf(stream, "%p", (void*)self);
	fputc('}', stream);
}

static inline bool vswap(struct vtype const* vtype, self_t* self, self_t* other)
{
	self_t* tmp = alloca(vtype->size);
	bool r = true;
	r = r && vmove(vtype, tmp, self);
	r = r && vmove(vtype, self, other);
	r = r && vmove(vtype, other, tmp);
	return r;
}
