#include "stream/stream.h"

bool stream_from_file(struct stream* stream, char const* filename)
{
	stream->stream = fopen(filename, "r");
	stream->filename = filename;
	stream->eol = true;
	stream->current = EOF;
	stream->pos = (struct pos){
		.filename = filename,
		.line = 0,
		.column = 0,
	};
	stream->peek_pos = (struct pos){
		.filename = filename,
		.line = 0,
		.column = 0,
	};
	return stream->stream != NULL;
}

void stream_dtor(struct stream* stream)
{
	if (stream->stream) fclose(stream->stream);
	stream->stream = NULL;
}

int stream_peek(struct stream* stream)
{
	if (stream->stream == NULL || stream->current != EOF) return stream->current;
	stream->current = fgetc(stream->stream);
	if (stream->eol)
	{
		++stream->peek_pos.line;
		stream->peek_pos.column = 1;
		stream->eol = false;
	}
	else
		++stream->peek_pos.column;
	if (stream->current == '\n')
		stream->eol = true;
	else if (stream->current == EOF)
	{
		fclose(stream->stream);
		stream->stream = NULL;
	}
	return stream->current;
}

int stream_pop(struct stream* stream)
{
	int current = stream_peek(stream);
	stream->pos = stream->peek_pos;
	stream->current = EOF;
	return current;
}
