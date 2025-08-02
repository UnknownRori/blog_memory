#ifndef     BLOG_MEMORY_H
#define     BLOG_MEMORY_H

#include <stddef.h>


typedef struct blog_memory_chunk {
    size_t  capacity;
    char    flags;

    struct blog_memory_chunk* child;
} BlogMemoryChunk;

typedef struct blog_memory_page {
    struct blog_memory_page* next;
    BlogMemoryChunk*         child;
} BlogMemoryPage;


#ifdef BLOG_MEMORY_IMPLEMENTATION
// our implementation detail
#endif

#endif      // BLOG_MEMORY_H
