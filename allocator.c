// allocator.c
#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#define METADATA_SIZE sizeof(struct block)
int count = 0;
/**Instantiates the heap on first custom_malloc call**/
struct block* createfirstblock(){

    struct block* firstblock;
    firstblock = (struct block*) sbrk(0);
    int breakpoint = (int) sbrk(1<<MAX_EXP);
    int position = 0;

    if(breakpoint < 0){
      return NULL;
    }

    firstblock->data = firstblock + 1;
    firstblock->free = true;
    firstblock->size = (1<<MAX_EXP);
    firstblock->next = NULL;
    firstblock->buddy = NULL;

    while(position < MAX_EXP) {
      firstblock->merge_buddy[position++] = NULL;
    }

    return firstblock;

}

/**Round up requested size to smallest power of 2**/
size_t getminimumsize(size_t size){

    for(int exponent = 0; exponent < MAX_EXP; exponent++){
        if(((1<<exponent) >= size)){
            size = (1<<exponent);
            break;
        }
    }
    return size;
}

/**Returns a free block big enough to accomodate requested size**/
struct block* getblock(size_t size){

    struct block* block;

    for(block = head; block != NULL; block = block->next){
        if((block->free) && (block->size >= size))
            break;
    }

    return block;
}

/**Divides block to fit requested size**/
void bisect(struct block* oldblock, size_t size){

    struct block* newblock;
    bool done = false;
    while(oldblock->size != size){
        //Set properties for new block after bisection
        newblock = oldblock->data + (oldblock->size)/2 - METADATA_SIZE;
        newblock->size = oldblock->size/2;
        newblock->free = true;
        newblock->next = oldblock->next;
        newblock->data = newblock + 1;
        newblock->buddy = oldblock;

        /**New block**/
        for(int i = 0; i < MAX_EXP; i++){
            newblock->merge_buddy[i] = NULL;
        }
        /**Old block**/
        for(int i = 0; (i < MAX_EXP) && !done; i++){
            if(!(newblock->merge_buddy[i])){
                newblock->merge_buddy[i] = newblock->buddy;
                done = true;
            }
        }
        done = false;
        oldblock->buddy = newblock;
        for(int i = 0; (i < MAX_EXP) && !done; i++){
            if(!(oldblock->merge_buddy[i])){
                oldblock->merge_buddy[i] = oldblock->buddy;
                done = true;
            }
        }
        done = false;
        //Updates old block
        oldblock->next = newblock;
        oldblock->size /= 2;
    }
    oldblock->free = false;

}

/**Merges free buddy blocks**/
void merge(struct block* block){

    struct block* current;

    /*
    while(block){
        if((block->next->free) && (block->next->size == block->size)){
          //buddy on left
          if(block > block->buddy){
             block->buddy = block->next;
             block = block->buddy;
             block->next = block->next;
          }
          else{
             block->buddy = block->buddy->buddy;
             block->next = block->buddy->next;
          }

          block->size *= 2;
          block->free = true;

        }
        block = block->next;
    }*/

}

/** Allocates a block of at least the requested size **/
void *custom_malloc(size_t size){

  struct block* block;

  if((size <= 0) || (size > (1<<MAX_EXP))){
     return NULL;
  }
  //create first big block
  if(head == NULL){

     block = createfirstblock();
     head = block;

  }

  size += METADATA_SIZE;
  if((size <= (1<<MAX_EXP))){
     size = getminimumsize(size);
  }

  block = getblock(size);

  if((block != NULL) && (block->size > size)){
      bisect(block, size);
  }
  else if(block->size == size){
      block->free = false;
  }

  return (block->data);
}

/** Mark a data block as free and merge free buddy blocks **/
void custom_free(void *p){

    struct block* block = ((struct block*)p - 1);

    if(block){
       block->free = true;
       if(block->buddy->free){
          //merge(block);
       }
    }
    else{
       return;
    }
}

/** Changes the memory allocation of the data to have at least the requested size **/
void *custom_realloc(void *p, size_t size){

    size += METADATA_SIZE;
    size = getminimumsize(size);

    struct block* block;
    block = ((struct block*)p - 1);
    void* pointer;

    if(!p){
      return custom_malloc(size);
    }
    else if(block->size == size){
      return (void*)block;
    }
    else if(block->size >= (size<<1)){
      void* data = block->data;
      size_t blocksize = block->size;
      bisect(block, size);
      //size = blocksize - METADATA_SIZE;
      memcpy(block->data, data, size);
      return block;
    }
    else{
      size = block->size - METADATA_SIZE;
      pointer = custom_malloc(size);
      memcpy(((struct block*)pointer - 1)->data, block->data, size);
      custom_free(p);
      return pointer;
    }
    return p;
}

/*------------------------------------*\
|            DEBUG FUNCTIONS           |
\*------------------------------------*/

/** Prints the metadata of a block **/
void print_block(struct block *b){

    if(!b){
        printf("NULL block\n");
    }
    else {
        int i = 0;
        printf("Start = %p\n", b);
        printf("Size = %lu bytes\n", b->size);
        printf("Free = %s\n", (b->free) ? "true":"false");
        printf("Data = %p\n", b->data);
        printf("Next = %p\n", b->next);
        printf("Buddy = %p\n", b->buddy);
        printf("Merge Buddies = ");

        while(b->merge_buddy[i] && i < MAX_EXP) {
            printf("%p ", b->merge_buddy[i]);
            i++;
        }
        printf("\n\n");
    }
}

/** Prints the metadata of all blocks **/
void print_list(){

    struct block *current = head;
    printf("--HEAP--\n");
    if(!head){
      printf("EMPTY\n");
    }

    while(current){
        print_block(current);
        current = current->next;
    }
    printf("--END--\n");
}
