#include "flag.h"

static int flag_malloc_should_fail = FLAG_VALUE_OK;
void flag_malloc_ok(void) { flag_malloc_should_fail = FLAG_VALUE_OK; }
void flag_malloc_fail(void) { flag_malloc_should_fail = FLAG_VALUE_FAIL; }
flag_value flag_malloc_value(void) { return flag_malloc_should_fail; }
