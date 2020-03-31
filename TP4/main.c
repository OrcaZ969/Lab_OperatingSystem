#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "mem.h"

int main()
{
    // initialize the allocator
    mem_init();
    mem_show_heap();
    //hello();
    
    char *p0 = mem_alloc( 42 ); 
    assert( p0!= NULL ); 
    mem_show_heap();
    char *p1 = mem_alloc( 60 ); 
    assert( p1!= NULL ); 
    char *p2 = mem_alloc( 60 ); 
    assert( p2!= NULL ); 
    char *p3 = mem_alloc( 60); 
    assert( p3!= NULL ); 
    mem_show_heap();

    mem_release(p0);
    mem_release(p3);
    mem_show_heap();
  
}
