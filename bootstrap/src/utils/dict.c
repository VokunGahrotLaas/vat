#include "utils/dict.h"

// bootstrap
#include "utils/hash.h"

static inline struct pair* dict_find_new(struct dict* dict, pair_key_t const* key);

bool dict_ctor(struct dict* dict, struct vdict const* vdict, size_t size)
{
	dict->vdict = vdict;
	dict->size = 0;
	list_ctor(&dict->pairs, &vdict->vlist, 0);
	return dict_reserve(dict, size);
}

void dict_dtor(struct dict* dict) { list_dtor(&dict->pairs); }

bool dict_copy(struct dict* dict, struct dict const* other) { return list_copy(&dict->pairs, &other->pairs); }

bool dict_move(struct dict* dict, struct dict* other) { return list_move(&dict->pairs, &other->pairs); }

bool dict_reserve(struct dict* dict, size_t size)
{
	if (size <= dict->pairs.size) return true;
	size_t act_size = dict->pairs.size != 0 ? dict->pairs.size : 16;
	while (act_size < size)
		act_size <<= 1;
	struct list pairs;
	if (!list_move(&pairs, &dict->pairs)) return false;
	if (!list_ctor(&dict->pairs, &dict->vdict->vlist, size)) return false;
	dict->pairs.size = act_size;
	for (size_t i = 0; i < act_size; ++i)
		if (!pair_ctor(list_get(&dict->pairs, i), &dict->vdict->vpair)) return false;
	for (size_t i = 0; i < pairs.size; ++i)
	{
		struct pair* pair = LIST_GET(&pairs, struct pair, i);
		if (pair_status(pair) != PAIR_SET) continue;
		*pair_pstatus(pair) = PAIR_UNSET;
		if (!dict_add_move(dict, pair_key(pair), pair_val(pair))) return false;
	}
	return true;
}

struct pair* dict_add_copy(struct dict* dict, pair_key_t const* key, pair_val_t const* value)
{
	if (!dict_reserve(dict, dict->size + 1)) return false;
	struct pair* pair = dict_find_new(dict, key);
	DBG_ASSERT(pair && "could not find free pair in dict");
	if (pair_status(pair) == PAIR_SET) return NULL;
	if (!pair_set_copy(pair, key, value)) return NULL;
	return pair;
}

struct pair* dict_add_move(struct dict* dict, pair_key_t* key, pair_val_t* value)
{
	if (!dict_reserve(dict, dict->size + 1)) return false;
	struct pair* pair = dict_find_new(dict, key);
	DBG_ASSERT(pair && "could not find free pair in dict");
	if (pair_status(pair) == PAIR_SET) return NULL;
	if (!pair_set_move(pair, key, value)) return NULL;
	return pair;
}

bool dict_remove(struct dict* dict, pair_key_t const* key)
{
	struct pair* pair = dict_find(dict, key);
	if (!pair) return false;
	pair_unset(pair);
	return true;
}

struct pair* dict_find(struct dict* dict, pair_key_t const* key) { return (struct pair*)dict_cfind(dict, key); }

struct pair const* dict_cfind(struct dict const* dict, pair_key_t const* key)
{
	uint64_t h = vhash(&dict->vdict->vpair.vkey, key, HASH_SEED);
	size_t const size = dict->pairs.size;
	for (size_t i = 0; i < size; ++i)
	{
		size_t idx = (h + (i * i + i) / 2) % size;
		struct pair const* pair = LIST_CGET(&dict->pairs, struct pair, idx);
		if (pair_status(pair) == PAIR_NONE) break;
		if (pair_status(pair) == PAIR_UNSET) continue;
		if (vcmp(&dict->vdict->vpair.vkey, pair_ckey(pair), key) != 0) continue;
		return pair;
	}
	return NULL;
}

static inline struct pair* dict_find_new(struct dict* dict, pair_key_t const* key)
{
	uint64_t h = vhash(&dict->vdict->vpair.vkey, key, HASH_SEED);
	size_t const size = dict->pairs.size;
	struct pair* first_unset = NULL;
	for (size_t i = 0; i < size; ++i)
	{
		size_t idx = (h + (i * i + i) / 2) % size;
		struct pair* pair = LIST_GET(&dict->pairs, struct pair, idx);
		if (pair_status(pair) == PAIR_NONE) return first_unset != NULL ? first_unset : pair;
		if (pair_status(pair) == PAIR_UNSET)
		{
			if (first_unset == NULL) first_unset = pair;
			continue;
		}
		if (vcmp(&dict->vdict->vpair.vkey, pair_ckey(pair), key) != 0) continue;
		return pair;
	}
	return first_unset;
}
