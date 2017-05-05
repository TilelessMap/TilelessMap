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

#include <time.h>
#include <sys/time.h>
#include<pthread.h>
#include "ext/sqlite/sqlite3.h"
//#include <sqlite3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <android/log.h>
#else
#include <GL/glew.h>
#endif
/* Using SDL2 for the base window and OpenGL context init */
#include "SDL.h"


#define APPNAME "TILELESS"
#define LOGLEVEL 100



#define INIT_WIDTH 1000
#define INIT_HEIGHT 500

/*Maximum number of dimmensions that a twkb geoemtry
can hold according to the specification*/
#define TWKB_IN_MAXCOORDS 4

/*twkb types*/
#define	POINTTYPE			1
#define	LINETYPE			2
#define	POLYGONTYPE		3
#define	MULTIPOINTTYPE	4
#define	MULTILINETYPE		5
#define	MULTIPOLYGONTYPE	6
#define	COLLECTIONTYPE		7
#define DEFAULT_TEXT_BUF 1024

#define MAX_ZOOM_FINGERS 2
#define DEBUG 1

#if DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define INIT_PS_POOL_SIZE 10

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

This is a struct keeping track of the arrays holdng vertices and vertice information
*/
typedef struct
{
    float *vertex_array; // A long list of vertices
    uint32_t *start_index; //a list of offsets where each point array starts in "vertex_array" above
    uint32_t *npoints; //a list of npoints in corresponding point array
    uint32_t total_npoints; // total npoints in all point arrays
    uint32_t used_n_pa; //used number of point arrays
    uint32_t max_pa; //max number of point arrays that we have allocated memory for in "npoints" and "start_index" lists
    float *buffer_end; //where "vertex_array" ends
    float *first_free;    //first free position in "vertex_array"
    uint32_t *polygon_offset; //a polygon can have several point arrays (if holes) so we have to keep track of where the polygon starts
    uint32_t used_n_polygon; //number of polygons that we have registered
    uint32_t max_polygon; //max number of polygons that we have allocated memory for in "used_n_polygon" and "polygon_offset" lists
    int *id; //lista med id till korresponderande pointarrays
    uint32_t *styleID; //array of styleID
}
GLESSTRUCT;

/**

This is a structsimilar to the above, but instead keeping track of arrays of indexes constructing triangels from vertex_array
*/
typedef struct
{
    GLushort *index_array; // a list of indeces for triangels
    uint32_t *start_index; //list where each point array starts (not each triangel)
    uint32_t *npoints; //number of values in "index_array"
    uint32_t total_npoints;
    uint32_t max_pa; //max number of point arrays we have allocated memory for storing info about
    uint32_t used_n_pa;
    GLushort *buffer_end;
    GLushort *first_free;
    int *id; //list if id to corresponding point array
    uint32_t *styleID; //array of styleID
}
ELEMENTSTRUCT;



/**

This is a structsimilar to the above, but instead keeping track text for labeling
*/
typedef struct
{
    char *char_array; // a list characters. Each string is zeroterminated
    size_t used_n_chars;
    size_t max_n_chars;
    uint32_t used_n_vals;
    uint32_t max_n_vals;
    float *rotation; //list if id to corresponding point array
    int *size; //list if id to corresponding point array
    uint32_t *anchor;
    uint32_t *styleID; //array of styleID
}
TEXTSTRUCT;


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
}
STYLES_RUNTIME;

/**

Information about all the layers in the project is loaded in an array of this structure at start.
*/
typedef struct
{
    char* name[63];
    uint8_t visible;
    sqlite3_stmt *preparedStatement;
    GLuint program;
    /*Placeholders in shaders*/
    GLint attribute_coord2d;
    GLint uniform_theMatrix;
    GLint uniform_color;
    /*Buffers*/
    GLuint vbo;
    GLuint ebo;
    /*Values for shaders*/
    GLfloat theMatrix[16];
    /*values for what and how to render*/
    GLfloat *BBOX;
    uint8_t geometryType;
//   uint8_t has_text;
    GLint minScale;
    GLint maxScale;
    GLESSTRUCT *res_buf;
    ELEMENTSTRUCT *tri_index;
    TEXTSTRUCT *text;
    //Placeholders in text_shader
    /*   GLuint txt_program;
       GLint txt_coord2d;
       GLint txt_theMatrix;
       GLint txt_color;
       GLint txt_box;
       GLint txt_tex;
    */
    uint8_t show_text;
    int render_area; //This is a way to render only border of polygon. No triangels will be loadded


}
LAYER_RUNTIME;




typedef struct {
    float ax;	// advance.x
    float ay;	// advance.y

    float bw;	// bitmap.width;
    float bh;	// bitmap.height;

    float bl;	// bitmap_left;
    float bt;	// bitmap_top;

    float tx;	// x offset of glyph in texture coordinates
    float ty;	// y offset of glyph in texture coordinates
} C;		// character information

typedef struct   {
    GLuint tex;		// texture object

    unsigned int w;			// width of texture in pixels
    unsigned int h;			// height of texture in pixels
    C metrics[256];
} ATLAS;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
} POINT_T;
/***************************************************************
			DECODING TWKB						*/
/*Holds a buffer with the twkb-data during read*/
typedef struct
{
    uint8_t handled_buffer; /*Indicates if this program is resposible for freeing*/
    uint64_t BufOffsetFromBof;	//Only osed when reading from file
    uint8_t *start_pos;
    uint8_t *read_pos;
    uint8_t *end_pos;
    uint8_t *max_end_pos;

} TWKB_BUF;

typedef struct
{
    float bbox_min[TWKB_IN_MAXCOORDS];
    float bbox_max[TWKB_IN_MAXCOORDS];
} BBOX;


typedef struct
{
    uint8_t has_bbox;
    uint8_t has_size;
    uint8_t has_idlist;
    uint8_t has_z;
    uint8_t has_m;
    uint8_t is_empty;
    uint8_t type;

    /* Precision factors to convert ints to double */
    uint8_t n_decimals[TWKB_IN_MAXCOORDS];
    /* Precision factors to convert ints to double */
    double factors[TWKB_IN_MAXCOORDS];

    uint32_t ndims; /* Number of dimensions */
    /* An array to keep delta values from 4 dimensions */
    int64_t coords[TWKB_IN_MAXCOORDS];

    BBOX *bbox;
    size_t next_offset;
    int32_t id;
} TWKB_HEADER_INFO;

/* Used for passing the parse state between the parsing functions.*/
typedef struct
{
    TWKB_BUF *tb; /* Points to start of TWKB */
    //~ buffer_collection *rb;
    TWKB_HEADER_INFO *thi;
    GLESSTRUCT *res_buf;
    sqlite3_stmt *prepared_statement;
    uint32_t id;  //the current styleID
    uint32_t styleID;  //the current styleID
} TWKB_PARSE_STATE;



/*************Memory handling***********/


GLESSTRUCT* init_res_buf();
void destroy_buffer(GLESSTRUCT *res_buf);
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


TEXTSTRUCT* init_text_buf();
int text_write(const char *the_text,uint32_t styleID, float size, float rotation,uint32_t anchor, TEXTSTRUCT *text_buf);
void text_reset_buffer(TEXTSTRUCT *text_buf);
void text_destroy_buffer(TEXTSTRUCT *text_buf);


//int id; //just for debugging, remove later

/* Functions for decoding twkb*/
int read_header (TWKB_PARSE_STATE *ts);
int decode_twkb_start(uint8_t *buf, size_t buf_len);
int decode_twkb(TWKB_PARSE_STATE *old_ts,GLESSTRUCT *res_buf);
int* decode_element_array(TWKB_PARSE_STATE *old_ts, ELEMENTSTRUCT *index_buf);

/*a type holding pointers to our parsing functions*/
typedef int (*parseFunctions_p)(TWKB_PARSE_STATE*,GLESSTRUCT *res_buf);

/*Functions for decoding varInt*/
int64_t unzigzag64(uint64_t val);
uint64_t buffer_read_uvarint(TWKB_BUF *tb);
int64_t buffer_read_svarint(TWKB_BUF *tb);
uint8_t buffer_read_byte(TWKB_BUF *tb);
void buffer_jump_varint(TWKB_BUF *tb,int n);


long int getReadPos(TWKB_BUF *tb);

/*resetting GLESSTRUCT buffer*/
void reset_buffer();

/*resetting ELEMENTSTRUCT buffer*/
void element_reset_buffer();

/*Functions exposed to other programs*/
void *twkb_fromSQLiteBBOX( void *theL);
GLuint create_shader(const char* source, GLenum type);
void print_log(GLuint object);
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);

/*utils*/
void copyNew2CurrentBBOX(GLfloat *newBBOX,GLfloat *currentBBOX);
int multiply_matrices(GLfloat *matrix1,GLfloat *matrix2, GLfloat *theMatrix);

/*This is functions for manipulating bbox, translations and zoom*/
int px2m(GLfloat *bbox,GLfloat px_x,GLfloat px_y,GLfloat *w_x,GLfloat *w_y);
int calc_translate(GLfloat w_x,GLfloat w_y, GLfloat *transl);
int calc_scale(GLfloat *bbox, GLfloat zoom, GLfloat *scale);
void windowResize(int newWidth,int newHeight,GLfloat *currentBBOX, GLfloat *newBBOX);
void initialBBOX(GLfloat x, GLfloat y, GLfloat width, GLfloat *newBBOX);

/*event handling*/
int matrixFromBboxPointZoom(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat px_x_clicked,GLfloat px_y_clicked, GLfloat zoom, GLfloat *theMatrix);
int matrixFromDeltaMouse(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat mouse_down_x,GLfloat mouse_down_y,GLfloat mouse_up_x,GLfloat mouse_up_y, GLfloat *theMatrix);

LAYER_RUNTIME* init_layer_runtime(int n);
int  matrixFromBBOX(GLfloat *newBBOX, GLfloat *theMatrix );
int get_data(SDL_Window* window,GLfloat *bbox,GLfloat *theMatrix);

int loadPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int loadLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix, int outline);

void render_txt(SDL_Window* window) ;
int loadPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int  renderPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int render_data(SDL_Window* window,GLfloat *theMatrix);


FINGEREVENT* init_touch_que();
int reset_touch_que(FINGEREVENT *touches);
int get_box_from_touches(FINGEREVENT *touches,GLfloat *currentBBOX,GLfloat *newBBOX);
int register_touch_down(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);
int register_touch_up(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);
int register_motion(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);

GLuint create_program(const unsigned char *vs_source,const unsigned char *fs_source, GLuint *vs, GLuint *fs);

void reset_shaders(GLuint vs,GLuint fs,GLuint program);

//uint32_t utf82unicode(char *text, char **the_rest);
uint32_t utf82unicode(const char *text,const char **the_rest);
int init_text_resources(char *dir);
void render_txt(SDL_Window* window);

ATLAS* create_atlas(ATLAS *a, FT_Face face, int height);

void log_this(int log_level, const char *log_txt, ... );
int draw_it(GLfloat *color,GLfloat *point_coord, int atlas_nr,GLint txt_box,GLint txt_color,GLint txt_coord2d,char *txt, float sx, float sy  );
int print_txt(float x,float y,float r, float g, float b, float a,int size, const char *txt, ... );
int render_simple_Polygon();
/*********************** Global variables*******************************/

struct timeval tval_before, tval_after, tval_result;

sqlite3 *projectDB;

int nLayers;
int text_scale;

LAYER_RUNTIME *layerRuntime;


STYLES_RUNTIME *global_styles;
size_t length_global_styles;
void render_text_test(const char *text, float x, float y, float sx, float sy);

int  render_text(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int CURR_WIDTH;
int CURR_HEIGHT;

FT_Library ft;
ATLAS *atlases[3];

const char *fontfilename;



GLuint text_program;
GLint text_attribute_coord;
GLint text_uniform_tex;
//GLint text_uniform_color;

GLint gen_program;
GLint gen_coord2d;
GLint gen_theMatrix;
GLint gen_color;
GLuint gen_vbo;

GLint gen_txt_program;
GLint gen_txt_coord2d;
GLint gen_txt_color;
GLint gen_txt_theMatrix;
GLint gen_txt_tex;
GLint gen_txt_box;

typedef struct  {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
} point;

GLuint text_vbo;


int total_points;
int n_points;
int n_lines;
int n_polys;
int n_tri;
int n_words;
int n_letters;

GLenum err;
#endif
