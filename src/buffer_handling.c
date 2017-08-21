#include "theclient.h"
#include "mem.h"
#include "buffer_handling.h"

/************* GL Float List ********************/


static GLFLOAT_LIST* init_glfloat_list()
{
    GLFLOAT_LIST *res = (GLFLOAT_LIST*) st_malloc(sizeof(GLFLOAT_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(GLfloat));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_glfloat_list(GLFLOAT_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(GLfloat));
    l->alloced = new_size;
    return 0;
}

static int reset_glfloat_list(GLFLOAT_LIST *l)
{
    l->used = 0;
    return 0;
}


static int destroy_glfloat_list(GLFLOAT_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2glfloat_list(GLFLOAT_LIST *list, GLfloat val)
{
    increase_glfloat_list(list, 1);
    *(list->list + list->used) = val;
    (list->used)++;
    return 0;
}



int addbatch2glfloat_list(GLFLOAT_LIST *list,GLuint n_vals, GLfloat *vals)
{
    increase_glfloat_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(GLfloat));
    list->used += n_vals;
    return 0;
}




/************* GLUInt List ********************/
static GLUINT_LIST* init_gluint_list()
{
    GLUINT_LIST *res = (GLUINT_LIST*) st_malloc(sizeof(GLUINT_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(GLuint));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_gluint_list(GLUINT_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(GLuint));
    l->alloced = new_size;
    return 0;
}

static int reset_gluint_list(GLUINT_LIST *l)
{
    l->used = 0;
    return 0;
}


static int destroy_gluint_list(GLUINT_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2gluint_list(GLUINT_LIST *list, GLuint val)
{
    increase_gluint_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}

int addbatch2gluint_list(GLUINT_LIST *list,GLuint n_vals, GLuint *vals)
{
    increase_gluint_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(GLuint));
    list->used += n_vals;
    return 0;
}

int setzero2gluint_list(GLUINT_LIST *list,GLuint n_vals)
{
    increase_gluint_list(list, n_vals);
    memset(list->list + list->used, 0, n_vals * sizeof(GLuint));
    list->used += n_vals;
    return 0;
}


/************* int64 List ********************/
static INT64_LIST* init_int64_list()
{
    INT64_LIST *res = (INT64_LIST*) st_malloc(sizeof(INT64_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(int64_t));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_int64_list(INT64_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(int64_t));
    l->alloced = new_size;
    return 0;
}

static int reset_int64_list(INT64_LIST *l)
{
    l->used = 0;
    return 0;
}


static int destroy_int64_list(INT64_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2int64_list(INT64_LIST *list, int64_t val)
{
    increase_int64_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}

int addbatch2int64_list(INT64_LIST *list,GLuint n_vals, int64_t *vals)
{
    increase_int64_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(int64_t));
    list->used += n_vals;
    return 0;
}

int setzero2int64_list(INT64_LIST *list,int64_t n_vals)
{
    increase_int64_list(list, n_vals);
    memset(list->list + list->used, 0, n_vals * sizeof(int64_t));
    list->used += n_vals;
    return 0;
}

/************* GL UShort List ********************/
static GLUSHORT_LIST* init_glushort_list()
{
    GLUSHORT_LIST *res = (GLUSHORT_LIST*) st_malloc(sizeof(GLUSHORT_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(GLushort));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_glushort_list(GLUSHORT_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(GLushort));
    l->alloced = new_size;
    return 0;
}

static int reset_glushort_list(GLUSHORT_LIST *l)
{
    l->used = 0;
    return 0;
}


static int destroy_glushort_list(GLUSHORT_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2glushort_list(GLUSHORT_LIST *list, GLshort val)
{
    increase_glushort_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}


int addbatch2glushort_list(GLUSHORT_LIST *list,GLuint n_vals, GLushort *vals)
{
    increase_glushort_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(GLushort));
    list->used += n_vals;
    return 0;
}

/************* UINT8_LIST List ********************/
static UINT8_LIST* init_uint8_list()
{
    UINT8_LIST *res = (UINT8_LIST*) st_malloc(sizeof(UINT8_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(uint8_t));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_uint8_list(UINT8_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(uint8_t));
    l->alloced = new_size;
    return 0;
}

static int reset_uint8_list(UINT8_LIST *l)
{
    l->used = 0;
    return 0;
}


static int destroy_uint8_list(UINT8_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2uint8_list(UINT8_LIST *list, uint8_t val)
{
    increase_uint8_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}


int addbatch2uint8_list(UINT8_LIST *list,GLuint n_vals, uint8_t *vals)
{
    increase_uint8_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(uint8_t));
    list->used += n_vals;
    return 0;
}

static RASTER_LIST* init_raster_list()
{
    RASTER_LIST *res = st_malloc(sizeof(RASTER_LIST));
    res->data = init_uint8_list();
    res->raster_start_indexes = init_gluint_list();
    res->tileidxy = init_gluint_list();
    glGenBuffers(1, &(res->tex_vbo));
    glGenBuffers(1, &(res->tex_ebo));
    glGenBuffers(1, &(res->vbo));
    glGenTextures(1, &(res->tex));
    return res;
}
static POINT_LIST* init_point_list()
{
    POINT_LIST *res = st_malloc(sizeof(POINT_LIST));
    res->points = init_glfloat_list();
    res->style_id = init_gluint_list();
    res->point_start_indexes = init_gluint_list();
    glGenBuffers(1, &(res->vbo));
    return res;
}

static LINESTRING_LIST* init_linestring_list()
{
    LINESTRING_LIST *res = st_malloc(sizeof(LINESTRING_LIST));
    res->vertex_array = init_glfloat_list();
    res->line_start_indexes = init_gluint_list();
    res->style_id = init_gluint_list();
    glGenBuffers(1, &(res->vbo));

    return res;
}

static POLYGON_LIST* init_polygon_list()
{
    POLYGON_LIST *res = st_malloc(sizeof(POLYGON_LIST));
    res->vertex_array = init_glfloat_list();
    res->pa_start_indexes = init_gluint_list();
    res->polygon_start_indexes = init_gluint_list();
    res->element_array = init_glushort_list();
    res->element_start_indexes = init_gluint_list();
    res->outline_style_id = init_gluint_list();
    res->area_style_id = init_gluint_list();

    glGenBuffers(1, &(res->vbo));
    glGenBuffers(1, &(res->ebo));
    return res;
}



static int reset_raster_list(RASTER_LIST *l)
{
    reset_gluint_list(l->tileidxy);
    reset_gluint_list(l->raster_start_indexes);
    reset_uint8_list(l->data);
    return 0;
}
static int reset_point_list(POINT_LIST *l)
{
    reset_glfloat_list(l->points);
    reset_gluint_list(l->point_start_indexes);
    reset_gluint_list(l->style_id);
    return 0;
}

static int reset_linestring_list(LINESTRING_LIST *l)
{
    reset_glfloat_list(l->vertex_array);
    reset_gluint_list(l->line_start_indexes);
    reset_gluint_list(l->style_id);

    return 0;
}

static int reset_polygon_list(POLYGON_LIST *l)
{
    reset_glfloat_list(l->vertex_array);
    reset_gluint_list(l->pa_start_indexes);
    reset_gluint_list(l->polygon_start_indexes);
    reset_glushort_list(l->element_array);
    reset_gluint_list(l->element_start_indexes);
    reset_gluint_list(l->outline_style_id);
    reset_gluint_list(l->area_style_id);
    return 0;
}


static int destroy_raster_list(RASTER_LIST *l)
{
    destroy_gluint_list(l->tileidxy);
    destroy_gluint_list(l->raster_start_indexes);
    destroy_uint8_list(l->data);
    free(l);
    return 0;
}
static int destroy_point_list(POINT_LIST *l)
{
    destroy_glfloat_list(l->points);
    destroy_gluint_list(l->point_start_indexes);
    destroy_gluint_list(l->style_id);
    free(l);
    return 0;
}

static int destroy_linestring_list(LINESTRING_LIST *l)
{

    destroy_glfloat_list(l->vertex_array);
    destroy_gluint_list(l->line_start_indexes);
    destroy_gluint_list(l->style_id);
    free(l);
    return 0;

}

static int destroy_polygon_list(POLYGON_LIST *l)
{
    destroy_glfloat_list(l->vertex_array);
    destroy_gluint_list(l->pa_start_indexes);
    destroy_gluint_list(l->polygon_start_indexes);
    destroy_glushort_list(l->element_array);
    destroy_gluint_list(l->element_start_indexes);
    destroy_gluint_list(l->outline_style_id);
    destroy_gluint_list(l->area_style_id);
    free(l);
    return 0;
}







int init_buffers(LAYER_RUNTIME *layer)
{
    if(layer->type & 224)
        layer->points = init_point_list();
    else
        layer->points = NULL;
    if(layer->type & 16)
        layer->lines = init_linestring_list();
    else
        layer->lines = NULL;
    if(layer->type & 8)
        layer->wide_lines = init_linestring_list();
    else
        layer->wide_lines = NULL;
    if (layer->type & 6)
        layer->polygons = init_polygon_list();
    else
        layer->polygons = NULL;

    layer->twkb_id = init_int64_list();
    
    if(layer->geometryType == RASTER)
        layer->rast = init_raster_list();
    //  layer->style_id = init_gluint_list();
    return 0;
}


int reset_buffers(LAYER_RUNTIME *layer)
{
    if(layer->points)
        reset_point_list(layer->points);
    if(layer->lines)
        reset_linestring_list(layer->lines);
    if(layer->wide_lines)
        reset_linestring_list(layer->wide_lines);
    if(layer->polygons)
        reset_polygon_list(layer->polygons);

    if(layer->twkb_id)
        reset_int64_list(layer->twkb_id);
    
    if(layer->geometryType == RASTER)
        reset_raster_list(layer->rast);
    //  reset_gluint_list(layer->style_id);
    return 0;
}


GLFLOAT_LIST* get_coord_list(LAYER_RUNTIME *l, GLuint style_id)
{
//   add2gluint_list(l->style_id, style_id);
    int type = l->type;
    if(type & 224)
    {
        add2gluint_list(l->points->style_id, style_id);
        return l->points->points;
    }
    else if(type & 16)
    {
        add2gluint_list(l->lines->style_id, style_id);
        return l->lines->vertex_array;
    }

    else if(type & 6)
    {
        add2gluint_list(l->polygons->outline_style_id, style_id);
        return l->polygons->vertex_array;
    }
    else
        return NULL;


}


GLFLOAT_LIST* get_wide_line_list(LAYER_RUNTIME *l, GLuint style_id)
{

    add2gluint_list(l->wide_lines->style_id, style_id);
    return l->wide_lines->vertex_array;

}

int pa_end(LAYER_RUNTIME *l, int64_t id)
{
    add2int64_list(l->twkb_id, id);

    int type = l->type;
    if(type & 224)
        add2gluint_list(l->points->point_start_indexes, l->points->points->used);
    if(type & 16)
        add2gluint_list(l->lines->line_start_indexes, l->lines->vertex_array->used);
    if(type & 8)
        add2gluint_list(l->wide_lines->line_start_indexes, l->wide_lines->vertex_array->used);
    if(type & 6)
        add2gluint_list(l->polygons->pa_start_indexes, l->polygons->vertex_array->used);


    return 0;
}





int destroy_buffers(LAYER_RUNTIME *layer)

{
    if(layer->type & 224)
        destroy_point_list(layer->points);
    if(layer->type & 16)
        destroy_linestring_list(layer->lines);
    if(layer->type & 8)
        destroy_linestring_list(layer->wide_lines);
    if (layer->type & 4)
        destroy_polygon_list(layer->polygons);

    destroy_int64_list(layer->twkb_id);
    
    if(layer->geometryType == RASTER)
        destroy_raster_list(layer->rast);
    return 0;
}







