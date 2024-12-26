#include "utils/str.h"

char str_empty_impl[1] = "";

void str_dtor(struct str* str)
{
	if (str->data != NULL && str->data != str_empty_impl) free(str->data);
	str->data = str_empty_impl;
	str->capacity = str->size = 0;
}

bool str_copy(struct str* str, struct str const* other) { return str_of_cv(str, cv_str(other)); }

bool str_move(struct str* str, struct str* other)
{
	*str = *other;
	other->data = str_empty_impl;
	other->capacity = other->size = 0;
	return true;
}
