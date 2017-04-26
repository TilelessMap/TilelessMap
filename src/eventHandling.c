/**********************************************************************
 *
 * TileLess
 *
 * TileLess is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * TileLess is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLess.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2017 Nicklas Avén
 *
 **********************************************************************/

#include "theclient.h"



int  matrixFromBBOX(GLfloat *newBBOX, GLfloat *theMatrix )
{
    GLfloat transl[16];
    GLfloat scale[16];
    GLfloat centerx, centery;

    centerx=newBBOX[0] + (newBBOX[2]-newBBOX[0]) * 0.5;
    centery=newBBOX[1] + (newBBOX[3]-newBBOX[1]) * 0.5;

    calc_scale(newBBOX,1, scale);
    calc_translate(centerx,centery, transl);
    multiply_matrices(scale,transl, theMatrix);
    return 0;
}

int matrixFromBboxPointZoom(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat px_x_clicked,GLfloat px_y_clicked, GLfloat zoom, GLfloat *theMatrix)
{

    log_this(10, "Entering get_bbox\n");
    GLfloat w_x_clicked,w_y_clicked;
    GLfloat w_x_center,w_y_center;
//GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat transl[16];
    GLfloat scale[16];

    px2m(currentBBOX,px_x_clicked, px_y_clicked, &w_x_clicked, &w_y_clicked);

    GLfloat original_w_width = (currentBBOX[2]-currentBBOX[0]);
    GLfloat original_w_height = (currentBBOX[3]-currentBBOX[1]);

    GLfloat original_w_x_dist2center = w_x_clicked - currentBBOX[0] - original_w_width * 0.5 ;
    GLfloat original_w_y_dist2center = w_y_clicked - currentBBOX[1] - original_w_height * 0.5;

    GLfloat new_w_x_dist2center = original_w_x_dist2center * zoom;
    GLfloat new_w_y_dist2center = original_w_y_dist2center * zoom;

    w_x_center = w_x_clicked - new_w_x_dist2center;
    w_y_center = w_y_clicked - new_w_y_dist2center;



    newBBOX[0]= w_x_center - original_w_width * 0.5 * zoom;
    newBBOX[1]= w_y_center - original_w_height * 0.5 * zoom;
    newBBOX[2]= w_x_center + original_w_width * 0.5 * zoom;
    newBBOX[3]= w_y_center + original_w_height * 0.5 * zoom;

    calc_scale(newBBOX,1, scale);
    calc_translate(w_x_center,w_y_center, transl);
    multiply_matrices(scale,transl, theMatrix);

    return 0;
}


int matrixFromDeltaMouse(GLfloat *currentBBOX,GLfloat *newBBOX,GLfloat mouse_down_x,GLfloat mouse_down_y,GLfloat mouse_up_x,GLfloat mouse_up_y, GLfloat *theMatrix)
{

//GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat transl[16];
    GLfloat scale[16];
    GLfloat centerx, centery;

    GLfloat width = currentBBOX[2]-currentBBOX[0];
    GLfloat height = currentBBOX[3]-currentBBOX[1];

    GLfloat deltax = - (mouse_up_x - mouse_down_x) * width/CURR_WIDTH;
    GLfloat deltay =  -(mouse_down_y - mouse_up_y) * height/CURR_HEIGHT;

    centerx=currentBBOX[0] + (currentBBOX[2]-currentBBOX[0]) * 0.5;
    centery=currentBBOX[1] + (currentBBOX[3]-currentBBOX[1]) * 0.5;

    centerx += deltax;
    centery += deltay;
	
    newBBOX[0] = currentBBOX[0]  + deltax;
    newBBOX[1] = currentBBOX[1]  + deltay;
    newBBOX[2] = currentBBOX[2]  + deltax;
    newBBOX[3] = currentBBOX[3]  + deltay;


    calc_scale(newBBOX,1, scale);
    calc_translate(centerx,centery, transl);
    multiply_matrices(scale,transl, theMatrix);

    return 0;
}
