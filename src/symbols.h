
#ifndef _symbols_H
#define _symbols_H
#include "buffer_handling.h"


GLfloat* create_circle(int npoints); //TODO: remove this and replace with create symbols
GLfloat* create_symbol(int npoints, float even, float odd,float rotation);


#define SQUARE_SYMBOL 1
#define CIRCLE_SYMBOL 2
#define TRIANGLE_SYMBOL 3
#define STAR_SYMBOL 4



#endif
