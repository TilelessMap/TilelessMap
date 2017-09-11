
#include "theclient.h"

/**
 *Memory handling
 * Just a function handling allocation error on malloc
 */
void* st_malloc(size_t len)
{
    log_this(10, "Entering function %s",__func__);

    void *res = malloc(len);
    if(res==NULL)
    {
        fprintf(stderr, "Failed to allocate from heap\n");
        exit(EXIT_FAILURE);
    }
    return res;
}


/**
 *Memory handling
 * Just a function handling allocation error on realloc
 */
void* st_realloc(void *ptr, size_t len)
{
    log_this(10, "Entering function %s",__func__);

    void *res = realloc(ptr, len);
  //  printf("new pointer is %p old pointer is %p\n", res, ptr);
    if(res==NULL)
    {
        free(ptr);
        fprintf(stderr, "Failed to allocate from heap\n");
        exit(EXIT_FAILURE);
    }
    return res;
}


/**
 *Memory handling
 * Just a function handling allocation error on calloc
 */
void* st_calloc(int n, size_t s)
{
    log_this(10, "Entering function %s",__func__);

    void *res = calloc(n, s);
    if(res==NULL)
    {
        fprintf(stderr, "Failed to allocate from heap\n");
        exit(EXIT_FAILURE);
    }
    return res;
}

