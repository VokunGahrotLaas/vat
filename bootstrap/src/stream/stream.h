#pragma once

// libc
#include <stdbool.h>
#include <stdio.h>

struct stream
{
	FILE* stream;
	bool eof;
	int current;
	size_t line;
	size_t column;
};

bool stream_from_file(struct stream* stream, char const* filename);
void stream_dtor(struct stream* stream);
int stream_peek(struct stream* stream);
int stream_pop(struct stream* stream);
