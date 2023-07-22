#include "ustack.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/riscv.h"

struct uheader* top = 0; //last allocated header 

void* ustack_malloc(uint len){
    if (len > MAX_ALLOC){
        return (void*)-1;
    }
    uint64 addr;
    if (top == 0){ //creating new page for the first time
        if ((addr = (uint64)sbrk(PGSIZE))==-1)
            return (void*)-1;
        top = (struct uheader*) addr;
        top->len = len;
        top->page_dealloc = PGSIZE-sizeof(uheader)-len;
        top->prev = 0;
    }
    else if ((len+sizeof(uheader)) <= top->page_dealloc){ //enough space in page
        struct uheader newHeader = {.len = len, .page_dealloc = (top->page_dealloc - sizeof(uheader) - len), .prev = top}; 
        int top_len = top->len;
        top = (struct uheader*)((uint64)top + top_len + sizeof(uheader));
        top->len = len;
        top->page_dealloc = newHeader.page_dealloc;
        top->prev = newHeader.prev;
    }
    else { //not enough space - create new page
        struct uheader newHeader = {.len = len, .page_dealloc = (PGSIZE+ top->page_dealloc -sizeof(uheader)-len), .prev = top}; 
        if ((uint64)sbrk(PGSIZE)==-1)
            return (void*)-1;
        top = (struct uheader*) ((uint64)top + top->len + sizeof(uheader));
        top->len = newHeader.len;
        top->page_dealloc = newHeader.page_dealloc;
        top->prev = newHeader.prev;
    }
    return (void*)top+sizeof(uheader);
}

int ustack_free(void){
    if(top == 0)
        return -1; 
    int len = top->len; 
    struct uheader* temp = top->prev;
    if(top->prev == 0
     || (uint64)top == PGROUNDDOWN((uint64)top) 
     || (uint64)((uint64)top+top->len+sizeof(uheader))>PGROUNDUP((uint64)top)){
            sbrk(-PGSIZE);
    }

    top = temp;
    return len;
}
