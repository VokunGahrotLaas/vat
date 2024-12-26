#pragma once

// bootstrap
#include "utils/utils.h"

#define HASH_SEED 0

uint64_t hash(void const* data, size_t size, uint64_t seed);
