#pragma once

// libc
#include <stdio.h>
// bootstrap
#include "utils/utils.h"

#define LOC_INVALID                                                                                                    \
	((struct loc){                                                                                                     \
		.filename = "not-a-file",                                                                                      \
		.first_line = 0,                                                                                               \
		.first_column = 0,                                                                                             \
		.last_line = 0,                                                                                                \
		.last_column = 0,                                                                                              \
	})

struct pos
{
	char const* filename;
	size_t line;
	size_t column;
};

struct loc
{
	char const* filename;
	size_t first_line;
	size_t first_column;
	size_t last_line;
	size_t last_column;
};

void loc_print(struct loc const* loc, FILE* stream);
