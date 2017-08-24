
#include "theclient.h"




#define INIT_LIST_SIZE 256


int init_buffers(LAYER_RUNTIME *layer);
int destroy_buffers(LAYER_RUNTIME *layer);

int reset_buffers(LAYER_RUNTIME *layer);

int add2glfloat_list(GLFLOAT_LIST *list, GLfloat val);
int add2gluint_list(GLUINT_LIST *list, GLuint val);
int add2int64_list(INT64_LIST *list, int64_t val);

int addbatch2glfloat_list(GLFLOAT_LIST *list,GLuint n_vals, GLfloat *vals);
int addbatch2int64_list(INT64_LIST *list,GLuint n_vals, int64_t *vals);
int addbatch2gluint_list(GLUINT_LIST *list,GLuint n_vals, GLuint *vals);
int addbatch2glushort_list(GLUSHORT_LIST *list,GLuint n_vals, GLushort *vals);

int setzero2gluint_list(GLUINT_LIST *list,GLuint n_vals);
int setzero2int64_list(INT64_LIST *list,int64_t n_vals);

int addbatch2uint8_list(UINT8_LIST *list,GLuint n_vals, uint8_t *vals);

GLFLOAT_LIST* get_coord_list(LAYER_RUNTIME *l, GLuint style_id);
GLFLOAT_LIST* get_wide_line_list(LAYER_RUNTIME *l, GLuint style_id);
int pa_end(LAYER_RUNTIME *l, int64_t id);



int init_symbols();
int addsymbol(int id, size_t n_points, GLfloat *points);
int destroy_symbols();
