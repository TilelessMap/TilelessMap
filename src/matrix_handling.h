#ifndef _matrix_handling_H
#define _matrix_handling_H

#include <stdint.h>
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif


typedef struct
{
    GLfloat matrix[16];
    GLfloat bbox[4];
    uint8_t vertical_enabled;
    uint8_t horizontal_enabled;
    uint8_t zoom_enabled;
} MATRIX;







#endif
