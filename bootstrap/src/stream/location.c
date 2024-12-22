#include "stream/location.h"

void loc_print(struct loc const* loc, FILE* stream)
{
	fprintf(stream, "%s:", loc->filename);
	if (loc->first_line != loc->last_line)
		fprintf(stream, "%zu:%zu-%zu:%zu", loc->first_line, loc->first_column, loc->last_line, loc->last_column);
	else if (loc->first_column == loc->last_column)
		fprintf(stream, "%zu:%zu", loc->first_line, loc->first_column);
	else
		fprintf(stream, "%zu:%zu-%zu", loc->first_line, loc->first_column, loc->last_column);
}
