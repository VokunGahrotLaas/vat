#include "utils/pair.h"

bool pair_ctor(struct pair* pair, struct vpair const* vpair)
{
	pair->vpair = vpair;
	pair_set_status(pair, PAIR_NONE);
	return true;
}

bool pair_set_copy(struct pair* pair, pair_key_t const* key, pair_val_t const* val)
{
	pair_unset(pair);
	bool r = true;
	struct vpair const* vpair = pair_vpair(pair);
	r = r && vcopy(&vpair->vkey, pair_key(pair), key);
	r = r && vcopy(&vpair->vval, pair_val(pair), val);
	if (r) pair_set_status(pair, PAIR_SET);
	return r;
}

bool pair_set_move(struct pair* pair, pair_key_t* key, pair_val_t* val)
{
	pair_unset(pair);
	bool r = true;
	struct vpair const* vpair = pair_vpair(pair);
	r = r && vmove(&vpair->vkey, pair_key(pair), key);
	r = r && vmove(&vpair->vval, pair_val(pair), val);
	if (r) pair_set_status(pair, PAIR_SET);
	return r;
}

bool pair_unset(struct pair* pair)
{
	if (pair_status(pair) != PAIR_SET) return false;
	pair_set_status(pair, PAIR_UNSET);
	struct vpair const* vpair = pair_vpair(pair);
	vdtor(&vpair->vkey, pair_key(pair));
	vdtor(&vpair->vval, pair_val(pair));
	return true;
}

void pair_dtor(struct pair* pair)
{
	pair_set_status(pair, PAIR_NONE);
	if (pair_status(pair) != PAIR_SET) return;
	struct vpair const* vpair = pair_vpair(pair);
	vdtor(&vpair->vkey, pair_key(pair));
	vdtor(&vpair->vval, pair_val(pair));
}

bool pair_copy(struct pair* pair, struct pair const* other)
{
	pair->vpair = other->vpair;
	// status is in vpair so this is not needed
	// pair_set_status(pair, pair_status(other));
	if (pair_status(pair) != PAIR_SET) return true;
	bool r = true;
	struct vpair const* vpair = pair_vpair(pair);
	r = r && vcopy(&vpair->vkey, pair_key(pair), pair_ckey(other));
	r = r && vcopy(&vpair->vval, pair_val(pair), pair_cval(other));
	if (!r) pair_set_status(pair, PAIR_NONE);
	return r;
}

bool pair_move(struct pair* pair, struct pair* other)
{
	pair->vpair = other->vpair;
	// status is in vpair so this is not needed
	// pair_set_status(pair, pair_status(other));
	if (!pair_status(pair)) return true;
	bool r = true;
	struct vpair const* vpair = pair_vpair(pair);
	r = r && vmove(&vpair->vkey, pair_key(pair), pair_key(other));
	r = r && vmove(&vpair->vval, pair_val(pair), pair_val(other));
	if (!r) pair_set_status(pair, PAIR_NONE);
	return r;
}
