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
 **********************************************************************



 ************************************************************************
*This is the main header file where all exposed fuctions is declared
*************************************************************************/
#ifndef _theclient_H
#define _theclient_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "fonts.h"
#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#endif
#include<pthread.h>
#include "ext/sqlite/sqlite3.h"
//#include <sqlite3.h>

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif
/* Using SDL2 for the base window and OpenGL context init */
#include "SDL.h"
#include "text.h"
#include "global.h"
#include "matrix_handling.h"
#include "symbols.h"
#include "uthash.h"
#include "buffer_handling.h"
#include "twkb.h"

#define INIT_WIDTH 1000
#define INIT_HEIGHT 500



#define DEFAULT_TEXT_BUF 1024

#define MAX_ZOOM_FINGERS 2


#define INIT_PS_POOL_SIZE 10



typedef struct
{
    double x;
    double y;
}
t_vec2;

typedef struct
{
    int active;
    int64_t fingerid;
    GLfloat x1;
    GLfloat y1;
    GLfloat x2;
    GLfloat y2;
}
FINGEREVENT;




/**

In runtime an array of the structure below holds all styles involved in the rendering
*/
typedef struct
{
    GLuint layerID;
    GLuint styleID;
    GLfloat color[4];
    GLfloat outlinecolor[4];
    GLfloat lineWidth;
    GLfloat lineWidth2;
    GLfloat z;
    GLint unit;
}
STYLES_RUNTIME;




typedef struct
{
        GLFLOAT_LIST *color;
        GLUSHORT_LIST *z;
        GLUSHORT_LIST *units;        
        int nsyms;
}
POLYGON_STYLE;

typedef struct
{
        GLFLOAT_LIST *color;
        GLFLOAT_LIST *width;
        GLUSHORT_LIST *z;
        GLUSHORT_LIST *units;   
        int nsyms;
}
LINE_STYLE;

typedef struct
{
        UINT8_LIST *symbol;
        GLFLOAT_LIST *color;
        GLFLOAT_LIST *size;
        GLUSHORT_LIST *z;
        GLUSHORT_LIST *units;  
        int nsyms;
}
POINT_STYLE;

typedef struct
{
        GLFLOAT_LIST *color;
        GLFLOAT_LIST *size;
        GLUSHORT_LIST *z;
        int nsyms;
}
TEXT_STYLE;



struct STYLES
{
        int key_type;
        int int_key;
        char *string_key;
        POLYGON_STYLE *polygon_styles;
        LINE_STYLE *line_styles;
        POINT_STYLE *point_styles;
        TEXT_STYLE *text_styles;
        UT_hash_handle hh;         /* makes this structure hashable */        
};








typedef struct {
    GLfloat coord[4];
    void *next;
} POINT_CIRCLE;



int init_resources(char *dir);
/*************Memory handling***********/


/*void destroy_buffer(GLESSTRUCT *res_buf);
int check_and_increase_max_pa(size_t needed, GLESSTRUCT *res_buf);
float* get_start(uint32_t npoints, uint8_t ndims, GLESSTRUCT *res_buf);
int set_end(uint32_t npoints, uint8_t ndims,uint32_t id, uint32_t styleID, GLESSTRUCT *res_buf);

int check_and_increase_max_polygon(uint32_t needed, GLESSTRUCT *res_buf);
int set_end_polygon( GLESSTRUCT *res_buf);


ELEMENTSTRUCT* init_element_buf();
void element_destroy_buffer(ELEMENTSTRUCT *element_buf);
int element_check_and_increase_max_pa(size_t needed, ELEMENTSTRUCT *element_buf);
GLushort* element_get_start(uint32_t npoints, uint8_t ndims, ELEMENTSTRUCT *element_buf);
int element_set_end(uint32_t npoints, uint8_t ndims,uint32_t styleID, ELEMENTSTRUCT *element_buf);
GLfloat* increase_buffer(GLESSTRUCT *res_buf);
*/
TEXTSTRUCT* init_text_buf();
int text_write(const char *the_text,uint32_t styleID, GLshort size, float rotation,uint32_t anchor, TEXTSTRUCT *text_buf);
void text_reset_buffer(TEXTSTRUCT *text_buf);
void text_destroy_buffer(TEXTSTRUCT *text_buf);


//int id; //just for debugging, remove later

/* Functions for decoding twkb*/
int read_header (TWKB_PARSE_STATE *ts);
int decode_twkb_start(uint8_t *buf, size_t buf_len);
int decode_twkb(TWKB_PARSE_STATE *old_ts);
int* decode_element_array(TWKB_PARSE_STATE *old_ts);

/*a type holding pointers to our parsing functions*/
typedef int (*parseFunctions_p)(TWKB_PARSE_STATE*);

/*Functions for decoding varInt*/
int64_t unzigzag64(uint64_t val);
uint64_t buffer_read_uvarint(TWKB_BUF *tb);
int64_t buffer_read_svarint(TWKB_BUF *tb);
uint8_t buffer_read_byte(TWKB_BUF *tb);
void buffer_jump_varint(TWKB_BUF *tb,int n);



/*resetting GLESSTRUCT buffer*/
void reset_buffer();

/*resetting ELEMENTSTRUCT buffer*/
void element_reset_buffer();

/*Functions exposed to other programs*/
void *twkb_fromSQLiteBBOX_thread( void *theL);
void *twkb_fromSQLiteBBOX( void *theL);
GLuint create_shader(const char* source, GLenum type);
void print_log(GLuint object);
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);

/*utils*/
void copy2ref_box(MATRIX *matrix_hndl,MATRIX *ref);
int multiply_matrices(GLfloat *matrix1,GLfloat *matrix2, GLfloat *theMatrix);
int multiply_matrix_vektor(GLfloat *matrix,GLfloat *vektor_in, GLfloat *vektor_out);

/*This is functions for manipulating bbox, translations and zoom*/
int px2m(GLfloat *bbox,GLint px_x,GLint px_y,GLfloat *w_x,GLfloat *w_y);
int calc_translate(GLfloat w_x,GLfloat w_y, GLfloat *transl);
int calc_scale(GLfloat *bbox, GLfloat zoom, GLfloat *scale);
void windowResize(int newWidth,int newHeight,MATRIX *matrix_hndl,MATRIX *out);

void initialBBOX(GLfloat x, GLfloat y, GLfloat width, MATRIX *map_matrix);
int reset_matrix(MATRIX *matrix_handl);
//void initialBBOX(GLfloat x, GLfloat y, GLfloat width, GLfloat *newBBOX);

/*event handling*/
int matrixFromBboxPointZoom(MATRIX *map_matrix,MATRIX *out,GLint px_x_clicked,GLint px_y_clicked, GLfloat zoom);
//int matrixFromDeltaMouse(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat mouse_down_x,GLfloat mouse_down_y,GLfloat mouse_up_x,GLfloat mouse_up_y, GLfloat *theMatrix);
int matrixFromDeltaMouse(MATRIX *map_matrix,MATRIX *out, GLint mouse_down_x, GLint mouse_down_y, GLint mouse_up_x, GLint mouse_up_y);
LAYER_RUNTIME* init_layer_runtime(int n);
void destroy_layer_runtime(LAYER_RUNTIME *lr, int n);

int  matrixFromBBOX(MATRIX *map_matrix );
//int get_data(SDL_Window* window,GLfloat *bbox,GLfloat *theMatrix);
int get_data(SDL_Window* window,MATRIX *map_matrix);

int loadPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int loadLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int renderLineTri(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);

void render_txt(SDL_Window* window) ;
int loadPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int render_data(SDL_Window* window,GLfloat *theMatrix);
int render_info(SDL_Window* window,GLfloat *theMatrix);


int loadRaster(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int renderRaster(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int loadandRenderRaster(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);

FINGEREVENT* init_touch_que();
int reset_touch_que(FINGEREVENT *touches);
int get_box_from_touches(FINGEREVENT *touches,MATRIX *matrix_hndl,MATRIX *out);
int register_touch_down(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);
int register_touch_up(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);
int register_motion(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);

GLuint create_program(const unsigned char *vs_source,const unsigned char *fs_source, GLuint *vs, GLuint *fs);

void reset_shaders(GLuint vs,GLuint fs,GLuint program);

//uint32_t utf82unicode(char *text, char **the_rest);
int init_text_resources();


void log_this(int log_level, const char *log_txt, ... );
//int draw_it(GLfloat *color,GLfloat *point_coord, int atlas_nr,int bold,GLint txt_box,GLint txt_color,GLint txt_coord2d,char *txt,GLint max_width, float sx, float sy);
//int draw_it(GLfloat *color,GLfloat *startp,GLfloat *offset, int atlas_nr,int bold,GLint txt_box,GLint txt_color,GLint txt_coord2d,char *txt,GLint max_width, float sx, float sy);
int draw_it(GLfloat *color,GLfloat *startp,GLfloat *offset,ATLAS *a/* int atlas_nr,int bold*/,GLint txt_box,GLint txt_color,GLint txt_coord2d,char *txt,GLint max_width, float sx, float sy);
//int print_txt(float x,float y,float r, float g, float b, float a,int size, const char *txt, ... );
//int print_txt(GLfloat *point_coord,GLfloat *color,int size,int bold,int max_width, const char *txt, ... );
//int render_simple_rect(GLfloat minx, GLfloat miny, GLfloat maxx, GLfloat maxy);
int render_simple_rect(GLshort *coords, GLfloat *color, MATRIX *matrix_hndl);

void calc_start(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal);
void calc_join(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal);
void calc_end(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal);

int build_program();
int check_layer(const unsigned char *dbname, const unsigned char  *layername);


void gps_in(double latitude, double longitude, double acc);
int renderGPS(GLfloat *theMatrix);
int loadGPS(GLfloat *gps_circle);
int loadSymbols();
void reproject(GLfloat *points,uint8_t utm_in,uint8_t utm_out, uint8_t hemi_in, uint8_t hemi_out);
int check_column(const unsigned char *dbname,const unsigned char * layername, const unsigned char  *col_name);


int search_string(const char *w,const  char *s);
/*********************** Global variables*******************************/


#ifndef _WIN32
struct timeval tval_before, tval_after, tval_result;
#endif

int map_modus;
GLfloat info_box_color[4];
sqlite3 *projectDB;

GLfloat init_x;
GLfloat init_y;
GLint curr_utm;
GLint curr_hemi;
GLfloat init_box_width;
int nLayers;
int text_scale;
GLfloat *gps_circle;

LAYER_RUNTIME *layerRuntime;
LAYER_RUNTIME *infoLayer;
LAYER_RUNTIME *infoRenderLayer;


STYLES_RUNTIME *global_styles;
size_t length_global_styles;
size_t length_global_symbols;
void render_text_test(const char *text, float x, float y, float sx, float sy);

int  render_text(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int CURR_WIDTH;
int CURR_HEIGHT;



const char *fontfilename;
/*shader programs*/

//Standard geometryprogram
GLuint std_program;
GLint std_coord2d;
GLint std_matrix;
GLint std_color;

//Standard textprogram
GLuint txt_program;
GLint txt_coord2d;
GLint txt_matrix;
GLint txt_box;
GLint txt_tex;
GLint txt_texpos;
GLint txt_color;

GLuint gen_vbo;


//Standard geometryprogram
GLuint lw_program;
GLint lw_coord2d;
GLint lw_matrix;
GLint lw_px_matrix;
GLint lw_linewidth;
GLint lw_norm;
GLint lw_color;
GLint lw_z;

//gps-void calc_end(POINT_CIRCLE* p, GLfloat* ut, int* c, vec2* last_normal)

GLuint gps_program;
GLint gps_norm;
GLint gps_coord2d;
GLint gps_radius;
GLint gps_color;
GLint gps_matrix;
GLint gps_px_matrix;

GLuint sym_program;
GLint sym_norm;
GLint sym_coord2d;
GLint sym_radius;
GLint sym_color;
GLint sym_matrix;
GLint sym_px_matrix;

GLuint raster_program;
GLint raster_coord2d;
GLint raster_texcoord;
GLint raster_matrix;
GLint raster_texture;


typedef struct  {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
} point;

point gps_point;
Uint32 GPSEventType;
GLuint text_vbo;
int gps_npoints;
GLuint gps_vbo;

int total_points;
int n_points;
int n_lines;
int n_polys;
int n_tri;
int n_words;
int n_letters;


GLuint  texes[256];
GLenum err;
#endif
