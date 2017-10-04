
#ifndef _utils_H
#define _utils_H

int multiply_short_array(GLshort *a, GLfloat v, GLshort ndims);
int multiply_float_array(float *a, float v, GLshort ndims);

float min_f(float a, float b);
float max_f(float a, float b);
/*utils*/
void initialBBOX(GLfloat x, GLfloat y, GLfloat width, MATRIX *map_matrix);
int reset_matrix(MATRIX *matrix_handl);
void copy2ref_box(MATRIX *matrix_hndl,MATRIX *ref);
int multiply_matrices(GLfloat *matrix1,GLfloat *matrix2, GLfloat *theMatrix);
int multiply_matrix_vektor(GLfloat *matrix,GLfloat *vektor_in, GLfloat *vektor_out);

/*This is functions for manipulating bbox, translations and zoom*/
int px2m(GLfloat *bbox,GLint px_x,GLint px_y,GLfloat *w_x,GLfloat *w_y);
int calc_translate(GLfloat w_x,GLfloat w_y, GLfloat *transl);
int calc_scale(GLfloat *bbox, GLfloat zoom, GLfloat *scale);
void windowResize(int newWidth,int newHeight,MATRIX *matrix_hndl,MATRIX *out);


int search_string(const char *w,const  char *s);


#endif
