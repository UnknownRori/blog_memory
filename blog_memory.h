#ifndef     BLOG_MEMORY_H
#define     BLOG_MEMORY_H

#include <stddef.h>


void* blog_malloc(size_t size);
void blog_free(void* ptr);

#ifdef BLOG_MEMORY_IMPLEMENTATION

//-----------------------------------
// INFO : Import header & macro & constants
//-----------------------------------

#if defined(_WIN32) || defined(_WIN64)

#elif defined(__linux__)

#include <sys/mman.h>

#else
    #error "Unsupported OS"
#endif

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

#define BLOCK_USE               0b10000000

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

BlogMemoryPage* blog_request_memory_page(size_t capacity)
{
#if defined(_WIN32) || defined(_WIN64)
    return NULL;
#elif defined(__linux__)
    return NULL;
#else
    #error "Unsupported OS"
#endif
}
void blog_free_memory_page(BlogMemoryPage* ptr)
{
#if defined(_WIN32) || defined(_WIN64)
    // TODO : Implementation
#elif defined(__linux__)
    // TODO : Implementation
#else
    #error "Unsupported OS"
#endif
}

#endif

#endif      // BLOG_MEMORY_H
