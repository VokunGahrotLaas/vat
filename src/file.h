#include "vatdef.h"

typedef enum VatFileMode {
	VatFileMode_Read = 1,
	VatFileMode_Write = 2,
	VatFileMode_Append = 4,
	VatFileMode_Truncate = 8,
	VatFileMode_Create = 16,
	VatFileMode_Binary = 32,
} VatFileMode;

typedef struct VatFile {
	char* path;
	VatFileMode mode;
	FILE* stream;
	size_t line;
	size_t column;
} VatFile;

VatFileMode VatFile_get_mode(char const* mode) {
	size_t len = strlen(mode);
	if (len == 0) vat_error("invalid file mode \"%s\"\n", mode);
	VatFileMode filemode = 0;

	if (mode[0] == 'r') filemode |= VatFileMode_Read;
	else if (mode[0] == 'w') filemode |= VatFileMode_Write | VatFileMode_Create | VatFileMode_Truncate;
	else if (mode[0] == 'a') filemode |= VatFileMode_Write | VatFileMode_Create | VatFileMode_Append;
	else vat_error("invalid file mode \"%s\"\n", mode);

	if (len == 1) return filemode;
	if (mode[1] == '+' || len >= 2 && mode[2] == '+') filemode |= VatFileMode_Read | VatFileMode_Write;
	if (mode[1] == 'b' || len >= 2 && mode[2] == 'b') filemode |= VatFileMode_Binary;
	return filemode;
}

VatFile* VatFile_new(char const* restrict path, char const* restrict mode) {
	VatFile* file = vat_xmalloc(sizeof(VatFile));
	file->path = path;
	file->mode = VatFile_get_mode(mode);
	file->stream = vat_xfopen(path, mode);
	file->line = 1;
	file->column = 0;
	return file;
}

char VatFile_get(VatFile* file) {
	char c = fgetc(file->stream);
	if (c == '\n') {
		file->line++;
		file->column = 0;
	} else {
		file->column++;
	}
	return c;
}

char VatFile_unget(VatFile* file, char c) {
	if (c == '\n') {
		vat_error("can't unget '\\n' yet\n");
		/* file->line--;
		file->column = ??; */
	} else {
		file->column--;
	}
	return ungetc(c, file->stream);
}
