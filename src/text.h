
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

#include "theclient.h"
#include "matrix_handling.h"
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
    TEXT **txt;
    ATLAS **font;
    int n_txts;
    int max_n_txts;
}TEXTBLOCK;
    
    
    
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


TEXT* init_txt(size_t s);
int add_txt(TEXT *t,const char *in);
char* get_txt(TEXT *t);
int reset_txt(TEXT *t);
int destroy_txt(TEXT *t);

WCHAR_TEXT* init_wc_txt(size_t s);
int add_wc_txt(TEXT *t);
int add_utf8_2_wc_txt(WCHAR_TEXT *t,const char *in);
int reset_wc_txt(WCHAR_TEXT *t);
int destroy_wc_txt(WCHAR_TEXT *t);

uint32_t utf82unicode(const char *text,const char **the_rest);


TEXTBLOCK* init_textblock(size_t s);
int destroy_textblock(TEXTBLOCK *tb);
int append_2_textblock(TEXTBLOCK *tb, const char* txt, ATLAS *font);




int print_txt(GLfloat *point_coord,GLfloat *point_offset, MATRIX *matrix_hndl,GLfloat *color,int size,int bold,int max_width, const char *txt, ... );
int print_txtblock(GLfloat *point_coord, MATRIX *matrix_hndl, GLfloat *color,int max_width, TEXTBLOCK *tb);

int init_txt_coords();
int check_and_realloc_txt_coords(size_t needed);
int destroy_txt_coords();
WCHAR_TEXT  *tmp_unicode_txt;

/*TODO This is just temporary
 * Later there will be something holding all txt_coordinates from all layers and controls
 * and all of it will be rendered from there. */
TEXTCOORDS *txt_coords;

#endif
