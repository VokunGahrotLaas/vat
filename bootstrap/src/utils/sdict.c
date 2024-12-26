#include "utils/sdict.h"

bool sdict_ctor(struct sdict* sdict, struct vdict const* vdict, size_t stack_capacity, size_t dict_size_hint)
{
	sdict->size_hint = dict_size_hint;
	if (!list_ctor(&sdict->scopes, &vlist_dict, stack_capacity)) return false;
	struct dict scope;
	if (!dict_ctor(&scope, vdict, sdict->size_hint)) return false;
	return list_push_move(&sdict->scopes, &scope);
}

void sdict_dtor(struct sdict* sdict) { list_dtor(&sdict->scopes); }

bool sdict_copy(struct sdict* sdict, struct sdict const* other)
{
	sdict->size_hint = other->size_hint;
	return list_copy(&sdict->scopes, &other->scopes);
}

bool sdict_scope_begin(struct sdict* sdict)
{
	struct dict scope;
	if (!dict_ctor(&scope, LIST_FRONT(&sdict->scopes, struct dict)->vdict, sdict->size_hint)) return false;
	return list_push_move(&sdict->scopes, &scope);
}

bool sdict_scope_end(struct sdict* sdict)
{
	if (sdict->scopes.size <= 1) return false;
	return list_pop(&sdict->scopes);
}

struct pair* sdict_add_copy(struct sdict* sdict, pair_key_t const* key, pair_val_t const* value)
{
	struct dict* scope = LIST_BACK(&sdict->scopes, struct dict);
	return dict_add_copy(scope, key, value);
}

struct pair* sdict_add_move(struct sdict* sdict, pair_key_t* key, pair_val_t* value)
{
	struct dict* scope = LIST_BACK(&sdict->scopes, struct dict);
	return dict_add_move(scope, key, value);
}

bool sdict_remove(struct sdict* sdict, pair_key_t const* key)
{
	struct dict* scope = LIST_BACK(&sdict->scopes, struct dict);
	return dict_remove(scope, key);
}

struct pair* sdict_find(struct sdict* sdict, pair_key_t const* key) { return (struct pair*)sdict_cfind(sdict, key); }

struct pair const* sdict_cfind(struct sdict const* sdict, pair_key_t const* key)
{
	for (size_t i = 0; i < sdict->scopes.size; ++i)
	{
		size_t idx = sdict->scopes.size - i - 1;
		struct dict const* scope = LIST_CGET(&sdict->scopes, struct dict, idx);
		struct pair const* pair = dict_cfind(scope, key);
		if (pair) return pair;
	}
	return NULL;
}
