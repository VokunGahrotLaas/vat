#pragma once

// bootstrap
#include "utils/list.h"
#include "utils/str.h"

enum type_type
{
	TYPE_NUM,
	TYPE_STR,
	TYPE_FN,
};

struct type_num
{
	struct str name;
};

struct type_fn
{
	struct list args;
};

struct type
{
	enum type_type type;
	union type_value
	{
		struct type_num num;
		struct type_fn fn;
	} value;
};
