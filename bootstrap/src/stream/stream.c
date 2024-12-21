#include "stream/stream.h"

bool stream_from_file(struct stream* stream, char const* filename)
{
	stream->stream = fopen(filename, "rb");
	stream->eof = false;
	stream->current = EOF;
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
	stream_peek(stream);
	int current = stream->current;
	stream->current = EOF;
	return current;
}
