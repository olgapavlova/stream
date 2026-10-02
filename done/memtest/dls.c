#define _GNU_SOURCE
#include <stddef.h>
#include <dlfcn.h>
#include "flag.h"

// real_malloc — это глобальная переменная в пределах библиотеки
static void * (*real_malloc)(size_t);

void * malloc(size_t size) {
    // определяем real_malloc только один раз, при первом вызове malloc
    if (real_malloc == NULL)
        real_malloc = dlsym(RTLD_NEXT, "malloc");  // а вдруг тут рекурсия?!

    return (flag_malloc_value() == FLAG_VALUE_FAIL) ? NULL : real_malloc(size);
}
