#include <lcom/lcf.h>

#ifdef LAB3
  int cnt = 0;
#endif

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t value32;

  if (value == NULL || sys_inb(port, &value32)) return 1;

  *value = value32;

  #ifdef LAB3
    cnt++;
  #endif

  return 0;
}
