#include "utils/vtype.h"

bool copy_fail(UNUSED void* ptr, UNUSED void const* other) { return false; }

bool move_fail(UNUSED void* ptr, UNUSED void* other) { return false; }
