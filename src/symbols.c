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
#include "symbols.h"
#include "mem.h"
#define _USE_MATH_DEFINES //This is for windows
#include <math.h>
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

#include "buffer_handling.h"

int init_symbols()
{

    GLfloat* points, first_len, second_len, rotation;

    global_symbols = init_symbol_list();
    int n_dirs;
    int symbolid;

    /*Create a square*/
    symbolid = SQUARE_SYMBOL;
    n_dirs = 4;
    first_len = 1;
    second_len = 1;
    rotation = 45;

    points = create_symbol(n_dirs,first_len, second_len, rotation);
    addsym(symbolid, (n_dirs + 2) * 2, points);
    st_free(points);


    /*Create a circle*/
    symbolid = CIRCLE_SYMBOL;
    n_dirs = 16;
    first_len = 1;
    second_len = 1;
    rotation = 0;

    points = create_symbol(n_dirs,first_len, second_len, rotation);
    addsym(symbolid, (n_dirs + 2) * 2, points);
    st_free(points);


    /*Create a triangle*/
    symbolid = TRIANGLE_SYMBOL;
    n_dirs = 3;
    first_len = 1;
    second_len = 1;
    rotation = 0;

    points = create_symbol(n_dirs,first_len, second_len, rotation);
    addsym(symbolid, (n_dirs + 2) * 2, points);
    st_free(points);

    /*Create a star*/
    symbolid = STAR_SYMBOL;
    n_dirs = 10;
    first_len = 1;
    second_len = (GLfloat) 0.3;
    rotation = 0;

    points = create_symbol(n_dirs,first_len, second_len, rotation);
    addsym(symbolid, (n_dirs + 2) * 2, points);
    st_free(points);



    return 0;

}



GLfloat* create_circle(int npoints)
{
    GLfloat *res = st_malloc((npoints + 2) * 2 * sizeof(GLfloat));

    double rad;
    int i, res_pos = 0;

    //add center of point at 0,0
    res[res_pos++] = 0;
    res[res_pos++] = 0;
    for (i = 0; i<npoints/2; i++)
    {
        rad = i * M_PI/(npoints/2);
        res[res_pos++] = (GLfloat) sin(rad);
        res[res_pos++] = (GLfloat)cos(rad);
    }
    for (i = npoints/2; i>0; i--)
    {
        rad = i * M_PI/(npoints/2);
        res[res_pos++] = (GLfloat)-sin(rad);
        res[res_pos++] = (GLfloat)cos(rad);
    }
    //add first point again
    res[res_pos++] = (GLfloat)sin(0);
    res[res_pos++] = (GLfloat)cos(0);

    return  res;

}



GLfloat* create_symbol(int npoints, float even, float odd,float rotation)
{
    GLfloat *res = st_malloc((npoints + 2) * 2 * sizeof(GLfloat));

    double rad;
    int i, res_pos = 0;
    float e=odd;
    float rot_rad = (float) (rotation * 2 * M_PI/360);
    //add center of point at 0,0
    res[res_pos++] = 0;
    res[res_pos++] = 0;
    for (i = 0; i<npoints; i++)
    {
        rad = rot_rad + i * 2* M_PI/npoints;
        res[res_pos++] = (GLfloat) sin(rad)*e;
        res[res_pos++] = (GLfloat) cos(rad)*e;
        if(e==odd)
            e = even;
        else
            e=odd;

    }
    //add first point again
    rad = rot_rad + 2 *M_PI;
    res[res_pos++] = (GLfloat) sin(rad)*e;
    res[res_pos++] = (GLfloat) cos(rad)*e;

    return  res;

}



