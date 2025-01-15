#pragma once

// boostrap
#include "utils/list.h"
#include "utils/utils.h"

bool compile_c(char const* source, char const* dest);
bool link_c(struct list const* objs, char const* dest);
bool run(char const* file);

// impl

VTYPE(vtype_pchar, char*, NULL, NULL, NULL, NULL, NULL, NULL);
VLIST(vlist_pchar, char*, &vtype_pchar);
