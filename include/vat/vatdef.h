#pragma once

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef __GNUC__
#warning "This program is not compiled with GCC, it might not work"
#endif

#ifndef __STDC_VERSION__
#error "This stdc version is not supported"
#elif __STDC_VERSION__ < 202000L
#define PRE_STDC23
#else
#define STDC23
#endif

#define CAT_NX(lhs, rhs) lhs##rhs
#define CAT(lhs, rhs) CAT_NX(lhs, rhs)
#define STR_NX(x) #x
#define STR(x) STR_NX(x)

#define attr(x) __attribute__((x))
#define attr_aligned(x) attr(aligned(x))
#define attr_warn_if_not_aligned(x) attr(warn_if_not_aligned(x))
#define attr_alloc_size(x) attr(alloc_size(x))
#define attr_always_inline attr(always_inline) inline
#define attr_cleanup(func) attr(cleanup(func))
#define attr_copy(arg) attr(copy(arg))
#define attr_cold attr(cold)
#define attr_const attr(const)
#define attr_constructor attr(constructor)
#define attr_destructor attr(destructor)
#define attr_deprecated attr(deprecated)
#define attr_deprecated_msg(msg) attr(deprecated(msg))
#define attr_designated_init attr(designated_init)
#define attr_packed attr(packed)
#define attr_transparent_union attr(transparent_union)
#define attr_unavailable attr(unavailable)
#define attr_unavailable_msg(msg) attr(unavailable(msg))
#define attr_fd_arg(n) attr(fd_arg(n))
#define attr_fd_arg_read(n) attr(fd_arg_read(n))
#define attr_fd_arg_write(n) attr(fd_arg_write(n))
#define attr_flatten attr(flatten)
#define attr_format(arch, fmt_index, first_index) attr(format(arch, fmt_index, first_index))
#define attr_format_arg(fmt_index) attr(format_arg(fmt_index))
#define attr_hot attr(hot)
#define attr_interrupt_handler attr(interrupt_handler)
#define attr_leaf attr(leaf)
#define attr_malloc attr(malloc)
#define attr_dealloc(dealloc, pos) attr(malloc(dealloc, pos))
#define attr_dealloc_free attr_dealloc(free, 1)
#define attr_dealloc_fclose attr_dealloc(fclose, 1)
#define attr_dealloc_vat_free attr_dealloc(vat_free, 1)
#define attr_noinline attr(noinline)
#define attr_nonnull(...) attr(nonnull(__VA_ARGS__))
#define attr_nonstring attr(nonstring)
#define attr_noreturn attr(noreturn)
#define attr_nothrow attr(nothrow)
#define attr_pure attr(pure)
#define attr_returns_nonnull attr(returns_nonnull)
#define attr_unused attr(unused)
#define attr_used attr(used)
#define attr_warn_unused_result attr(warn_unused_result)
#define attr_wur attr_warn_unused_result
#define attr_fallthrough attr(fallthrough)

#ifdef VAT_PRE_STDC23
#define attr_c23(x) /* c23 attribute (disabled) */
#else
#define attr_c23(x) [[x]] /* c23 attribute (enabled) */
#endif

#ifdef VAT_DEBUG
#define vat_debug(fmt, ...) printf(fmt, ##__VA_ARGS__) /* vat debug printer (enabled) */
#else
#define vat_debug(fmt, ...) /* vat debug printer (disabled) */
#endif

#define vat_eq(lhs, rhs) (strcmp((lhs), (rhs)) == 0)
#define vat_beg(lhs, rhs) (strncmp((lhs), (rhs), strlen((rhs))) == 0)

#define vat_error(fmt, ...) do { fprintf(stderr, fmt, ##__VA_ARGS__); exit(EXIT_FAILURE); } while (0)
#define vat_perror(fmt, ...) do { fprintf(stderr, fmt, ##__VA_ARGS__); perror(NULL); exit(EXIT_FAILURE); } while (0)
#define vat_alloc_error() vat_error("%s: alloc failed\n", STR(__func__))

void vat_free(void* ptr) { free(ptr); }

attr_malloc attr_dealloc_vat_free attr_wur
void* vat_xmalloc(size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

attr_malloc attr_dealloc_vat_free attr_wur
void* vat_xcalloc(size_t count, size_t size) {
	void* ptr = calloc(count, size);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

attr_dealloc_vat_free attr_wur
void* vat_xrealloc(void* ptr, size_t size) {
	if (size == 0) {
		if (ptr != NULL) free(ptr);
		return NULL;
	}
	ptr = realloc(ptr, size);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

attr_returns_nonnull attr_nonnull(1, 2) attr_dealloc_fclose attr_wur
FILE* vat_xfopen(char const* restrict path, char const* restrict mode) {
	FILE* stream = fopen(path, mode);
	if (stream == NULL) vat_perror("failed to open file \"%s\"\n", path);
	return stream;
}

attr_returns_nonnull attr_nonnull(1) attr_dealloc_vat_free attr_wur
char* vat_xstrdup(char const* str) {
	char* ptr = strdup(str);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

attr_returns_nonnull attr_nonnull(1) attr_dealloc_vat_free attr_wur
char* vat_xstrndup(char const* str, size_t size) {
	char* ptr = strndup(str, size);
	if (ptr == NULL) vat_alloc_error();
	return ptr;
}

attr_nonnull(1) attr_noreturn
void vat_usage(char* name, bool failure) {
	printf(
		"Usage: %s <input file> [OPTIONS]\n"
		"\nOptions:\n"
		"  -h/--help        displays this usage page\n"
		"  -v/--version     displays program version\n"
	, name);
	exit(failure ? EXIT_FAILURE : EXIT_SUCCESS);
}

attr_noreturn
void vat_version(void) {
	printf("Version: alpha\n");
	exit(EXIT_SUCCESS);
}

attr_nonnull(1)
void vat_cleanup_file(FILE** ptr) {
	FILE* f = *ptr;
	if (f == NULL || f == stdin || f == stdout || f == stderr) return;
	vat_debug("cleaned file\n");
	fclose(f);
}

#define vat_safe_file attr_cleanup(vat_cleanup_file) FILE* /* attribute cleanup with cleanup_file */

attr_nonnull(1)
void vat_cleanup_free(void** ptr) { vat_free(*ptr); }

#define vat_safe(type) attr_cleanup(vat_cleanup_free) type

#define vat_not_found ((size_t)-1)
