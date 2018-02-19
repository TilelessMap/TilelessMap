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
