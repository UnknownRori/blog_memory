#ifndef     BLOG_MEMORY_H
#define     BLOG_MEMORY_H

#include <stddef.h>


void* blog_malloc(size_t size);
void blog_free(void* ptr);

#ifdef BLOG_MEMORY_IMPLEMENTATION

//-----------------------------------
// INFO : Import header
//-----------------------------------

#if defined(_WIN32) || defined(_WIN64)

#elif defined(__linux__)

#include <sys/mman.h>

#else
    #error "Unsupported OS"
#endif

//-----------------------------------
// INFO : Memory header
//-----------------------------------

typedef struct blog_memory_chunk {
    size_t  capacity;
    char    flags;

    struct blog_memory_chunk* child;
} BlogMemoryChunk;

typedef struct blog_memory_page {
    size_t  capacity;

    struct blog_memory_page* next;
    BlogMemoryChunk*         child;
} BlogMemoryPage;

//-----------------------------------
// INFO : Code Implementation
//-----------------------------------

void* blog_request_memory_page()
{
#if defined(_WIN32) || defined(_WIN64)
    // TODO : Implementation
#elif defined(__linux__)
    return NULL;
#else
    #error "Unsupported OS"
#endif
}

#endif

#endif      // BLOG_MEMORY_H
