// libc
#include <err.h>
#include <stdio.h>
// bootstrap
#include "stream/stream.h"

int main(int argc, char** argv)
{
	if (argc != 2) errx(1, "invalid number of arguments");
	struct stream stream;
	stream_from_file(&stream, argv[1]);
	while (stream_peek(&stream) != EOF)
	{
		char c = (unsigned char)stream_pop(&stream);
		printf("%c", c);
	}
	stream_dtor(&stream);
	return 0;
}
