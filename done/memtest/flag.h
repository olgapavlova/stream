#ifndef FLAG_H
#define FLAG_H

typedef enum flag_value {
	FLAG_VALUE_FAIL = 0,
	FLAG_VALUE_OK
} flag_value;

// Флаги вызова ошибки malloc()
void flag_malloc_ok(void);
void flag_malloc_fail(void);
flag_value flag_malloc_value(void);

#endif
