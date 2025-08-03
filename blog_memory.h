#ifndef     BLOG_MEMORY_H
#define     BLOG_MEMORY_H

#include <stddef.h>
#define BLOG_MEMORY_IMPLEMENTATION


void* blog_malloc(size_t size);
void blog_free(void* ptr);

#ifdef BLOG_MEMORY_IMPLEMENTATION

//-----------------------------------
// INFO : Import header & macro & constants
//-----------------------------------

#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h>

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

    struct blog_memory_chunk* next;
} BlogMemoryChunk;

typedef struct blog_memory_page {
    size_t  capacity;

    struct blog_memory_page* next;
    BlogMemoryChunk*         child;
} BlogMemoryPage;

BlogMemoryPage *__ALLOCATOR = NULL;

//-----------------------------------
// INFO : Code Implementation
//-----------------------------------

BlogMemoryPage* blog_request_memory_page(size_t capacity)
{
#if defined(_WIN32) || defined(_WIN64)
    capacity = ALIGN(capacity, 4096);
    BlogMemoryPage* page = (BlogMemoryPage*)VirtualAlloc(NULL, capacity, MEM_COMMIT, PAGE_READWRITE);

    if (page == NULL) return NULL;
    
    page->capacity = capacity;
    page->child = NULL;
    page->next = NULL;

    return page;
#elif defined(__linux__)
    capacity = ALIGN(capacity, 4096);

    BlogMemoryPage* page = (BlogMemoryPage*)mmap(
        NULL, 
        capacity, 
        PROT_READ | PROT_WRITE, 
        MAP_ANONYMOUS | MAP_PRIVATE, 
        -1, 
        0
    );

    if (page == NULL) return NULL;

    page->capacity = capacity;
    page->child = NULL;
    page->next = NULL;

    return page;
#else
    #error "Unsupported OS"
#endif
}

void blog_free_memory_page(BlogMemoryPage* ptr)
{
#if defined(_WIN32) || defined(_WIN64)
    VirtualFree(ptr, ptr->capacity, MEM_RELEASE);
#elif defined(__linux__)
    munmap(ptr, ptr->capacity);
#else
    #error "Unsupported OS"
#endif
}

void* blog_malloc(size_t size)
{
    size = ALIGN(size, 8);
    if (__ALLOCATOR == NULL) {
        __ALLOCATOR = blog_request_memory_page(size);
        if (__ALLOCATOR == NULL) return NULL;
    }

    size_t allocated = 0;
    BlogMemoryChunk* chunk = __ALLOCATOR->child;
    BlogMemoryChunk* last  = NULL;
    while (chunk) {
        allocated += chunk->capacity;
        if (!(chunk->flags & BLOCK_USE) && chunk->capacity >= size) {
            chunk->flags |= BLOCK_USE;
            return chunk + 1;
        }

        last = chunk;
        chunk = chunk->next;
    }

    if (allocated + sizeof(BlogMemoryChunk) + size >= __ALLOCATOR->capacity) {
        return NULL;
    }

    if (last == NULL) {
        BlogMemoryChunk *current_chunk = (BlogMemoryChunk*)(__ALLOCATOR + 1);
        current_chunk->capacity = size;
        current_chunk->flags = BLOCK_USE;
        current_chunk->next = NULL;
        __ALLOCATOR->child = current_chunk;

        return current_chunk + 1;
    }

    BlogMemoryChunk *current_chunk = (last + 1);
    current_chunk->capacity = size;
    current_chunk->flags = BLOCK_USE;
    current_chunk->next = NULL;
    last->next = current_chunk;

    return current_chunk + 1;
}

void blog_free(void* ptr)
{
    BlogMemoryChunk* current = ((BlogMemoryChunk*)ptr) - 1;
    current->flags = current->flags & (~BLOCK_USE);
}

#endif      // BLOG_MEMORY_IMPLEMENTATION

#endif      // BLOG_MEMORY_H
