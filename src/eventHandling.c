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

#include "theclient.h"
#include "interface/interface.h"
#include "utils.h"

/*int  matrixFromBBOX(GLfloat *newBBOX, GLfloat *theMatrix )
{
    GLfloat transl[16];
    GLfloat scale[16];
    GLfloat centerx, centery;

    centerx= (GLfloat) (newBBOX[0] + (newBBOX[2]-newBBOX[0]) * 0.5);
    centery= (GLfloat)(newBBOX[1] + (newBBOX[3]-newBBOX[1]) * 0.5);

    calc_scale(newBBOX,1, scale);
    calc_translate(centerx,centery, transl);
    multiply_matrices(scale,transl, theMatrix);
    return 0;
}*/
int  matrixFromBBOX(MATRIX *matrix_hndl)
{
    GLfloat transl[16];
    GLfloat scale[16];
    GLfloat centerx, centery;

    centerx= (GLfloat) (matrix_hndl->bbox[0] + (matrix_hndl->bbox[2]-matrix_hndl->bbox[0]) * 0.5);
    centery= (GLfloat)(matrix_hndl->bbox[1] + (matrix_hndl->bbox[3]-matrix_hndl->bbox[1]) * 0.5);

    calc_scale(matrix_hndl->bbox,1, scale);
    calc_translate(centerx,centery, transl);
    multiply_matrices(scale,transl, matrix_hndl->matrix);
    return 0;
}
/*
int matrixFromBboxPointZoom(MATRIX *matrix_hndl, GLint px_x_clicked, GLint px_y_clicked, GLfloat zoom)
{

    log_this(10, "Entering get_bbox\n");
    GLfloat w_x_clicked,w_y_clicked;
    GLfloat w_x_center,w_y_center;
//GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat transl[16];
    GLfloat scale[16];

    px2m(matrix_hndl->bbox,px_x_clicked, px_y_clicked, &w_x_clicked, &w_y_clicked);

    GLfloat original_w_width = (matrix_hndl->bbox[2]-matrix_hndl->bbox[0]);
    GLfloat original_w_height = (matrix_hndl->bbox[3]-matrix_hndl->bbox[1]);

    GLfloat original_w_x_dist2center = (GLfloat) (w_x_clicked - matrix_hndl->bbox[0] - original_w_width * 0.5);
    GLfloat original_w_y_dist2center = (GLfloat)(w_y_clicked - matrix_hndl->bbox[1] - original_w_height * 0.5);

    GLfloat new_w_x_dist2center = (GLfloat)(original_w_x_dist2center * zoom);
    GLfloat new_w_y_dist2center = (GLfloat)(original_w_y_dist2center * zoom);

    w_x_center = w_x_clicked - new_w_x_dist2center;
    w_y_center = w_y_clicked - new_w_y_dist2center;



    matrix_hndl->bbox[0]= (GLfloat)(w_x_center - original_w_width * 0.5 * zoom);
    matrix_hndl->bbox[1]= (GLfloat)(w_y_center - original_w_height * 0.5 * zoom);
    matrix_hndl->bbox[2]= (GLfloat)(w_x_center + original_w_width * 0.5 * zoom);
    matrix_hndl->bbox[3]= (GLfloat)(w_y_center + original_w_height * 0.5 * zoom);

    calc_scale(matrix_hndl->bbox,1, scale);
    calc_translate(w_x_center,w_y_center, transl);
    multiply_matrices(scale,transl, matrix_hndl->matrix);

    return 0;
}


int matrixFromDeltaMouse(MATRIX *matrix_hndl,GLint mouse_down_x, GLint mouse_down_y, GLint mouse_up_x, GLint mouse_up_y)
{

//GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat transl[16];
    GLfloat scale[16];
    GLfloat centerx, centery;

    GLfloat width = matrix_hndl->bbox[2]-matrix_hndl->bbox[0];
    GLfloat height = matrix_hndl->bbox[3]-matrix_hndl->bbox[1];

    GLfloat deltax = - (mouse_up_x - mouse_down_x) * width/CURR_WIDTH;
    GLfloat deltay =  -(mouse_down_y - mouse_up_y) * height/CURR_HEIGHT;

    centerx= (GLfloat)(matrix_hndl->bbox[0] + (matrix_hndl->bbox[2]-matrix_hndl->bbox[0]) * 0.5);
    centery= (GLfloat)(matrix_hndl->bbox[1] + (matrix_hndl->bbox[3]-matrix_hndl->bbox[1]) * 0.5);

    centerx += deltax;
    centery += deltay;

    matrix_hndl->bbox[0] += deltax;
    matrix_hndl->bbox[1] += deltay;
    matrix_hndl->bbox[2] += deltax;
    matrix_hndl->bbox[3] += deltay;


    calc_scale(matrix_hndl->bbox,1, scale);
    calc_translate(centerx,centery, transl);
    multiply_matrices(scale,transl, matrix_hndl->matrix);

    return 0;
}*/

int matrixFromBboxPointZoom(MATRIX *matrix_hndl,MATRIX *ref, GLint px_x_clicked, GLint px_y_clicked, GLfloat zoom)
{

    log_this(10, "Entering get_bbox\n");

    if(! matrix_hndl->zoom_enabled)
        zoom = 1;

    GLfloat w_x_clicked,w_y_clicked;
    GLfloat w_x_center,w_y_center;
    GLfloat new_w_x_dist2center, new_w_y_dist2center;
//GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat transl[16];
    GLfloat scale[16];

    px2m(ref->bbox,px_x_clicked, px_y_clicked, &w_x_clicked, &w_y_clicked);

    GLfloat original_w_width = (ref->bbox[2]-ref->bbox[0]);
    GLfloat original_w_height = (ref->bbox[3]-ref->bbox[1]);

    GLfloat original_w_x_dist2center = (GLfloat) (w_x_clicked - ref->bbox[0] - original_w_width * 0.5);
    GLfloat original_w_y_dist2center = (GLfloat)(w_y_clicked - ref->bbox[1] - original_w_height * 0.5);


    new_w_x_dist2center = (GLfloat)(original_w_x_dist2center * zoom);



    new_w_y_dist2center = (GLfloat)(original_w_y_dist2center * zoom);


    if(matrix_hndl->horizontal_enabled)
        w_x_center = w_x_clicked - new_w_x_dist2center;
    else
        w_x_center = matrix_hndl->bbox[0] + original_w_width * 0.5;

    if(matrix_hndl->vertical_enabled)
        w_y_center = w_y_clicked - new_w_y_dist2center;
    else
        w_y_center = matrix_hndl->bbox[1] + original_w_height * 0.5;


    matrix_hndl->bbox[0]= (GLfloat)(w_x_center - original_w_width * 0.5 * zoom);
    matrix_hndl->bbox[2]= (GLfloat)(w_x_center + original_w_width * 0.5 * zoom);
    matrix_hndl->bbox[1]= (GLfloat)(w_y_center - original_w_height * 0.5 * zoom);
    matrix_hndl->bbox[3]= (GLfloat)(w_y_center + original_w_height * 0.5 * zoom);

    calc_scale(matrix_hndl->bbox,1, scale);
    calc_translate(w_x_center,w_y_center, transl);
    multiply_matrices(scale,transl, matrix_hndl->matrix);

    return 0;
}


int matrixFromDeltaMouse(MATRIX *matrix_hndl,MATRIX *ref,GLint mouse_down_x, GLint mouse_down_y, GLint mouse_up_x, GLint mouse_up_y)
{

//GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat transl[16];
    GLfloat scale[16];
    GLfloat centerx, centery, deltax, deltay;

    if(matrix_hndl->horizontal_enabled)
    {
        GLfloat width = ref->bbox[2]-ref->bbox[0];
        deltax = - (mouse_up_x - mouse_down_x) * width/CURR_WIDTH;
    }
    else
        deltax = 0;

    if(matrix_hndl->vertical_enabled)
    {
        GLfloat height = ref->bbox[3]-ref->bbox[1];
        deltay =  -(mouse_down_y - mouse_up_y) * height/CURR_HEIGHT;
    }
    else
        deltay = 0;

    centerx= (GLfloat)(ref->bbox[0] + (ref->bbox[2]-ref->bbox[0]) * 0.5);
    centery= (GLfloat)(ref->bbox[1] + (ref->bbox[3]-ref->bbox[1]) * 0.5);

    centerx += deltax;
    centery += deltay;

    matrix_hndl->bbox[0] = ref->bbox[0]  + deltax;
    matrix_hndl->bbox[1] = ref->bbox[1]  + deltay;
    matrix_hndl->bbox[2] = ref->bbox[2]  + deltax;
    matrix_hndl->bbox[3] = ref->bbox[3]  + deltay;


    calc_scale(matrix_hndl->bbox,1, scale);
    calc_translate(centerx,centery, transl);
    multiply_matrices(scale,transl, matrix_hndl->matrix);

    return 0;
}
