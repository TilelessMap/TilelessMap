
#include "theclient.h"




#define INIT_LIST_SIZE 256


int init_buffers(LAYER_RUNTIME *layer);

int reset_buffers(LAYER_RUNTIME *layer);

int add2glfloat_list(GLFLOAT_LIST *list, GLfloat val);
int add2gluint_list(GLUINT_LIST *list, GLuint val);

int addbatch2glfloat_list(GLFLOAT_LIST *list,GLuint n_vals, GLfloat *vals);
int addbatch2glushort_list(GLUSHORT_LIST *list,GLuint n_vals, GLushort *vals);
GLFLOAT_LIST* get_coord_list(LAYER_RUNTIME *l, GLuint style_id);
GLFLOAT_LIST* get_wide_line_list(LAYER_RUNTIME *l, GLuint style_id);
int pa_end(LAYER_RUNTIME *l);
