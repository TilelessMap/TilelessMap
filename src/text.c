

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
#include "utils.h"

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

    t->txt = st_realloc(t->txt, new_s);
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
    t->txt = NULL;
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

//TODO, fix real unicode. Now truncated to first byte
//Could be fixed by putting "next_byte" info in a separate array. 
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

 
int add_n_utf8_2_wc_txt(WCHAR_TEXT *t,const char *in, size_t len)
{

    uint32_t p = 0;
    unsigned int i;
    const char *u = in;
    for (i=0;i<len;i++)
    {
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

static TXT_INFO* init_txt_info()
{
    TXT_INFO *ti = st_malloc(sizeof(TXT_INFO));
    ti->formating_index = init_gluint_list();
    add2gluint_list(ti->formating_index, 0);
    ti->linestart_index = init_gluint_list();
    add2gluint_list(ti->linestart_index,0);
    ti->alignment = init_gluint_list();
    ti->points = NULL;
    ti->ntexts = 0;
    
    return ti;
}

static int reset_txt_info(TXT_INFO *ti)
{
 reset_gluint_list(ti->formating_index);
    add2gluint_list(ti->formating_index, 0);
    reset_gluint_list(ti->linestart_index);
    add2gluint_list(ti->linestart_index,0);
 reset_gluint_list(ti->alignment);
 /*if(ti->points)
     reset_point_list(ti->points);
 */
    ti->ntexts = 0;
    
 return 0;
}


static int destroy_txt_info(TXT_INFO *ti)
{
 destroy_gluint_list(ti->formating_index);
 //destroy_gluint_list(ti->text_index);
    destroy_gluint_list(ti->linestart_index);
 destroy_gluint_list(ti->alignment);
/* if(ti->points)
     destroy_point_list(ti->points);
  */  
 st_free(ti);
    ti = NULL;
 return 0;
}

static TXT_DIMS* init_txt_dims()
{
    TXT_DIMS *td = st_malloc(sizeof(TXT_DIMS));
    td->txt_index = init_gluint_list();
    add2gluint_list(td->txt_index, 0);
    
    td->coords = init_txt_coords(64);
    td->coord_index = init_gluint_list();
    add2gluint_list(td->coord_index, 0);
    td->linestart = init_gluint_list();  
    add2gluint_list(td->linestart,0);
    td->line_widths = init_glfloat_list();
    td->max_widths = init_glfloat_list();
    td->widths = init_glfloat_list();
    td->heights = init_glfloat_list();
    
    return td;
}

static int reset_txt_dims(TXT_DIMS *td)
{
 reset_gluint_list(td->txt_index);
    add2gluint_list(td->txt_index, 0);
 reset_gluint_list(td->coord_index);
    add2gluint_list(td->coord_index, 0);
 reset_txt_coords(td->coords);
reset_gluint_list(td->linestart);  
    add2gluint_list(td->linestart,0);
reset_glfloat_list(td->line_widths);
 reset_glfloat_list(td->max_widths);
 reset_glfloat_list(td->heights);
 reset_glfloat_list(td->widths);
 return 0;
}
static int destroy_txt_dims(TXT_DIMS *td)
{
 destroy_gluint_list(td->txt_index);
 destroy_gluint_list(td->coord_index);
 destroy_txt_coords(td->coords);
destroy_gluint_list(td->linestart);
destroy_glfloat_list(td->line_widths);
 destroy_glfloat_list(td->max_widths);
 destroy_glfloat_list(td->heights);
 destroy_glfloat_list(td->widths);
 free(td);
 td = NULL;
 return 0;
}

static TXT_FORMATING* init_txt_formating()
{
    TXT_FORMATING *tf = st_malloc(sizeof(TXT_FORMATING));
    tf->txt_index = init_gluint_list();
    add2gluint_list(tf->txt_index, 0);
    tf->color = init_glfloat_list();
    tf->font = init_pointer_list();    
    tf->nform = 0;
    
    return tf;
}
static int reset_txt_formating(TXT_FORMATING *tf)
{
    reset_gluint_list(tf->txt_index);
    add2gluint_list(tf->txt_index, 0);
    reset_glfloat_list(tf->color);
    reset_pointer_list(tf->font);    
    tf->nform = 0;
    
    return 0;
}

static int destroy_txt_formating(TXT_FORMATING *tf)
{
 
    destroy_gluint_list(tf->txt_index);
    destroy_glfloat_list(tf->color);
    destroy_pointer_list(tf->font);    
    free(tf);
    tf=NULL;
 return 0;
}


TEXTBLOCK* init_textblock()
{
    TEXTBLOCK *tb = st_malloc(sizeof(TEXTBLOCK));
 //   tb->txt = st_malloc(sizeof(TEXT*));

    tb->txt = init_txt(32);

    tb->formating = init_txt_formating();
    tb->dims = init_txt_dims();
    tb->txt_info = init_txt_info();
    return tb;
}
int reset_textblock(TEXTBLOCK *tb)
{
    reset_txt(tb->txt);
    reset_txt_formating(tb->formating);
    reset_txt_dims(tb->dims);
    reset_txt_info(tb->txt_info);
    return 0;
}

/*
static int realloc_textblock(TEXTBLOCK *tb)
{
    int i;
    int new_size = tb->max_n_txts * 2;
    tb->txt = st_realloc(tb->txt, new_size * sizeof(TEXT*));
    for (i=tb->max_n_txts; i<new_size; i++)
    {
        tb->txt[i] = init_txt(32);
    }
    tb->font = st_realloc(tb->font, new_size * sizeof(ATLAS*));

    tb->font = st_realloc(tb->font, new_size * sizeof(ATLAS*));
    tb->max_n_txts = new_size;
    return 0;
}
*/
int destroy_textblock(TEXTBLOCK *tb)
{
    destroy_txt(tb->txt); 
    destroy_txt_formating(tb->formating);
    destroy_txt_dims(tb->dims);
    destroy_txt_info(tb->txt_info);
    free(tb);
    tb = NULL;
    return 0;
}

/*Append text to a text block
 * THis can be used in multiple way:
 * 1) Just put a single text, with a single styling that will be drawn with the same anchor point in the block, 
 *      USE NEW_STRING as last parameter
 * 2) Put multiple text strings with different styling in the block. All with common anchor point. 
 *      Call multiple times, first with NEW_STRING as last parameter
 *      the preceeding with APPENDING_STRING  
 * 3) Add a totally new string what will use it's own anchor point, Use NEW_STRING as last parameter
 * */
int append_2_textblock(TEXTBLOCK *tb, const char* txt, ATLAS *font, float *font_color, int max_width,int newstring, WCHAR_TEXT *unicode_txt)
{    
    

    log_this(10, "entering %s with text: %s\n",__func__, txt);
    size_t len = strlen(txt);
    TEXT *text = tb->txt;
    char *txt_startpoint;
    unsigned int nlinestarts = 0;
    
    if((len) > (text->alloced - text->used))
        realloc_txt(text, text->used + len+1); //We overwrite the last nullterminator 

        
        
    if(text->used)
        txt_startpoint = text->txt+text->used; 
    else
        txt_startpoint = text->txt;
    
    strncpy(txt_startpoint, txt, len + 1);
    text->used += len;

    TXT_FORMATING *formating = tb->formating;
    
    add2gluint_list(formating->txt_index, text->used);
    
    if(font_color)
    {
        addbatch2glfloat_list(formating->color,4, font_color);
    
    GLfloat *color = formating->color->list+formating->color->used-4;
    multiply_float_array(color, 1.0/255, 4);
    }
    
    //We multiply after inserting to list, to not affect font_color from calling function
    
    
    add2pointer_list(formating->font, font);    

    if(newstring)
    {
        add2gluint_list(tb->dims->txt_index, text->used);
       // add2gluint_list(tb->dims->coord_index,tb->dims->coords->used);
        add2glfloat_list(tb->dims->widths, 0);
        add2glfloat_list(tb->dims->heights, 0);
        add2glfloat_list(tb->dims->max_widths, 0);
        
        add2gluint_list(tb->txt_info->formating_index, tb->formating->nform);
        
        tb->cursor_x=0;
        tb->cursor_y=0; 
        tb->rowheight=0;
        nlinestarts = tb->dims->linestart->used;
        tb->txt_info->ntexts++;
    }   
    calc_dims(tb,max_width,unicode_txt);
  
    tb->formating->nform++;
    
    
       // tb->dims->coord_index->list[tb->dims->coord_index->used-1] = tb->dims->coords->used;
       add2gluint_list(tb->dims->coord_index, tb->dims->coords->used);
    //   printf("coords = %p, %p, %p\n",*tb->dims->coord_index->list, tb->dims->coord_index->list[tb->dims->coord_index->used-1],tb->dims->coord_index->list[tb->dims->coord_index->used-2]);
       
     //  printf("coords used = %d\n",((float**) tb->dims->coord_index->list)[tb->dims->coord_index->used-1]-((float**) tb->dims->coord_index->list)[tb->dims->coord_index->used-2]);
       
       
        tb->txt_info->formating_index->list[tb->txt_info->formating_index->used-1] = tb->formating->nform;
    
        //add2gluint_list(tb->txt_info->formating_index, tb->formating->nform);
        
        if(tb->dims->linestart->used > nlinestarts)
            tb->txt_info->linestart_index->list[tb->txt_info->linestart_index->used-1] = tb->dims->linestart->used; //add first linebreak in text to line break coord_index
            
    

        return tb->txt_info->ntexts;

}





TEXTCOORDS* init_txt_coords(size_t size)
{
    /*TODO This is just temporary
    * Later there will be something holding all txt_coordinates from all layers and controls
    * and all of it will be rendered from there. */
    
    TEXTCOORDS *tc = st_malloc(sizeof(TEXTCOORDS));
    tc->coords = st_malloc(size * sizeof(POINT_T));
    tc->alloced = size;
    tc->used = 0;
    return tc;
}
int check_and_realloc_txt_coords(TEXTCOORDS *tc, size_t needed)
{
    size_t needed_tot = tc->used + needed;
    if(needed_tot > tc->alloced)
    {
        size_t new_size = tc->alloced * 2;

        while (new_size < needed_tot)
        {
            new_size *=2;
        }
        tc->coords = st_realloc(tc->coords, new_size * sizeof(POINT_T));
        tc->alloced = new_size;
    }
    return 0;

}

int reset_txt_coords(TEXTCOORDS *tc)
{
    if(tc && tc->used)
        tc->used = 0;
    return 0;
}

int destroy_txt_coords(TEXTCOORDS *tc)
{
    if(tc)
    {
        if(tc->coords)
            free(tc->coords);

        free(tc);

    }
    return 0;
}











