/**********************************************************************
 *
 * TileLessMap
 *
 * TileLessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TileLessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/

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

int st_free(void *s)
{
    if(s)
        free(s);
    return 0;
}
