/**********************************************************************
 *
 * TilelessMap
 *
 * TilelessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TilelessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TilelessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/

#ifndef _text_H
#define _text_H



#include "../handle_input/matrix_handling.h"
#include "../buffer_handling.h"
#include "../structures.h"
#include "fonts.h"

//Alignment left bottom is default
#define H_LEFT_ALIGNMENT 0
#define V_BOTTOM_ALIGNMENT 0
#define H_CENTER_ALIGNMENT 1
#define V_CENTER_ALIGNMENT 2
#define H_RIGHT_ALIGNMENT 4
#define V_TOP_ALIGNMENT 8


#define APPENDING_STRING 0
#define NEW_STRING 1



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
int append_2_textblock(TEXTBLOCK *tb, const char* txt, ATLAS *font, float *font_color, int max_width, int newstring, WCHAR_TEXT *unicode_txt);



int print_txt(GLfloat *point_coord,GLfloat *point_offset, MATRIX *matrix_hndl,GLfloat *color,int size,int bold,int max_width, const char *txt, ... );
//int print_txtblock(GLfloat *point_coord, MATRIX *matrix_hndl, GLfloat *color,int max_width, TEXTBLOCK *tb);
int print_txtblock(GLfloat *point_coord, MATRIX *matrix_hndl,  TEXTBLOCK *tb,float *anchor, float *displacement);

TEXTCOORDS* init_txt_coords(size_t size);
int check_and_realloc_txt_coords(TEXTCOORDS *tc, size_t needed);
int reset_txt_coords(TEXTCOORDS *tc);
int destroy_txt_coords(TEXTCOORDS *tc);

/*This is ugly. tmp_unicode_txt is used in a lot of places just to reduce calls to malloc
 * But this gives that it can only be used in serial and not in threads.
 * It is used only when creating controls, which is done in the same thread.
 * If used in layers which is handles in differnt threads it will be problems*/
WCHAR_TEXT  *tmp_unicode_txt;


int calc_dims(TEXTBLOCK *tb,int max_width, WCHAR_TEXT *unicode_txt);
int get_txt_height(TEXTBLOCK *tb);

/*TODO This is just temporary
 * Later there will be something holding all txt_coordinates from all layers and controls
 * and all of it will be rendered from there. */
TEXTCOORDS *txt_coords;

int reset_textblock(TEXTBLOCK *tb);
#endif
