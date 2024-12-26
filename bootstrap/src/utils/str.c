#include "utils/str.h"

char str_empty_impl[1] = "";

uint64_t cv_hash_(struct cv const* cv, uint64_t seed) { return cv_hash(*cv, seed); }

enum cmp_result cv_cmp_(struct cv const* cv, struct cv const* other) { return cv_cmp(*cv, *other); }

void str_dtor(struct str* str)
{
	if (str->data != NULL && str->data != str_empty_impl) free(str->data);
	str->data = str_empty_impl;
	str->capacity = str->size = 0;
}

bool str_copy(struct str* str, struct str const* other) { return str_of_cv(str, cv_str(other)); }

uint64_t str_hash(struct str const* str, uint64_t seed) { return cv_hash(cv_str(str), seed); }

enum cmp_result str_cmp(struct str const* str, struct str const* other) { return cv_cmp(cv_str(str), cv_str(other)); }
