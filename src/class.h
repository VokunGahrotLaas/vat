#pragma once

#include "vatdef.h"

typedef struct VatStructType {
	char* name;
	size_t size;
	void (*ctor)(void* self);
	void (*dtor)(void* self);
} VatStructType;

void VatStruct_delete(VatStructType const* type, void* obj) {
	if (obj == NULL) return;
	type->dtor(obj);
	vat_free(obj);
}

vat_gnu(malloc(VatStruct_delete, 2))
void* VatStruct_new(VatStructType const* type) {
	void* obj = vat_malloc(type->size);
	type->ctor(obj);
	return obj;
}
