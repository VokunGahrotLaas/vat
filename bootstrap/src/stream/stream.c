#include "stream/stream.h"

bool stream_from_file(struct stream* stream, char const* filename)
{
	stream->stream = fopen(filename, "r");
	stream->eof = false;
	stream->current = EOF;
	stream->line = 1;
	stream->column = 1;
	return stream->stream != NULL;
}

void stream_dtor(struct stream* stream)
{
	if (stream->stream) fclose(stream->stream);
	stream->stream = NULL;
}

int stream_peek(struct stream* stream)
{
	if (stream->eof) return EOF;
	if (stream->current == EOF) stream->current = fgetc(stream->stream);
	if (stream->current == EOF) stream->eof = true;
	return stream->current;
}

int stream_pop(struct stream* stream)
{
	int current = stream_peek(stream);
	stream->current = EOF;
	if (current == '\n')
	{
		++stream->line;
		stream->column = 1;
	}
	else if (current != EOF)
		++stream->column;
	return current;
}
