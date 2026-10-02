#include <stddef.h>
#include <stdio.h>

void * __real_malloc(size_t c);

void * __wrap_malloc(size_t c) {
                     printf ("\tmalloc вызван с размером %zu\n", c);
                     // return __real_malloc(c);
                     return NULL;
}
