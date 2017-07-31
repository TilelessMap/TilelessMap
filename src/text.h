
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


/*
typedef struct
{
    TEXT *txt;
    int *sizes;
  */  
    
    
typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
} POINT_T;




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


WCHAR_TEXT  *tmp_unicode_txt;
#endif
