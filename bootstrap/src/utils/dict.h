#pragma once

// bootstrap
#include "utils/list.h"
#include "utils/pair.h"

#define VDICT(Name, TypeKey, TypeVal, VPair)                                                                           \
	VLIST_DYN(CATX(Name, _vlist), PAIR_SIZE(sizeof(TypeKey), sizeof(TypeVal), alignof(TypeKey), alignof(TypeVal)),     \
			  &(VPair)->vtype);                                                                                        \
	static struct vdict const Name = {                                                                                 \
		.vpair = *(VPair),                                                                                             \
		.vlist = CATX(Name, _vlist),                                                                                   \
	}

struct vdict
{
	struct vpair vpair;
	struct vlist vlist;
};

struct dict
{
	struct vdict const* vdict;
	struct list pairs;
	size_t size;
};

bool dict_ctor(struct dict* dict, struct vdict const* vdict, size_t size);
void dict_dtor(struct dict* dict);
bool dict_copy(struct dict* dict, struct dict const* other);

bool dict_reserve(struct dict* dict, size_t size);
struct pair* dict_add_copy(struct dict* dict, pair_key_t const* key, pair_val_t const* value);
struct pair* dict_add_move(struct dict* dict, pair_key_t* key, pair_val_t* value);
bool dict_remove(struct dict* dict, pair_key_t const* key);
struct pair* dict_find(struct dict* dict, pair_key_t const* key);
struct pair const* dict_cfind(struct dict const* dict, pair_key_t const* key);
struct pair* dict_hfind(struct dict* dict, pair_key_t const* key, uint64_t h);
struct pair const* dict_chfind(struct dict const* dict, pair_key_t const* key, uint64_t h);
uint64_t dict_hash(struct dict const* dict, pair_key_t const* key);

// impl

VTYPE(vtype_dict, struct dict, &dict_dtor, &dict_copy, NULL, NULL, NULL, NULL);
VLIST(vlist_dict, struct dict, &vtype_dict);
