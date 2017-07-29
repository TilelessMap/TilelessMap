#ifndef _matrix_handling_H
#define _matrix_handling_H


typedef struct
{
    GLfloat matrix[16];
    GLfloat bbox[4];
    uint8_t vertical_enabled;
    uint8_t horizontal_enabled;
    uint8_t zoom_enabled;
} MATRIX;







#endif
