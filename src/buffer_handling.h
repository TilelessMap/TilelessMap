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
#define INIT_LIST_SIZE 4


#define INT_TYPE 1
#define FLOAT_TYPE 2
#define STRING_TYPE 3

#define PIXEL_UNIT 0
#define METER_UNIT 1

typedef struct
{
    GLfloat *list;
    size_t alloced;
    size_t used;
}
GLFLOAT_LIST;


typedef struct
{
    uint8_t *list;
    size_t alloced;
    size_t used;
}
UINT8_LIST;

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
    void* *list;
    size_t alloced;
    size_t used;
}
POINTER_LIST;



typedef struct
{
    void *list;
    int list_type;
    GLUINT_LIST *s_start_indexes; //In the case of stored characters this variable holds where each string starts
    size_t alloced;
    size_t used;
}
UNION_LIST;


typedef struct
{
    GLFLOAT_LIST *points;
    GLUINT_LIST *point_start_indexes;
    POINTER_LIST *style_id;
    GLuint vbo;

}
POINT_LIST;


typedef struct
{
    GLFLOAT_LIST *vertex_array;
    GLUINT_LIST *line_start_indexes;
    POINTER_LIST *style_id;
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
    POINTER_LIST *style_id;
    POINTER_LIST *line_style_id;
    GLuint vbo;
    GLuint ebo;
}
POLYGON_LIST;



typedef struct
{
    UINT8_LIST *data;
    GLUINT_LIST *raster_start_indexes;
    GLUINT_LIST *tileidxy; //each 4 glfloat represents minx, maxx, miny, maxy in world coordinates
    GLuint cvbo;
    GLuint cibo;
    GLuint vbo;
    GLuint tilewidth;
    GLuint tileheight;
}
RASTER_LIST;





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

typedef struct
{
    
    sqlite3_stmt *ps;
    int usage;
}PS_HOLDER;

/**

Information about all the layers in the project is loaded in an array of this structure at start.
*/
typedef struct
{
    //General
    char *name;
    char *db;
    char *title;
    uint8_t visible;
    int layer_id;
    
    //Info for fetching data and rendering
    PS_HOLDER *preparedStatement;
    GLfloat *BBOX; // the requested bounding box (window)
    uint8_t geometryType;
    uint8_t type;
    uint8_t n_dims;
    GLfloat minScale;
    GLfloat maxScale;
    int utm_zone;
    int hemisphere; //1 is southern hemisphere and 0 is northern
    
    //What info to show when identify
    int info_active;
    char *info_rel;
    
    //Styling
    struct STYLES *styles;
    int style_key_type;
    
    //Buffers
    POINT_LIST *points;
    LINESTRING_LIST *lines;
    LINESTRING_LIST *wide_lines;
    POLYGON_LIST *polygons;
    TEXTSTRUCT *text;
    INT64_LIST *twkb_id;
    RASTER_LIST *rast;
    
}
LAYER_RUNTIME;


typedef struct
{
    LAYER_RUNTIME *layers;
    int nlayers;
    int max_nlayers;    
}
LAYERS;















/***************************************************************
			DECODING TWKB						*/
/*Holds a buffer with the twkb-data during read*/



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
#define	RASTER		        128



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
    sqlite3_stmt *info_prepared_statement;
    int64_t id;  //the current id
    union {
        int int_type;
        char string_type[128];
    } styleID; //the current styleID
    int styleid_type;
    uint8_t line_width;  //If we shall calculate triangels to get line width
    uint8_t utm_zone;
    uint8_t hemisphere;
    uint8_t close_ring;
} TWKB_PARSE_STATE;













typedef struct
{
    POINT_LIST *points;
} SYMBOLS;


SYMBOLS *global_symbols;








int init_buffers(LAYER_RUNTIME *layer);
int destroy_buffers(LAYER_RUNTIME *layer);
int destroy_symbol_list(SYMBOLS *l);
int reset_buffers(LAYER_RUNTIME *layer);


UINT8_LIST* init_uint8_list();
GLFLOAT_LIST* init_glfloat_list();
GLUSHORT_LIST* init_glushort_list();
SYMBOLS* init_symbol_list();
UNION_LIST* init_union_list();
POINTER_LIST* init_pointer_list();
GLUINT_LIST* init_gluint_list();

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

int add2pointer_list(POINTER_LIST *list, void *val);
int setzero2pointer_list(POINTER_LIST *list,GLuint n_vals);
int setzero2int64_list(INT64_LIST *list,int64_t n_vals);
int reset_gluint_list(GLUINT_LIST *l);
int addbatch2uint8_list(UINT8_LIST *list,GLuint n_vals, uint8_t *vals);


int destroy_glfloat_list(GLFLOAT_LIST *l);
int destroy_gluint_list(GLUINT_LIST *l);
int destroy_int64_list(INT64_LIST *l);
int destroy_glushort_list(GLUSHORT_LIST *l);
int destroy_pointer_list(POINTER_LIST *l);
int destroy_uint8_list(UINT8_LIST *l);

GLFLOAT_LIST* get_coord_list(LAYER_RUNTIME *l, TWKB_PARSE_STATE *ts);
GLFLOAT_LIST* get_wide_line_list(LAYER_RUNTIME *l, TWKB_PARSE_STATE *ts);
int pa_end(LAYER_RUNTIME *l, int64_t id);



int init_symbols();
int addsym(uint8_t id, size_t n_points, GLfloat *points);
//int destroy_symbols();




int get_style(struct STYLES *styles, POINTER_LIST *list, void *val,int val_type);








struct STYLES *system_default_style;
struct STYLES *system_default_info_style;




























#endif
