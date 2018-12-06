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



#ifndef _buffer_handling_H
#define _buffer_handling_H

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

#include <stdint.h>
#include "ext/sqlite/sqlite3.h"
#include "structures.h"
#define INIT_LIST_SIZE 4


#define INT_TYPE 1
#define FLOAT_TYPE 2
#define STRING_TYPE 3

#define PIXEL_UNIT 0
#define METER_UNIT 1

SYMBOLS *global_symbols;








UINT8_LIST* init_uint8_list();
GLFLOAT_LIST* init_glfloat_list();
GLUSHORT_LIST* init_glushort_list();
SYMBOLS* init_symbol_list();
UNION_LIST* init_union_list();
POINTER_LIST* init_pointer_list();
GLUINT_LIST* init_gluint_list();
POINT_LIST* init_tb_point_list();


int add2glfloat_list(GLFLOAT_LIST *list, GLfloat val);
int add2gluint_list(GLUINT_LIST *list, GLuint val);
int add2int64_list(INT64_LIST *list, int64_t val);
int add2glushort_list(GLUSHORT_LIST *list, GLshort val);
int add2uint8_list(UINT8_LIST *list, uint8_t val);

int addbatch2glfloat_list(GLFLOAT_LIST *list,GLuint n_vals, GLfloat *vals);
int addbatch2int64_list(INT64_LIST *list,GLuint n_vals, int64_t *vals);
int addbatch2gluint_list(GLUINT_LIST *list,GLuint n_vals, GLuint *vals);
int addbatch2glushort_list(GLUSHORT_LIST *list,GLuint n_vals, GLushort *vals);


int add2union_list(UNION_LIST *list, void *val);
int addbatch2uint8_list(UINT8_LIST *list,GLuint n_vals, uint8_t *vals);

int add2pointer_list(POINTER_LIST *list, void *val);
int setzero2pointer_list(POINTER_LIST *list,GLuint n_vals);
int setzero2int64_list(INT64_LIST *list,int64_t n_vals);

int reset_gluint_list(GLUINT_LIST *l);
int reset_glfloat_list(GLFLOAT_LIST *l);
int reset_pointer_list(POINTER_LIST *l);
int reset_point_list(POINT_LIST *l);
int reset_uint8_list(UINT8_LIST *l);

int destroy_glfloat_list(GLFLOAT_LIST *l);
int destroy_gluint_list(GLUINT_LIST *l);
int destroy_int64_list(INT64_LIST *l);
int destroy_glushort_list(GLUSHORT_LIST *l);
int destroy_pointer_list(POINTER_LIST *l);
int destroy_uint8_list(UINT8_LIST *l);

int destroy_symbol_list(SYMBOLS *l);
int destroy_buffers(LAYER_RUNTIME *layer);

int destroy_point_list(POINT_LIST *l);


int reset_buffers(LAYER_RUNTIME *layer);

int init_buffers(LAYER_RUNTIME *layer);


TEXTSTRUCT* init_text_buf();
void text_reset_buffer(TEXTSTRUCT *text_buf);
void text_destroy_buffer(TEXTSTRUCT *text_buf);
int pa_end(LAYER_RUNTIME *l, int64_t id);



int init_symbols();
int addsym(uint8_t id, size_t n_points, GLfloat *points);
//int destroy_symbols();




struct STYLES* get_style(struct STYLES *styles, void *val,int val_type);







struct STYLES *system_default_style;
struct STYLES *system_default_info_style;




























#endif
