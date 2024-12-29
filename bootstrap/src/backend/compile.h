#pragma once

// boostrap
#include "utils/list.h"
#include "utils/utils.h"

bool compile_c(struct list const* source, char const* dest);
bool run(char const* file);

// impl

VTYPE(vtype_pchar, char*, NULL, NULL, NULL, NULL, NULL, NULL);
VLIST(vlist_pchar, char*, &vtype_pchar);
