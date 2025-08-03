#include <stdio.h>

#define BLOG_MEMORY_IMPLEMENTATION
#include "../blog_memory.h"

int main()
{
    char *ptr = (char*)blog_malloc(4);
    if (ptr == NULL) {
        printf("Memory allocation fail\n");
        return -1;
    }

    ptr[0] = 'H';
    ptr[1] = 'i';
    ptr[2] = '!';
    ptr[3] = '\0';
    printf("%s\n", ptr);

    blog_free(ptr);

    return 0;
}
