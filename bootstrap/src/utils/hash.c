#include "utils/hash.h"

// komihash
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <komihash.h>
#pragma GCC diagnostic pop

uint64_t hash(void const* data, size_t size, uint64_t seed) { return komihash(data, size, seed); }
