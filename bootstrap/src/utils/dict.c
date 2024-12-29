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

void dict_print(struct dict const* dict, FILE* stream)
{
	size_t const size = dict->pairs.size;
	fputc('{', stream);
	bool first = true;
	for (size_t i = 0; i < size; ++i)
	{
		struct pair const* pair = LIST_CGET(&dict->pairs, struct pair, i);
		if (pair_status(pair) != PAIR_SET) continue;
		if (!first) fputs(", ", stream);
		first = false;
		vprint(&dict->vdict->vpair.vkey, pair_ckey(pair), stream);
		fputs(": ", stream);
		vprint(&dict->vdict->vpair.vval, pair_cval(pair), stream);
	}
	fputc('}', stream);
}

bool dict_reserve(struct dict* dict, size_t size)
{
	if (size <= dict->pairs.size) return true;
	size_t act_size = dict->pairs.size != 0 ? dict->pairs.size : 16;
	while (act_size < size)
		act_size <<= 1;
	struct list pairs;
	if (!vmove(&vtype_list, &pairs, &dict->pairs)) return false;
	if (!list_ctor(&dict->pairs, &dict->vdict->vlist, act_size)) return false;
	dict->pairs.size = act_size;
	for (size_t i = 0; i < act_size; ++i)
		if (!pair_ctor(list_get(&dict->pairs, i), &dict->vdict->vpair)) return false;
	for (size_t i = 0; i < pairs.size; ++i)
	{
		struct pair* pair = LIST_GET(&pairs, struct pair, i);
		if (pair_status(pair) != PAIR_SET) continue;
		pair_set_status(pair, PAIR_UNSET);
		if (!dict_add_move(dict, pair_key(pair), pair_val(pair))) return false;
	}
	pairs.size = 0;
	list_dtor(&pairs);
	return true;
}

struct pair* dict_add_copy(struct dict* dict, pair_key_t const* key, pair_val_t const* value)
{
	if (!dict_reserve(dict, (dict->size + 1) * 2)) return false;
	struct pair* pair = dict_find_new(dict, key);
	DBG_ASSERT(pair && "could not find free pair in dict");
	++dict->size;
	if (pair_status(pair) == PAIR_SET) return NULL;
	if (!pair_set_copy(pair, key, value)) return NULL;
	return pair;
}

struct pair* dict_add_move(struct dict* dict, pair_key_t* key, pair_val_t* value)
{
	if (!dict_reserve(dict, (dict->size + 1) * 2)) return false;
	struct pair* pair = dict_find_new(dict, key);
	DBG_ASSERT(pair && "could not find free pair in dict");
	++dict->size;
	if (pair_status(pair) == PAIR_SET) return NULL;
	if (!pair_set_move(pair, key, value)) return NULL;
	return pair;
}

bool dict_remove(struct dict* dict, pair_key_t const* key)
{
	struct pair* pair = dict_find(dict, key);
	if (!pair) return false;
	--dict->size;
	pair_unset(pair);
	return true;
}

struct pair* dict_find(struct dict* dict, pair_key_t const* key) { return (struct pair*)dict_cfind(dict, key); }

struct pair const* dict_cfind(struct dict const* dict, pair_key_t const* key)
{
	uint64_t h = dict_hash(dict, key);
	return dict_chfind(dict, key, h);
}

struct pair* dict_hfind(struct dict* dict, pair_key_t const* key, uint64_t h)
{
	return (struct pair*)dict_chfind(dict, key, h);
}

struct pair const* dict_chfind(struct dict const* dict, pair_key_t const* key, uint64_t h)
{
	size_t const size = dict->pairs.size;
#ifdef BOOTSTRAP_LOG_DICT
	fputs("find(", stderr);
	vprint(&dict->vdict->vpair.vkey, key, stderr);
	fprintf(stderr, ") => size %zu hash %" PRIu64 "\n", size, h);
#endif
	for (size_t i = 0; i < size; ++i)
	{
		size_t idx = (h + (i * i + i) / 2) % size;
		struct pair const* pair = LIST_CGET(&dict->pairs, struct pair, idx);
#ifdef BOOTSTRAP_LOG_DICT
		fputs("find(", stderr);
		vprint(&dict->vdict->vpair.vkey, key, stderr);
		fprintf(stderr, ") => try at %zu => ", idx);
#endif
		if (pair_status(pair) == PAIR_NONE)
		{
#ifdef BOOTSTRAP_LOG_DICT
			fputs("none\n", stderr);
#endif
			break;
		}
		if (pair_status(pair) == PAIR_UNSET)
		{
#ifdef BOOTSTRAP_LOG_DICT
			fputs("unset\n", stderr);
#endif
			continue;
		}
		if (vcmp(&dict->vdict->vpair.vkey, pair_ckey(pair), key) != 0)
		{
#ifdef BOOTSTRAP_LOG_DICT
			fputs("not equal\n", stderr);
#endif
			continue;
		}
#ifdef BOOTSTRAP_LOG_DICT
		fputs("found\n", stderr);
#endif
		return pair;
	}
#ifdef BOOTSTRAP_LOG_DICT
	fputs("find(", stderr);
	vprint(&dict->vdict->vpair.vkey, key, stderr);
	fputs(") => not found\n", stderr);
#endif
	return NULL;
}

uint64_t dict_hash(struct dict const* dict, pair_key_t const* key)
{
	return vhash(&dict->vdict->vpair.vkey, key, HASH_SEED);
}

static inline struct pair* dict_find_new(struct dict* dict, pair_key_t const* key)
{
	uint64_t h = vhash(&dict->vdict->vpair.vkey, key, HASH_SEED);
	size_t const size = dict->pairs.size;
	struct pair* first_unset = NULL;
#ifdef BOOTSTRAP_LOG_DICT
	fputs("find_new(", stderr);
	vprint(&dict->vdict->vpair.vkey, key, stderr);
	fprintf(stderr, ") => size %zu hash %" PRIu64 "\n", size, h);
#endif
	for (size_t i = 0; i < size; ++i)
	{
		size_t idx = (h + (i * i + i) / 2) % size;
#ifdef BOOTSTRAP_LOG_DICT
		fputs("find_new(", stderr);
		vprint(&dict->vdict->vpair.vkey, key, stderr);
		fprintf(stderr, ") => try at %zu\n", idx);
#endif
		struct pair* pair = LIST_GET(&dict->pairs, struct pair, idx);
		if (pair_status(pair) == PAIR_NONE)
		{
#ifdef BOOTSTRAP_LOG_DICT
			fputs("find_new(", stderr);
			vprint(&dict->vdict->vpair.vkey, key, stderr);
			fprintf(stderr, ") => found at %zu\n", idx);
#endif
			return first_unset != NULL ? first_unset : pair;
		}
		if (pair_status(pair) == PAIR_UNSET)
		{
			if (first_unset == NULL) first_unset = pair;
			continue;
		}
		if (vcmp(&dict->vdict->vpair.vkey, pair_ckey(pair), key) != 0) continue;
#ifdef BOOTSTRAP_LOG_DICT
		fputs("find_new(", stderr);
		vprint(&dict->vdict->vpair.vkey, key, stderr);
		fprintf(stderr, ") => found at %zu\n", idx);
#endif
		return pair;
	}
#ifdef BOOTSTRAP_LOG_DICT
	fputs("find_new(", stderr);
	vprint(&dict->vdict->vpair.vkey, key, stderr);
	fputs(") => not found\n", stderr);
#endif
	return first_unset;
}
