#pragma once

#include "vatdef.h"

typedef struct VatVector {
	size_t size;
	size_t capacity;
	void* data;
} VatVector;

VatVector* VatVector_new(void) attr_returns_nonnull attr_malloc attr_wur;
void VatVector_delete(VatVector* self);
void VatVector_ctor(VatVector* self) attr_nonnull(1);
void VatVector_dtor(VatVector* self) attr_nonnull(1);
void* VatVector_get_(VatVector* self, size_t index) attr_nonnull(1) attr_returns_nonnull attr_wur;
void VatVector_resize_(VatVector* self, size_t size) attr_nonnull(1);
void VatVector_push_(VatVector* self, void const* data, size_t data_size) attr_nonnull(1, 2);
void* VatVector_pop_(VatVector* self, size_t data_size) attr_nonnull(1);

#define VatVector_get_safe_(self, index) ((0 <= (index) && (index) < (self)->size) ? VatVector_get((self), (index)) : vat_error("index out of bounds"))
#define VatVector_get(type, self, index) ((type*)VatVector_get_((self), (index) * sizeof(type)))
#define VatVector_get_safe(type, self, index) ((type*)VatVector_get_safe_((self), (index) * sizeof(type)))
#define VatVector_size(type, self) ((self)->size / sizeof(type))
#define VatVector_capacity(type, self) ((self)->capacity / sizeof(type))
#define VatVector_resize(type, self, size) VatVector_resize_((self), (size) * sizeof(type))
#define VatVector_push(type, self, data) VatVector_push_((self), (data), sizeof(type))
#define VatVector_pop(type, self) ((type*)VatVector_pop_((self), sizeof(type)))

VatVector* VatVector_new(void) {
	VatVector* self = vat_xmalloc(sizeof(VatVector));
	VatVector_ctor(self);
	return self;
}

void VatVector_delete(VatVector* self) {
	if (self == NULL) return;
	VatVector_dtor(self);
	vat_free(self);
}

void VatVector_ctor(VatVector* self) {
	self->size = 0;
	self->capacity = 0;
	self->data = NULL;
}

void VatVector_dtor(VatVector* self) {
	if (self->data == NULL) return;
	vat_free(self->data);
	self->data = NULL;
}

void* VatVector_get_(VatVector* self, size_t index) { return self->data + index; }

size_t VatVector_size_(VatVector* self) { return self->size; }

size_t VatVector_capacity_(VatVector* self) { return self->capacity; }

void VatVector_push_(VatVector* self, void const* data, size_t data_size) {
	if (self->size + data_size > self->capacity) {
		size_t new_capacity = self->capacity == 0 ? data_size : self->capacity * 2;
		self->data = vat_xrealloc(self->data, new_capacity);
		self->capacity = new_capacity;
	}
	memcpy(self->data + self->size, data, data_size);
	self->size += data_size;
}

void* VatVector_pop_(VatVector* self, size_t data_size) {
	if (self->size < data_size) vat_error("pop: array is too small");
	return self->data + (self->size -= data_size);
}
