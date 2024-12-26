#pragma once

// bootstrap
#include "utils/vtype.h"

#define PAIR_ALIGN(AlignKey, AlignVal) MAX(MAX(AlignKey, AlignVal), alignof(struct pair))
#define PAIR_SIZE(SizeKey, SizeVal, AlignKey, AlignVal)                                                                \
	ALIGN(sizeof(struct pair) + PAIR_VAL_OFFSET(SizeKey, AlignKey, AlignVal) + SizeVal, alignof(struct pair))
#define PAIR_KEY_OFFSET(AlignKey) (ALIGN(sizeof(struct pair), AlignKey) - sizeof(struct pair))
#define PAIR_VAL_OFFSET(SizeKey, AlignKey, AlignVal)                                                                   \
	(ALIGN(sizeof(struct pair) + PAIR_KEY_OFFSET(AlignKey) + SizeKey, AlignVal) - sizeof(struct pair))
#define VPAIR(Name, VTypeKey, VTypeVal)                                                                                \
	VTYPE_DYN(CATX(Name, _vtype), struct pair, &pair_dtor, &pair_copy, &pair_move, NULL, NULL, NULL,                   \
			  PAIR_SIZE((VTypeKey)->size, (VTypeVal)->size, (VTypeKey)->align, (VTypeVal)->align),                     \
			  PAIR_ALIGN((VTypeKey)->align, (VTypeVal)->align));                                                       \
	static struct vpair const Name = {                                                                                 \
		.vtype = CATX(Name, _vtype),                                                                                   \
		.vkey = *(VTypeKey),                                                                                           \
		.vval = *(VTypeVal),                                                                                           \
		.key_offset = PAIR_KEY_OFFSET((VTypeKey)->align),                                                              \
		.val_offset = PAIR_VAL_OFFSET((VTypeKey)->size, (VTypeKey)->align, (VTypeVal)->align),                         \
		.status_offset = PAIR_VAL_OFFSET((VTypeKey)->size, (VTypeKey)->align, (VTypeVal)->align) + (VTypeVal)->size,   \
	}
#define PAIR_KEY(Pair, TypeKey) (TypeKey*)pair_key((Pair))
#define PAIR_VAL(Pair, TypeVal) (TypeVal*)pair_val((Pair))
#define PAIR_CKEY(Pair, TypeKey) (TypeKey const*)pair_ckey((Pair))
#define PAIR_CVAL(Pair, TypeVal) (TypeVal const*)pair_cval((Pair))

typedef void pair_key_t;
typedef void pair_val_t;

struct vpair
{
	struct vtype vtype;
	struct vtype vkey;
	struct vtype vval;
	size_t key_offset;
	size_t val_offset;
	size_t status_offset;
};

enum pair_status
{
	PAIR_NONE = 0,
	PAIR_SET = 1,
	PAIR_UNSET = 2,
};

struct pair
{
	struct vpair const* vpair;
	uint8_t data[];
};

bool pair_ctor(struct pair* pair, struct vpair const* vpair);
bool pair_set_copy(struct pair* pair, pair_key_t const* key, pair_val_t const* val);
bool pair_set_move(struct pair* pair, pair_key_t* key, pair_val_t* val);
bool pair_unset(struct pair* pair);
void pair_dtor(struct pair* pair);
bool pair_copy(struct pair* pair, struct pair const* other);
bool pair_move(struct pair* pair, struct pair* other);

static inline pair_key_t* pair_key(struct pair* pair);
static inline pair_val_t* pair_val(struct pair* pair);
static inline pair_key_t const* pair_ckey(struct pair const* pair);
static inline pair_val_t const* pair_cval(struct pair const* pair);
static inline struct vpair const* pair_vpair(struct pair const* pair);
static inline enum pair_status pair_status(struct pair const* pair);
static inline void pair_set_status(struct pair* pair, enum pair_status status);

// impl

static inline pair_key_t* pair_key(struct pair* pair) { return pair->data + pair_vpair(pair)->key_offset; }

static inline pair_val_t* pair_val(struct pair* pair) { return pair->data + pair_vpair(pair)->val_offset; }

static inline pair_key_t const* pair_ckey(struct pair const* pair) { return pair->data + pair_vpair(pair)->key_offset; }

static inline pair_val_t const* pair_cval(struct pair const* pair) { return pair->data + pair_vpair(pair)->val_offset; }

static inline struct vpair const* pair_vpair(struct pair const* pair)
{
	uintptr_t mask = 3;
	uintptr_t r = (uintptr_t)pair->vpair & ~mask;
	return (struct vpair const*)r;
}

static inline enum pair_status pair_status(struct pair const* pair)
{
	uintptr_t mask = 3;
	uintptr_t r = (uintptr_t)pair->vpair & mask;
	return r;
}

static inline void pair_set_status(struct pair* pair, enum pair_status status)
{
	uintptr_t mask = 3;
	uintptr_t r = (uintptr_t)pair->vpair & ~mask;
	uintptr_t s = (uintptr_t)status & mask;
	pair->vpair = (struct vpair const*)(r | s);
}
