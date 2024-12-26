#include "utils/pair.h"

bool pair_ctor(struct pair* pair, struct vpair const* vpair)
{
	pair->vpair = vpair;
	*pair_pstatus(pair) = PAIR_NONE;
	return true;
}

bool pair_set_copy(struct pair* pair, pair_key_t const* key, pair_val_t const* val)
{
	pair_unset(pair);
	bool r = true;
	r = r && vcopy(&pair->vpair->vkey, pair_key(pair), key);
	r = r && vcopy(&pair->vpair->vval, pair_val(pair), val);
	if (r) *pair_pstatus(pair) = PAIR_SET;
	return r;
}

bool pair_set_move(struct pair* pair, pair_key_t* key, pair_val_t* val)
{
	pair_unset(pair);
	bool r = true;
	r = r && vmove(&pair->vpair->vkey, pair_key(pair), key);
	r = r && vmove(&pair->vpair->vval, pair_val(pair), val);
	if (r) *pair_pstatus(pair) = PAIR_SET;
	return r;
}

bool pair_unset(struct pair* pair)
{
	if (pair_status(pair) != PAIR_SET) return false;
	*pair_pstatus(pair) = PAIR_UNSET;
	vdtor(&pair->vpair->vkey, pair_key(pair));
	vdtor(&pair->vpair->vval, pair_val(pair));
	return true;
}

void pair_dtor(struct pair* pair)
{
	*pair_pstatus(pair) = PAIR_NONE;
	if (pair_status(pair) != PAIR_SET) return;
	vdtor(&pair->vpair->vkey, pair_key(pair));
	vdtor(&pair->vpair->vval, pair_val(pair));
}

bool pair_copy(struct pair* pair, struct pair const* other)
{
	pair->vpair = other->vpair;
	*pair_pstatus(pair) = pair_status(other);
	if (pair_status(pair) != PAIR_SET) return true;
	bool r = true;
	r = r && vcopy(&pair->vpair->vkey, pair_key(pair), pair_ckey(other));
	r = r && vcopy(&pair->vpair->vval, pair_val(pair), pair_cval(other));
	if (!r) *pair_pstatus(pair) = PAIR_NONE;
	return r;
}

bool pair_move(struct pair* pair, struct pair* other)
{
	pair->vpair = other->vpair;
	*pair_pstatus(pair) = pair_status(other);
	if (!pair_status(pair)) return true;
	bool r = true;
	r = r && vmove(&pair->vpair->vkey, pair_key(pair), pair_key(other));
	r = r && vmove(&pair->vpair->vval, pair_val(pair), pair_val(other));
	if (!r) *pair_pstatus(pair) = PAIR_NONE;
	return r;
}
