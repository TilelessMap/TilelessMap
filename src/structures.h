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
#ifndef _structures_H
#define _structures_H

#include <stdint.h>

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

#include "ext/sqlite/sqlite3.h"



#define TWKB_IN_MAXCOORDS 4


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
    
    sqlite3_stmt *ps;
    int usage;
}PS_HOLDER;




typedef struct
{
    GLFLOAT_LIST *points;
    GLUINT_LIST *point_start_indexes;
    POINTER_LIST *style_id;
    GLuint vbo;
    GLuint tbo;

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
    int pointlist_owner; 
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
    TEXTBLOCK *tb;
}
TEXTSTRUCT;


typedef struct
{
    POINT_LIST *points;
} SYMBOLS;


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
    UINT8_LIST *rawdata;
}
LAYER_RUNTIME;


typedef struct
{
    LAYER_RUNTIME *layers;
    int nlayers;
    int max_nlayers;    
}
LAYERS;





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
    const char *txt;
    uint8_t line_width;  //If we shall calculate triangels to get line width
    uint8_t utm_zone;
    uint8_t hemisphere;
    uint8_t close_ring;
    WCHAR_TEXT *unicode_txt; //place for temporary unicode text used for text layers
    
} TWKB_PARSE_STATE;



#endif
