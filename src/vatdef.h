#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef __GNUC__
#warning "This program is not compiled with GCC, it might not work"
#endif

#ifndef __STDC_VERSION__
#error "This stdc version is not supported"
#elif __STDC_VERSION__ < 202000L
#define VAT_PRE_STDC23
#else
#define VAT_STDC23
#endif

#define VAT_CAT_NX(lhs, rhs) lhs##rhs
#define VAT_CAT(lhs, rhs) VAT_CAT_NX(lhs, rhs)
#define VAT_STR_NX(x) #x
#define VAT_STR(x) VAT_STR_NX(x)

#ifdef VAT_PRE_STDC23
#define vat_gnu(x) __attribute__((x))
#define vat_unused vat_gnu(unused)
#define vat_noreturn vat_gnu(noreturn)
#else
#define vat_gnu(x) [[gnu::x]]
#define vat_unused [[maybe_unused]]
#define vat_noreturn [[gnu::noreturn]]
#endif

#define vat_nonnull vat_gnu(nonnull)
#define vat_nonnull_idx(...) vat_gnu(nonnull(__VA_ARGS__))

#ifdef VAT_DEBUG
#define vat_debug(fmt, ...) printf(fmt, ##__VA_ARGS__) /* vat debug printer (enabled) */
#else
#define vat_debug(fmt, ...) /* vat debug printer (disabled) */
#endif

#define vat_eq(lhs, rhs) (strcmp((lhs), (rhs)) == 0)
#define vat_beg(lhs, rhs) (strncmp((lhs), (rhs), strlen((rhs))) == 0)

vat_gnu(format(printf, 1, 2)) vat_noreturn
void vat_error(char const* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

vat_noreturn
void vat_alloc_error(void) { vat_error("%s: alloc failed\n", VAT_STR(__func__)); };

void vat_free(void* ptr) { free(ptr); }

vat_gnu(malloc(vat_free))
void* vat_malloc(size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

vat_gnu(malloc(vat_free))
void* vat_calloc(size_t count, size_t size) {
	void* ptr = calloc(count, size);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

vat_gnu(malloc(vat_free))
void* vat_realloc(void* ptr, size_t size) {
	if (size == 0) return NULL;
	ptr = realloc(ptr, size);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

vat_nonnull vat_noreturn
void vat_usage(char* name, bool failure) {
	printf(
		"Usage: %s <input file> [OPTIONS]\n"
		"\nOptions:\n"
		"  -h/--help        displays this usage page\n"
		"  -v/--version     displays program version\n"
	, name);
	exit(failure ? EXIT_FAILURE : EXIT_SUCCESS);
}

vat_noreturn
void vat_version(void) {
	printf("Version: alpha\n");
	exit(EXIT_SUCCESS);
}

vat_nonnull
void vat_cleanup_file(FILE** ptr) {
	FILE* f = *ptr;
	if (f == NULL || f == stdin || f == stdout || f == stderr) return;
	vat_debug("cleaned file\n");
	fclose(f);
}

#define vat_safe_file vat_gnu(cleanup(vat_cleanup_file)) FILE* /* GNU attribute cleanup with cleanup_file */
