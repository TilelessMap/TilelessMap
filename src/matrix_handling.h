/**********************************************************************
 *
 * TileLessMap
 *
 * TileLessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TileLessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/
 
 
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
