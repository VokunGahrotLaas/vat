#include "stream/stream.h"

bool stream_from_file(struct stream* stream, char const* filename)
{
	stream->stream = fopen(filename, "r");
	stream->eof = false;
	stream->eol = true;
	stream->current = EOF;
	stream->line = 0;
	stream->column = 0;
	stream->peek_line = 0;
	stream->peek_column = 0;
	return stream->stream != NULL;
}

void stream_dtor(struct stream* stream)
{
	if (stream->stream) fclose(stream->stream);
	stream->stream = NULL;
}

int stream_peek(struct stream* stream)
{
	if (stream->eof || stream->current != EOF) return stream->current;
	stream->current = fgetc(stream->stream);
	if (stream->eol)
	{
		++stream->peek_line;
		stream->peek_column = 1;
		stream->eol = false;
	}
	else
		++stream->peek_column;
	if (stream->current == '\n')
		stream->eol = true;
	else if (stream->current == EOF)
		stream->eof = true;
	return stream->current;
}

int stream_pop(struct stream* stream)
{
	int current = stream_peek(stream);
	stream->line = stream->peek_line;
	stream->column = stream->peek_column;
	stream->current = EOF;
	return current;
}
