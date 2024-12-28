#include "post/type.h"

struct type* type_init(enum type_type type_type, size_t count)
{
	struct type* type = calloc(1, sizeof(*type));
	if (type == NULL) return NULL;
	type->type = type_type;
	type->count = count;
	return type;
}

void type_free(struct type* type)
{
	if (type == NULL) return;
	switch (type->type)
	{
	case TYPE_VOID: break;
	case TYPE_NUM: str_dtor(&type->value.num.name); break;
	case TYPE_STR: break;
	case TYPE_FN:
		for (size_t i = 0; i < type->value.fn.args.size; ++i)
			type_free(*LIST_GET(&type->value.fn.args, struct type*, i));
		type_free(type->value.fn.ret);
		break;
	};
	free(type);
}

void type_pdtor(struct type** ptype)
{
	if (ptype == NULL) return;
	type_free(*ptype);
	*ptype = NULL;
}
