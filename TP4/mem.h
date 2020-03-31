#ifndef _MEM_H_
#define _MEM_H_

#include <stdint.h>

// initialize the memory manager
//
// this function must be called just once, before any calls to mem_alloc or mem_release
void mem_init(void);

// allocate a region of memory and return its address (or NULL in case of error)
void * mem_alloc(int64_t length);

// deallocate a region of memory
void mem_release(void* ptr);

// aux function for mem_release
void mem_release_basic(void* ptr);

int if_prec_occup(void* ptr);

int if_suiv_occup(void* ptr);

// pretty-print the heap (useful for debug)
void mem_show_heap();

void hello(void);

#endif /* _MEM_H_ */
