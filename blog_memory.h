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
    BlogMemoryPage* page = (BlogMemoryPage*)VirtualAlloc(NULL, capacity + sizeof(BlogMemoryPage), MEM_COMMIT, PAGE_READWRITE);

    if (page == NULL) return NULL;
    
    page->capacity = capacity;
    page->child = NULL;
    page->next = NULL;

    return page;
#elif defined(__linux__)
    capacity = ALIGN(capacity, 4096);


    BlogMemoryPage* page = (BlogMemoryPage*)mmap(
        NULL, 
        capacity  + sizeof(BlogMemoryPage),
        PROT_READ | PROT_WRITE, 
        MAP_ANONYMOUS | MAP_PRIVATE, 
        -1, 
        0
    );

    if (page == MAP_FAILED) return NULL;

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
    VirtualFree(ptr, 0, MEM_RELEASE);
#elif defined(__linux__)
    munmap(ptr, ptr->capacity + sizeof(BlogMemoryPage));
#else
    #error "Unsupported OS"
#endif
}

static BlogMemoryPage* __blog_init_allocator(size_t capacity)
{
	if (__ALLOCATOR != NULL) return __ALLOCATOR;
    __ALLOCATOR = blog_request_memory_page(capacity);
    return __ALLOCATOR;
}

static BlogMemoryChunk* __blog_allocate_chunk_on_page(size_t capacity, BlogMemoryPage* page)
{
    size_t allocated = 0;
    BlogMemoryChunk* chunk = page->child;
    BlogMemoryChunk* last  = NULL;
    while (chunk) {
        allocated += chunk->capacity + sizeof(BlogMemoryChunk);
        if (!(chunk->flags & BLOCK_USE) && chunk->capacity >= capacity) {
            chunk->flags |= BLOCK_USE;
            return chunk + 1;
        }

        last = chunk;
        chunk = chunk->next;
    }

    if (allocated + sizeof(BlogMemoryChunk) + capacity >= page->capacity) {
        return NULL;
    }

    if (last == NULL) {
        BlogMemoryChunk *current_chunk = (BlogMemoryChunk*)(page + 1);
        current_chunk->capacity = capacity;
        current_chunk->flags = BLOCK_USE;
        current_chunk->next = NULL;
        page->child = current_chunk;

        return current_chunk;
    }


    BlogMemoryChunk *current_chunk = (last + 1);
    current_chunk->capacity = capacity;
    current_chunk->flags = BLOCK_USE;
    current_chunk->next = NULL;
    last->next = current_chunk;


    return current_chunk;
}

void* blog_malloc(size_t size)
{
    size = ALIGN(size, 8);
    if (__blog_init_allocator(size) == NULL) return NULL;

    BlogMemoryChunk* ptr = __blog_allocate_chunk_on_page(size, __ALLOCATOR);

    BlogMemoryPage* page = __ALLOCATOR;
    while(ptr == NULL) {
        if (page->next == NULL) {
            page->next = blog_request_memory_page(size + sizeof(BlogMemoryChunk));

            if (page->next == NULL) return NULL;
        }

        page = page->next;
        ptr = __blog_allocate_chunk_on_page(size, page);
    }

    return ptr + 1;
}

void blog_free(void* ptr)
{
    BlogMemoryChunk* current = ((BlogMemoryChunk*)ptr) - 1;
    current->flags = current->flags & (~BLOCK_USE);
}

#endif      // BLOG_MEMORY_IMPLEMENTATION

#endif      // BLOG_MEMORY_H
