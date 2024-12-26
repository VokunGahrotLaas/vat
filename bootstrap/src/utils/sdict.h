#pragma once

// bootstrap
#include "utils/dict.h"
#include "utils/list.h"

struct sdict
{
	struct list scopes;
	size_t size_hint;
};

bool sdict_ctor(struct sdict* sdict, struct vdict const* vdict, size_t stack_capacity, size_t dict_size_hint);
void sdict_dtor(struct sdict* sdict);
bool sdict_copy(struct sdict* sdict, struct sdict const* other);

bool sdict_scope_begin(struct sdict* sdict);
bool sdict_scope_end(struct sdict* sdict);

struct pair* sdict_add_copy(struct sdict* sdict, pair_key_t const* key, pair_val_t const* value);
struct pair* sdict_add_move(struct sdict* sdict, pair_key_t* key, pair_val_t* value);
bool sdict_remove(struct sdict* sdict, pair_key_t const* key);
struct pair* sdict_find(struct sdict* sdict, pair_key_t const* key);
struct pair const* sdict_cfind(struct sdict const* sdict, pair_key_t const* key);

// impl

VTYPE(vtype_sdict, struct sdict, &sdict_dtor, &sdict_copy, NULL, NULL, NULL);
VLIST(vlist_sdict, struct sdict, &vtype_sdict);
