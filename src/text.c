

/**********************************************************************
 *
 * TileLess
 *
 * TileLess is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * TileLess is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLess.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2017 Nicklas Avén
 *
 **********************************************************************/

#include <stdlib.h>
#include "log.h"
#include "text.h"

 
 TEXT* init_txt(size_t s)
{
    
    TEXT *t = malloc(sizeof(TEXT));
    if(!t)
    {
        log_this(100,"Failed to alloc memory in func %s",__func__);
        t->alloced = 0;
        return NULL;
    }    
    
    t->txt = malloc(s);
    if(!t->txt)
    {
        log_this(100,"Failed to alloc memory in func %s",__func__);
        t->alloced = 0;
        return NULL;
    }  

    t->alloced = s;
    t->used = 0;    
    
    return t;
}

static int realloc_txt(TEXT *t, size_t s)
{
    size_t new_s = t->alloced;
    
    while (new_s*=2 < s){};

    t->txt = realloc(t->txt, new_s);
    if(!t->txt)
    {
        log_this(100,"Failed to realloc memory in func %s",__func__);
        t->alloced = 0;
        return 1;
    }          
    t->alloced = new_s;    
    return 0;
}


int add_txt(TEXT *t,const char *in)
{
    
    size_t len = strlen(in);
    
    if((len + 1) > (t->alloced - t->used))
        if(realloc_txt(t, t->used + len + 1))
            return 1;
        
    strncpy(t->txt + t->used,  in, t->alloced - t->used);
    
    return 0;
}
char* get_txt(TEXT *t)
{
 return t->txt;   
}

int reset_txt(TEXT *t)
{
    t->used = 0;
    return 1;
}

int destroy_txt(TEXT *t)
{
    free(t->txt);
    t->alloced=0;
    t->used=0;
    free(t);
    t=NULL;
    return 0;
}
