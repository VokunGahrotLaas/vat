#pragma once

#include "vatdef.h"
#include "vector.h"

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
	bool opened;
} VatFile;

attr_nonnull(1) attr_pure attr_wur
VatFileMode VatFile_get_mode(char const* mode) {
	size_t len = strlen(mode);
	if (len == 0) vat_error("invalid file mode \"%s\"\n", mode);
	VatFileMode filemode = 0;

	if (mode[0] == 'r') filemode |= VatFileMode_Read;
	else if (mode[0] == 'w') filemode |= VatFileMode_Write | VatFileMode_Create | VatFileMode_Truncate;
	else if (mode[0] == 'a') filemode |= VatFileMode_Write | VatFileMode_Create | VatFileMode_Append;
	else vat_error("invalid file mode \"%s\"\n", mode);

	if (len == 1) return filemode;
	if (mode[1] == '+' || (len >= 2 && mode[2] == '+')) filemode |= VatFileMode_Read | VatFileMode_Write;
	if (mode[1] == 'b' || (len >= 2 && mode[2] == 'b')) filemode |= VatFileMode_Binary;
	return filemode;
}

void VatFile_delete(VatFile* file) {
	if (file == NULL) return;
	if (file->opened) fclose(file->stream);
	vat_free(file->path);
	vat_free(file);
}

attr_nonnull(1)
void VatFile_cleanup(VatFile** file) { VatFile_delete(*file); }

attr_nonnull(1, 2) attr_returns_nonnull attr_dealloc(VatFile_delete, 1) attr_wur
VatFile* VatFile_open(char const* restrict path, char const* restrict mode) {
	VatFile* file = vat_xmalloc(sizeof(VatFile));
	file->path = vat_xstrdup(path);
	file->mode = VatFile_get_mode(mode);
	file->stream = vat_xfopen(path, mode);
	file->line = 1;
	file->column = 0;
	file->opened = true;
	return file;
}

attr_nonnull(1, 2, 3) attr_returns_nonnull attr_dealloc(VatFile_delete, 1) attr_wur
VatFile* VatFile_new(FILE* stream, char const* restrict path, char const* restrict mode) {
	VatFile* file = vat_xmalloc(sizeof(VatFile));
	file->path = vat_xstrdup(path);
	file->mode = VatFile_get_mode(mode);
	file->stream = stream;
	file->line = 1;
	file->column = 0;
	file->opened = false;
	return file;
}

#define VatFile_safe attr_cleanup(VatFile_cleanup) VatFile* /* attribute cleanup delete */

attr_nonnull(1) attr_wur
char VatFile_get(VatFile* file) {
	if ((file->mode & VatFileMode_Read) == 0)
		vat_error("file is not open for reading\n");
	int c = fgetc(file->stream);
	if (c == EOF) {
		if (ferror(file->stream)) vat_error("error getting character in '%s'\n", file->path);
		return EOF;
	}
	if (c == '\n') {
		file->line++;
		file->column = 0;
	} else {
		file->column++;
	}
	return c;
}

attr_nonnull(1)
void VatFile_unget(VatFile* file, char c) {
	if ((file->mode & VatFileMode_Read) == 0)
		vat_error("file is not open for reading\n");
	if (c == '\n') {
		vat_error("can't unget '\\n' yet\n");
		/* file->line--;
		file->column = ??; */
	} else {
		file->column--;
	}
	if (ungetc(c, file->stream) == EOF)
		vat_error("error ungetting character in '%s'\n", file->path);
}

attr_nonnull(1) attr_wur
bool VatFile_eof(VatFile* file) { return feof(file->stream) != 0; }

attr_nonnull(1, 2)
void VatFile_skip_predicate(VatFile* file, bool (*predicate)(char)) {
	while (!VatFile_eof(file)) {
		char c = VatFile_get(file);
		if (!predicate(c)) {
			VatFile_unget(file, c);
			return;
		}
	}
}

char* VatFile_gets_predicate(VatFile* file, bool (*predicate)(char)) {
	VatVector* vec = VatVector_new();
	while (!VatFile_eof(file)) {
		char c = VatFile_get(file);
		if (!predicate(c)) {
			VatFile_unget(file, c);
			break;
		}
		VatVector_push(char, vec, &c);
	}
	char* str = vat_xstrndup(vec->data, VatVector_size(char, vec));
	VatVector_delete(vec);
	return str;
}

char* VatFile_gets_until_predicate(VatFile* file, bool (*predicate)(char)) {
	VatVector* vec = VatVector_new();
	while (!VatFile_eof(file)) {
		char c = VatFile_get(file);
		VatVector_push(char, vec, &c);
		if (predicate(c)) break;
	}
	char* str = vat_xstrndup(vec->data, VatVector_size(char, vec));
	VatVector_delete(vec);
	return str;
}
