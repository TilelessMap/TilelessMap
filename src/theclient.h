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
#ifndef _WIN32
#include <sys/time.h>
#endif
#include<pthread.h>
#include "ext/sqlite/sqlite3.h"
//#include <sqlite3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

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


#define INIT_PS_POOL_SIZE 10

#define PIXELUNIT 0
#define MAPUNIT 1

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



typedef struct
{
    GLfloat *list;
    size_t alloced;
    size_t used;
}
GLFLOAT_LIST;


typedef struct
{
    GLushort *list;
    size_t alloced;
    size_t used;
}
GLUSHORT_LIST;

typedef struct
{
    GLuint *list;
    size_t alloced;
    size_t used;
}
GLUINT_LIST;

typedef struct
{
    int64_t *list;
    size_t alloced;
    size_t used;
}
INT64_LIST;

typedef struct
{
    GLFLOAT_LIST *points;
    GLUINT_LIST *point_start_indexes;
    GLUINT_LIST *style_id;
    GLuint vbo;

}
POINT_LIST;


typedef struct
{
    GLFLOAT_LIST *vertex_array;
    GLUINT_LIST *line_start_indexes;
    GLUINT_LIST *style_id;
    GLuint vbo;

}
LINESTRING_LIST;

typedef struct
{
    GLFLOAT_LIST *vertex_array;  //all vertex coordinates in a long array
    GLUINT_LIST *pa_start_indexes; //start index in vertex array above of each point array
    GLUINT_LIST *polygon_start_indexes; //start index in vertex_array above for each polygon
    GLUSHORT_LIST *element_array;    // a long array of triangle indexes
    GLUINT_LIST *element_start_indexes; //indexes telling where each polygon starts
    GLUINT_LIST *area_style_id;
    GLUINT_LIST *outline_style_id;
    GLuint vbo;
    GLuint ebo;
}
POLYGON_LIST;





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
    GLshort *size; //list if id to corresponding point array
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
    GLfloat lineWidth2;
    GLfloat z;
    GLint unit;
}
STYLES_RUNTIME;

/**

Information about all the layers in the project is loaded in an array of this structure at start.
*/
typedef struct
{
    char *name;
    uint8_t visible;
    sqlite3_stmt *preparedStatement;
    GLfloat *BBOX;
    uint8_t geometryType;
    uint8_t type; //8 on/off switches: point simple, point symbol, point text, line simple, line width, poly
    uint8_t n_dims;
    GLfloat minScale;
    GLfloat maxScale;
    POINT_LIST *points;
    LINESTRING_LIST *lines;
    LINESTRING_LIST *wide_lines;
    POLYGON_LIST *polygons;
    TEXTSTRUCT *text;
    INT64_LIST *twkb_id;
    int layer_id;
    int utm_zone;
    int hemisphere; //1 is southern hemisphere and 0 is northern
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
    unsigned int ch;			// max_character_height
    C metrics[256];
} ATLAS;







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
//    GLESSTRUCT *res_buf;
    LAYER_RUNTIME *theLayer;
    sqlite3_stmt *prepared_statement;
    int64_t id;  //the current id
    uint32_t styleID;  //the current styleID
    uint8_t line_width;  //If we shall calculate triangels to get line width
    uint8_t utm_zone;
    uint8_t hemisphere;
    uint8_t close_ring;
} TWKB_PARSE_STATE;



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
void copyNew2CurrentBBOX(GLfloat *newBBOX,GLfloat *currentBBOX);
int multiply_matrices(GLfloat *matrix1,GLfloat *matrix2, GLfloat *theMatrix);

/*This is functions for manipulating bbox, translations and zoom*/
int px2m(GLfloat *bbox,GLint px_x,GLint px_y,GLfloat *w_x,GLfloat *w_y);
int calc_translate(GLfloat w_x,GLfloat w_y, GLfloat *transl);
int calc_scale(GLfloat *bbox, GLfloat zoom, GLfloat *scale);
void windowResize(int newWidth,int newHeight,GLfloat *currentBBOX, GLfloat *newBBOX);
void initialBBOX(GLfloat x, GLfloat y, GLfloat width, GLfloat *newBBOX);

/*event handling*/
int matrixFromBboxPointZoom(GLfloat *currentBBOX,GLfloat *newBBOX,GLint px_x_clicked,GLint px_y_clicked, GLfloat zoom, GLfloat *theMatrix);
//int matrixFromDeltaMouse(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat mouse_down_x,GLfloat mouse_down_y,GLfloat mouse_up_x,GLfloat mouse_up_y, GLfloat *theMatrix);
int matrixFromDeltaMouse(GLfloat *currentBBOX, GLfloat *newBBOX, GLint mouse_down_x, GLint mouse_down_y, GLint mouse_up_x, GLint mouse_up_y, GLfloat *theMatrix);
LAYER_RUNTIME* init_layer_runtime(int n);
void destroy_layer_runtime(LAYER_RUNTIME *lr, int n);

int  matrixFromBBOX(GLfloat *newBBOX, GLfloat *theMatrix );
int get_data(SDL_Window* window,GLfloat *bbox,GLfloat *theMatrix);

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

FINGEREVENT* init_touch_que();
int reset_touch_que(FINGEREVENT *touches);
int get_box_from_touches(FINGEREVENT *touches,GLfloat *currentBBOX,GLfloat *newBBOX);
int register_touch_down(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);
int register_touch_up(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);
int register_motion(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y);

GLuint create_program(const unsigned char *vs_source,const unsigned char *fs_source, GLuint *vs, GLuint *fs);

void reset_shaders(GLuint vs,GLuint fs,GLuint program);

//uint32_t utf82unicode(char *text, char **the_rest);
int init_text_resources();
void render_txt(SDL_Window* window);

ATLAS* create_atlas(ATLAS *a, FT_Face face, int height);

void log_this(int log_level, const char *log_txt, ... );
int draw_it(GLfloat *color,GLfloat *point_coord, int atlas_nr,int bold,GLint txt_box,GLint txt_color,GLint txt_coord2d,char *txt,GLint max_width, float sx, float sy);
//int print_txt(float x,float y,float r, float g, float b, float a,int size, const char *txt, ... );
int print_txt(GLfloat *point_coord,GLfloat *color,int size,int bold,int max_width, const char *txt, ... );
int render_simple_rect(GLfloat minx, GLfloat miny, GLfloat maxx, GLfloat maxy);


void calc_start(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal);
void calc_join(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal);
void calc_end(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal);

int build_program();
int check_layer(const unsigned char *dbname, const unsigned char  *layername);


void gps_in(double latitude, double longitude, double acc);
int renderGPS(GLfloat *theMatrix);
int loadGPS(GLfloat *theMatrix);
GLfloat* create_circle(int npoints);
void reproject(GLfloat *points,uint8_t utm_in,uint8_t utm_out, uint8_t hemi_in, uint8_t hemi_out);
int check_column(const unsigned char *dbname,const unsigned char * layername, const unsigned char  *col_name);

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
void render_text_test(const char *text, float x, float y, float sx, float sy);

int  render_text(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix);
int CURR_WIDTH;
int CURR_HEIGHT;

FT_Library ft;

ATLAS *font_normal[3];

ATLAS *font_bold[3];


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

GLint gps_program;
GLint gps_norm;
GLint gps_coord2d;
GLint gps_radius;
GLint gps_color;
GLint gps_matrix;
GLint gps_px_matrix;


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

GLenum err;
#endif
