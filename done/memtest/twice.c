#include <stdio.h>
#include <stdlib.h>
#include "goal.h"
#include "flag.h"

void mem_and_message(flag_value flag) {
  (flag == FLAG_VALUE_OK) ? flag_malloc_ok() : flag_malloc_fail();
  void * p = i_need_memory(30);
  flag_malloc_ok();

  printf("\tЗначение флага: %d\n", flag);
  printf("\t%s\n", (p == NULL) ? "Подмена!" : "Обычное выделение памяти");
  free(p);
}

int main(void) {
  mem_and_message(FLAG_VALUE_OK);
  mem_and_message(FLAG_VALUE_FAIL);
  return 0;
}
