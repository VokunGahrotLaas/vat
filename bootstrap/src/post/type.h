#pragma once

// bootstrap
#include "utils/list.h"
#include "utils/str.h"

enum type_type
{
	TYPE_VOID,
	TYPE_NUM,
	TYPE_STR,
	TYPE_FN,
};

struct type_num
{
	struct str name;
};

struct type;

struct type_fn
{
	struct list args;
	struct type* ret;
};

struct type
{
	size_t count;
	enum type_type type;
	union type_value
	{
		struct type_num num;
		struct type_fn fn;
	} value;
};

struct type* type_init(enum type_type type_type, size_t count);
void type_free(struct type* type);
void type_pdtor(struct type** ptype);

VTYPE(vtype_ptype, struct type*, NULL, NULL, NULL, NULL, NULL, NULL);
VTYPE(vtype_uptype, struct type*, &type_pdtor, &copy_fail, NULL, NULL, NULL, NULL);
VLIST(vlist_ptype, struct type*, &vtype_ptype);
VLIST(vlist_uptype, struct type*, &vtype_uptype);
