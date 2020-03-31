#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

#include "mem.h"

void * heap_base = NULL;// first byte of the heap
void * heap_end  = NULL;// first byte beyond the heap

int mem_initialized = 0;

void hello(){
    printf("hello");
}

// initialize the memory manager
/*void mem_init(void)
{
    assert(mem_initialized == 0);

    // request memory from the kernel
    heap_base = mmap(NULL, 800, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    assert(heap_base != MAP_FAILED);

    heap_end = heap_base + 800 ;

    // create some free blocks: five of 80 bytes and one of 400 bytes
    void *block_ptr;
    for(int i=0; i<5;i++)
    {
        block_ptr = heap_base + 80*i;
        *( (int64_t*)block_ptr ) = 80;//mark that 80 bytes
    }
    block_ptr = heap_base + 400;
    *( (int64_t*)block_ptr ) = 400;
    
    mem_initialized = 1;
}
*/
//TODO:4 nouveau fonction d'initialisation

void mem_init(void){
    assert(mem_initialized==0);
    heap_base = mmap(NULL, 800, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    assert(heap_base!=MAP_FAILED);
    heap_end=heap_base+800;

    void *block_ptr;
    block_ptr=heap_base;
    *((int64_t*)block_ptr)=800;//header
    
    *(((int64_t*)heap_end-8))=800;//footer
    mem_initialized = 1;
}



void * mem_alloc(int64_t length)
{ 
    printf("in memory alloc\n");
    assert(mem_initialized == 1);
    
    
    // compute actual size of block
    length = (length+7)/8*8 ; // round up to nearest multiple of 8
    length += 16;              // add space for the header and footer

    // heap traversal
    void *  block_ptr ;
    //void *  footer_ptr;
    int64_t header ;
    //int64_t footer ;
    int64_t size;   
    char    flags;
    
    block_ptr = heap_base;
    while(block_ptr < heap_end)
    {
        header = *( (int64_t*)block_ptr );
        flags  = header & 0b111;  // keep only three least significant bits
        size = header & ~0b111;   // discard the three least significant bits

        if( (flags == 0 ) &&      // if current block is free, and
            (size >= length))     // is also large enough, then we have a winner
            break;

        // otherwise we move on to the next block
        block_ptr += size;
    }

    // if the heap  traversal reached this far, then it  means we have
    // found no suitable block, so we should return NULL
    if(block_ptr >= heap_end)
    {
        return NULL;
    }
    // in the case a appropriate block is found
    flags = 0b001; // mark block as taken
    
    //header = size | flags;
    //TODO: 3 Decouper les blocks
        
    if(size-length>16){
        // it's meaningful to create new blocks
        header=length|flags;
        // creation du nouveau block
        void* new_block_ptr = block_ptr + length;
        *( (int64_t*)new_block_ptr ) = size-length;//new block header
        //TODO: 4 Ajouter des footer
        *( (int64_t*)(new_block_ptr+size-length-8))=size-length;//new block footer          
    }else{//sinon on fait traditionnellement
        header=size|flags;
    }

    *( (int64_t*)block_ptr ) = header ; // write header back into the block
    //TODO: 4 Ajouter des footer
    *( (int64_t*)(block_ptr+length-8))=header;//footer
    return block_ptr + 8 ; // skip header
}

int if_prec_occup(void *ptr)
{
    void* header_ptr=ptr-8;
    if(header_ptr==heap_base) return 1;
    void* pre_footer_ptr=header_ptr-8;
    return *((int64_t*)pre_footer_ptr)%2;
}

int if_suiv_occup(void *ptr)
{
    void* header_ptr=ptr-8;
    int64_t header=*(int64_t*)(header_ptr);
    int64_t size = header & ~0b111;
    void* suiv_header_ptr=header_ptr+size;
    if(suiv_header_ptr==heap_end) return 1;
    return *((int64_t*)suiv_header_ptr)%2;
}


void mem_release_basic(void *ptr)
{
    assert( mem_initialized == 1);
    assert( ((int64_t)ptr % 8) == 0 ); // sanity check

    // TODO: 2 Recyclage des blocs désalloués
    void* header_ptr=ptr-8;
    int64_t header=*((int64_t*)header_ptr);
    int64_t size = header & ~0b111;   // discard the three least significant bits
    *((int64_t*)header_ptr)=size;
    //TODO: 4 Changer les footers associes
    void* footer_ptr=header_ptr+size-8;
    *((int64_t*)footer_ptr)=size;

    return;
}


void mem_release(void *ptr)
{
    assert( mem_initialized == 1);
    assert( ((int64_t)ptr % 8) == 0 ); // sanity check

    //TODO: 5 Défragmentation du tas par fusion des bloc
    if(if_prec_occup(ptr)&&if_suiv_occup(ptr)){
        mem_release_basic(ptr);    
    }
    else if(!if_prec_occup(ptr)&&!if_suiv_occup(ptr)){
        //fusionner les trois
        void* this_header_ptr=ptr-8;
        int64_t this_header=*((int64_t*)this_header_ptr);
        int64_t this_size = this_header & ~0b111;

        void* suiv_header_ptr=this_header_ptr+this_size;
        int64_t suiv_header=*((int64_t*)suiv_header_ptr);
        int64_t suiv_size= suiv_header& ~0b111;

        void* prec_footer_ptr=this_header_ptr-8;
        int64_t prec_footer=*((int64_t*)prec_footer_ptr);
        int64_t prec_size=prec_footer& ~0b111;

        int64_t total_size=this_size+suiv_size+prec_size;
        char flags=0b000;

        void* prec_header_ptr=this_header_ptr-prec_size;
        void* suiv_footer_ptr=suiv_header_ptr+suiv_size-8;
         
        int64_t new_header_footer=total_size|flags;
        
        *((int64_t*) prec_header_ptr)=new_header_footer;
        *((int64_t*) suiv_footer_ptr)=new_header_footer;
        //wtf its bug free!!!!!  
    }
    else if(!if_prec_occup(ptr)&&if_suiv_occup(ptr)){
        //fusionner celuici et le precedant
        void* this_header_ptr=ptr-8;
        int64_t this_header=*((int64_t*)this_header_ptr);
        int64_t this_size = this_header & ~0b111;

        void* prec_footer_ptr=this_header_ptr-8;
        int64_t prec_footer=*((int64_t*)prec_footer_ptr);
        int64_t prec_size=prec_footer& ~0b111;

        int64_t total_size=this_size+prec_size;
        char flags=0b000;

        void* prec_header_ptr=this_header_ptr-prec_size;
        void* this_footer_ptr=this_header_ptr+this_size-8;
        
        int64_t new_header_footer=total_size|flags;

        *((int64_t*) prec_header_ptr)=new_header_footer;
        *((int64_t*) this_footer_ptr)=new_header_footer;

    }else if(if_prec_occup(ptr)&&!if_suiv_occup(ptr)){
        //fusionner celuici et le suivant
         void* this_header_ptr=ptr-8;
        int64_t this_header=*((int64_t*)this_header_ptr);
        int64_t this_size = this_header & ~0b111;

        void* suiv_header_ptr=this_header_ptr+this_size;
        int64_t suiv_header=*((int64_t*)suiv_header_ptr);
        int64_t suiv_size= suiv_header& ~0b111;

        int64_t total_size=this_size+suiv_size;
        char flags=0b000;

         void* suiv_footer_ptr=suiv_header_ptr+suiv_size-8;

          int64_t new_header_footer=total_size|flags;
        *((int64_t*) this_header_ptr)=new_header_footer;
        *((int64_t*) suiv_footer_ptr)=new_header_footer;
}
}





void mem_show_heap(void)
{
    assert( mem_initialized == 1);
    
    void * block_ptr = heap_base;

    printf("heap_base = %p\n",heap_base);
    while(block_ptr < heap_end)
    {
        int64_t header = *( (int64_t*)block_ptr );
        
        char    flags  = header & 0b111;  //   keep only three least significant bits
        int64_t size   = header & ~0b111; // discard the three least significant bits
        //TODO:4 show footer        
        int64_t footer =*((int64_t*)(block_ptr+(int)size-8));
        void * footer_ptr=block_ptr+(int)size-8;
       
        if( (size < 8) ||
            (size%8 != 0) )
        {
            printf("error: block at %p has incorrect size %ld\n",block_ptr,size);
            exit(1);
        }

        printf("  block at %p: header=0x%08lx size=%ld flags=%d (%s)\n",
               block_ptr,header,size,flags,
               (flags ? "taken" : "free")
               );
        //TODO:4. show footer
        printf("    end at %p: footer=0x%08lx size=%ld flags=%d (%s)\n",
               footer_ptr,footer,size,flags,
               (flags ? "taken" : "free")
               );

        block_ptr += size; // move on to next block
    }
  
    printf("heap_end = %p\n",heap_end);
    //sanity check: a full heap traversal should reach *exactly* the end
    assert( block_ptr == heap_end); 
}
