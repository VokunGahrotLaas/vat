#pragma once

// libc
#include <assert.h>
#include <err.h>
#include <inttypes.h>
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
