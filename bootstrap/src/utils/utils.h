#pragma once

// libc
#include <assert.h>
#include <err.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>

#define ATTR(...) __attribute__((__VA_ARGS__))
#define FALLTHROUGH                                                                                                    \
	NOOP();                                                                                                            \
	ATTR(fallthrough)
#define UNUSED ATTR(unused)
#define EXPORT ATTR(visibility("default"))

#define NOOP() (void)0
#define UNREACHABLE() __builtin_unreachable()

#ifdef BOOTSTRAP_DEBUG
#	define DBG_ASSERT(...) assert(__VA_ARGS__)
#else
#	define DBG_ASSERT(...) /**/
#endif

#define MAX(A, B) ((A) >= (B) ? (A) : (B))
#define MIN(A, B) ((A) <= (B) ? (A) : (B))
#define STR(A) #A
#define STRX(A) STR(A)
#define CAT(A, B) A##B
#define CATX(A, B) CAT(A, B)
#define ALIGN(Offset, Align) ((Offset) + ((Offset) % (Align) == 0 ? 0 : (Align) - (Offset) % (Align)))
