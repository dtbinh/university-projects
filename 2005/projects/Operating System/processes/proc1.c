#include "include/crt.h"
#include "include/sys_calls.h"
int main() {
  int handle;
  int i;
  
  handle = crt_open();

  i = 0;
  while (i<1000) {
    crt_gotoxy(handle, 0, 0);
    crt_printf(handle, "i=%d", i);
    i++;
  }
  sys_0(SYS_EXIT);
  return 0;
}
