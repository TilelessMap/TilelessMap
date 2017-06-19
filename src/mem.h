#include <stddef.h>

/**
 *Memory handling
 * Just a function handling allocation error on malloc
 */
void* st_malloc(size_t len);


/**
 *Memory handling
 * Just a function handling allocation error on realloc
 */
void* st_realloc(void *ptr, size_t len);

/**
 *Memory handling
 * Just a function handling allocation error on calloc
 */
void* st_calloc(int n, size_t s);
