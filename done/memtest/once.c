#include <stdio.h>
#include <stdlib.h>
#include "goal.h"

int main(void) {
  void * p = i_need_memory(30);
  printf("\t%s\n", (p == NULL) ? "Подмена!" : "Обычное выделение памяти");
  free(p);
  return 0;
}
