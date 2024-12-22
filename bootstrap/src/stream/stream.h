#pragma once

// libc
#include <stdbool.h>
#include <stdio.h>
// bootstrap
#include "stream/location.h"

struct stream
{
	FILE* stream;
	char const* filename;
	bool eol;
	int current;
	struct pos pos;
	struct pos peek_pos;
};

bool stream_from_file(struct stream* stream, char const* filename);
void stream_dtor(struct stream* stream);
int stream_peek(struct stream* stream);
int stream_pop(struct stream* stream);
