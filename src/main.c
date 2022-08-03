#include "vatdef.h"
#include "class.h"

typedef struct VatString {
	size_t size;
	char* data;
} VatString;

vat_nonnull void VatString_ctor(void* _self) {
	VatString* self = _self;
	self->size = 0;
	self->data = vat_malloc(self->size + 1);
	self->data[0] = '\0';
}

void VatString_dtor(void* _self) {
	VatString* self = _self;
	vat_free(self->data);
}

VatStructType const* const VatString_t = &(VatStructType const){ .name = "array", .size = sizeof(VatString), .ctor = VatString_ctor, .dtor = VatString_dtor };

vat_nonnull void VatString_append(void* _self, char const* str) {
	VatString* self = _self;
	self->data = vat_realloc(self->data, (self->size += strlen(str)) + 1);
	strcat(self->data, str);
}

vat_nonnull void VatString_puts(void* _self) {
	VatString* self = _self;
	fputs(self->data, stdout);
}

int main(vat_unused int argc, vat_unused char** argv) {
	vat_safe_file input = fopen("main.vat", "r"); // stdin;
	vat_unused vat_safe_file output = stdout;

	/*
	if (argc <= 1) vat_usage(argc >= 1 ? *argv : "vat", true);
	if (argc > 1) {
		if (vat_eq(argv[1], "-h") || vat_eq(argv[1], "--help")) vat_usage(*argv, false);
		else if (vat_eq(argv[1], "-v") || vat_eq(argv[1], "--version")) vat_version();
		else if (vat_eq(argv[1], "-")) input = stdin;
		else input = fopen(argv[1], "r");
	}
	*/

	VatString* str = VatStruct_new(VatString_t);

	VatString_append(str, "Hello World!\n");
	VatString_puts(str);

	VatStruct_delete(VatString_t, str);

	if (input == NULL) vat_error("failed to open input file\n");

	return EXIT_SUCCESS;
}
