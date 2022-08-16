#pragma once

#include "vatdef.h"
#include "vector.h"

attr_nonnull(1)
void vat_cleanup_str(char** ptr) { vat_free(*ptr); }

#define vat_safe_str attr_cleanup(vat_cleanup_str) char*

attr_nonnull(1)
size_t vat_strip_end(char* str, bool (*pred)(char)) {
	size_t i;
	for (i = strlen(str); i > 0 && pred(str[i - 1]); --i) {}
	str[i] = '\0';
	return i;
}

#define vat_addr_temp(t, x) (&(struct {t _;}){(x)}._)

attr_returns_nonnull attr_wur
char const* vat_escaped_ascii(char c) {
	static char const* const vat_escaped_ascii_arr[] = {
		"\\0", "\\x01", "\\x02", "\\x03", "\\x04", "\\x05", "\\x06",
		"\\a", "\\b", "\\t", "\\n", "\\v", "\\f", "\\r",
		"\\x0E", "\\x0F", "\\x10", "\\x11", "\\x12", "\\x13", "\\x14", "\\x15", "\\x16", "\\x17", "\\x18", "\\x19", "\\x1A", "\\x1B", "\\x1C", "\\x1D", "\\x1E", "\\x1F",
		" ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
		"@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"[", "\\\\", "]", "^", "_", "`",
		"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
		"{", "|", "}", "~", "\\x7F",
	};
	size_t i = (unsigned char)c;
	if (i < 128)
		return vat_escaped_ascii_arr[i];
	else
		vat_error("can't escape non ascii char");
}

typedef char const* (*vat_escape_f)(char c);

attr_returns_nonnull attr_wur
char* vat_escaped_str(char const* str, vat_escape_f escape) {
	VatVector v; VatVector_ctor(&v);
	VatVector_reserve(char, &v, strlen(str));
	for (size_t i = 0; str[i] != '\0'; ++i) {
		char const* esc = escape(str[i]);
		VatVector_append(char, &v, esc, strlen(esc));
	}
	char* nstr = vat_xstrndup(v.data, v.size);
	VatVector_dtor(&v);
	return nstr;
}

#define vat_escaped_safe_str(str, escape) (vat_safe_str)vat_escaped_str(str, escape)
