#include "ustack.h"
#include "kernel/types.h"
#include "user/user.h"

void test_ustack_malloc_and_free() {
  for (int i=0; i<100; i++){
    ustack_malloc(512);
    printf("malloc 512\n");
  }
  for (int i=0; i<100; i++){
     ustack_free();
     printf("free 512\n");
  }
  
}

int main() {
  test_ustack_malloc_and_free();
  return 0;
}
