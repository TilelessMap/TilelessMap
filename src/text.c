

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
#include "mem.h"


TEXT* init_txt(size_t s)
{

    TEXT *t = st_malloc(sizeof(TEXT));

    t->txt = st_malloc(s);

    t->alloced = s;
    t->used = 0;

    return t;
}

static int realloc_txt(TEXT *t, size_t s)
{
    size_t new_s = t->alloced;

    while (new_s < s) 
    {
        new_s*=2;
    };

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

    strncpy(t->txt + t->used, in, len + 1);
    t->used += len;
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

WCHAR_TEXT* init_wc_txt(size_t s)
{

    WCHAR_TEXT *t = malloc(sizeof(WCHAR_TEXT));
    if(!t)
    {
        log_this(100,"Failed to alloc memory in func %s",__func__);
        t->alloced = 0;
        return NULL;
    }

    t->txt = malloc(sizeof(uint32_t) * s);
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

static int realloc_wc_txt(WCHAR_TEXT *t)
{
    size_t new_s = t->alloced * 2;


    t->txt = realloc(t->txt, new_s * sizeof(uint32_t));
    if(!t->txt)
    {
        log_this(100,"Failed to realloc memory in func %s",__func__);
        t->alloced = 0;
        return 1;
    }
    t->alloced = new_s;
    return 0;
}


int add_utf8_2_wc_txt(WCHAR_TEXT *t,const char *in)
{

    uint32_t p = 0;

    const char *u = in;
    while(*u) {

        p = utf82unicode(u,&u);




        if(t->alloced-t->used < 1)
        {
            if(realloc_wc_txt(t))
                return 1;
        }
        t->txt[t->used] = p;
        t->used++;
    }
    return 0;
}

int reset_wc_txt(WCHAR_TEXT *t)
{
    t->used = 0;
    return 1;
}

int destroy_wc_txt(WCHAR_TEXT *t)
{
    free(t->txt);
    t->alloced=0;
    t->used=0;
    free(t);
    t=NULL;
    return 0;
}



uint32_t utf82unicode(const char *text,const char **the_rest)
{
    uint32_t res = 0;
    uint8_t nbytes=0;
    uint8_t c;
    int i =0;

    c = text[0];
    if(c==0)
    {
        return 0;
    }
    else if(!(c & 128))
    {
        res = (uint32_t) c;
        *(the_rest) = text+1;
        return res;
    }

    while((c<<++nbytes) & 128)
    {};

    *(the_rest) = text+nbytes;

    /*TODO: test this "thing". */
    res = ((c<<nbytes) & 0xff)>>nbytes;

    for (i=1; i<nbytes; i++)
    {
        c = text[i];
        if(c==0)
        {
            printf("Something went wrong, UTF is invalid\n");
            return 0;
        }
        res = (res<<6) | (c & 63);
    }
    return res;
}


TEXTBLOCK* init_textblock(size_t s)
{
    unsigned int i;
    TEXTBLOCK *tb = st_malloc(sizeof(TEXTBLOCK));
    tb->txt = st_malloc(s * sizeof(TEXT*));
    for (i=0;i<s;i++)
    {
        tb->txt[i] = init_txt(32);
    }
    
    tb->font = st_malloc(s * sizeof(ATLAS*));
    tb->max_n_txts = s;
    tb->n_txts = 0;
    return tb;
}

static int realloc_textblock(TEXTBLOCK *tb)
{
    int i;
    int new_size = tb->max_n_txts * 2;
    tb->txt = st_realloc(tb->txt, new_size * sizeof(TEXT*));
    for (i=tb->max_n_txts;i<new_size;i++)
    {
        tb->txt[i] = init_txt(32);
    }
    tb->font = st_realloc(tb->font, new_size * sizeof(ATLAS*));

    tb->font = st_realloc(tb->font, new_size * sizeof(ATLAS*));
    tb->max_n_txts = new_size;
    return 0;
}

int destroy_textblock(TEXTBLOCK *tb)
{
    int i;
    for (i=0;i<tb->n_txts;i++)
    {
        destroy_txt(tb->txt[i]);
    }
    
    free(tb->font);
    tb->font = NULL;
    tb->max_n_txts = 0;
    tb->n_txts = 0;
    free(tb);
    tb = NULL;
    return 0;
}

int append_2_textblock(TEXTBLOCK *tb, const char* txt, ATLAS *font)
{
    if(! (tb->n_txts < tb->max_n_txts))
        realloc_textblock(tb);
    
    add_txt(tb->txt[tb->n_txts], txt);
    
    tb->font[tb->n_txts] = font;
    
    tb->n_txts++;
 

    return 0;
    
}





int init_txt_coords()
{
 /*TODO This is just temporary
 * Later there will be something holding all txt_coordinates from all layers and controls
 * and all of it will be rendered from there. */
 
    const size_t init_size = 2048;
    txt_coords = st_malloc(sizeof(TEXTCOORDS));
    txt_coords->coords = st_malloc(init_size * sizeof(POINT_T));
    txt_coords->alloced = init_size;
    txt_coords->used = 0;    
 return 0;
}
int check_and_realloc_txt_coords(size_t needed)
{
    size_t needed_tot = txt_coords->used + needed;
        if(needed_tot > txt_coords->alloced)
        {
            size_t new_size = txt_coords->alloced * 2;
            
            while (new_size < needed_tot)
            {
                new_size *=2;
            }
            txt_coords->coords = st_realloc(txt_coords->coords, new_size * sizeof(POINT_T));
            txt_coords->alloced = new_size;
        }
 return 0;
    
}

int destroy_txt_coords()
{
 if(txt_coords)
 {
        if(txt_coords->coords)
            free(txt_coords->coords);
        
        free(txt_coords);
     
 }
 return 0;
}

