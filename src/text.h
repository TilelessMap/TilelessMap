
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

#ifndef _text_H
#define _text_H



#include "matrix_handling.h"
#include "buffer_handling.h"
#include "theclient.h"


//Alignment left bottom is default
#define H_LEFT_ALIGNMENT 0
#define V_BOTTOM_ALIGNMENT 0
#define H_CENTER_ALIGNMENT 1
#define V_CENTER_ALIGNMENT 2
#define H_RIGHT_ALIGNMENT 4
#define V_TOP_ALIGNMENT 8


#define APPENDING_STRING 0
#define NEW_STRING 1

typedef struct
{
    char *txt;
    size_t alloced;
    size_t used;
}
TEXT;

typedef struct
{
    uint32_t *txt;
    size_t alloced;
    size_t used;
}
WCHAR_TEXT;


typedef struct
{
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
} POINT_T;

typedef struct
{
    POINT_T *coords;
    size_t used;
    size_t alloced;
} TEXTCOORDS;

/*This is used for styling a text in a text_block
 * It holds pointer to beginning and end of parts of text in textblock.
 * This way we kan have different style quite easy on a single row*/

typedef struct
{
    GLUINT_LIST *formating_index;
    //GLUINT_LIST *text_index; Not needed since relation is 1:1 in index
    GLUINT_LIST *linestart_index;
    POINT_LIST *points;
    GLUINT_LIST *alignment;
    size_t ntexts;
}TXT_INFO;

typedef struct
{
    GLUINT_LIST *txt_index;
    GLFLOAT_LIST *color;
    POINTER_LIST *font;
    size_t nform;
}TXT_FORMATING;

typedef struct
{    
    GLUINT_LIST *txt_index;    
    GLUINT_LIST *coord_index;
    TEXTCOORDS *coords;
    GLUINT_LIST *linestart;
    GLFLOAT_LIST *line_widths; 
    GLFLOAT_LIST *max_widths;
    GLFLOAT_LIST *widths;
    GLFLOAT_LIST *heights;    
}TXT_DIMS;
typedef struct
{
    TEXT *txt;
    TXT_FORMATING *formating;
    TXT_DIMS *dims;
    TXT_INFO *txt_info;
    float cursor_x;
    float cursor_y;
    float rowheight;
} TEXTBLOCK;



TEXT* init_txt(size_t s);
int add_txt(TEXT *t,const char *in);
char* get_txt(TEXT *t);
int reset_txt(TEXT *t);
int destroy_txt(TEXT *t);


WCHAR_TEXT* init_wc_txt(size_t s);
int add_wc_txt(TEXT *t);
int add_utf8_2_wc_txt(WCHAR_TEXT *t,const char *in);
int add_n_utf8_2_wc_txt(WCHAR_TEXT *t,const char *in, size_t len);
int reset_wc_txt(WCHAR_TEXT *t);
int destroy_wc_txt(WCHAR_TEXT *t);

uint32_t utf82unicode(const char *text,const char **the_rest);


TEXTBLOCK* init_textblock();
int destroy_textblock(TEXTBLOCK *tb);
int append_2_textblock(TEXTBLOCK *tb, const char* txt, ATLAS *font, float *font_color, int max_width,int alignment, int newstring);



int print_txt(GLfloat *point_coord,GLfloat *point_offset, MATRIX *matrix_hndl,GLfloat *color,int size,int bold,int max_width, const char *txt, ... );
//int print_txtblock(GLfloat *point_coord, MATRIX *matrix_hndl, GLfloat *color,int max_width, TEXTBLOCK *tb);
int print_txtblock(GLfloat *point_coord, MATRIX *matrix_hndl,  TEXTBLOCK *tb);

TEXTCOORDS* init_txt_coords(size_t size);
int check_and_realloc_txt_coords(TEXTCOORDS *tc, size_t needed);
int destroy_txt_coords(TEXTCOORDS *tc);
WCHAR_TEXT  *tmp_unicode_txt;
int calc_dims(TEXTBLOCK *tb,int max_width);
/*TODO This is just temporary
 * Later there will be something holding all txt_coordinates from all layers and controls
 * and all of it will be rendered from there. */
TEXTCOORDS *txt_coords;

#endif
