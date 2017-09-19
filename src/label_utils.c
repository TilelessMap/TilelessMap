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





//int print_txt(float x,float y,float r, float g, float b, float a,int size,int max_width, const char *txt, ... )
int print_txt(GLfloat *point_coord,GLfloat *point_offset, MATRIX *matrix_hndl,GLfloat *color,int size,int bold,int max_width, const char *txt, ... )
{


    char txt_tot[1024];


       ATLAS *a;
    va_list args;
    va_start (args, txt);
    vsnprintf (txt_tot,1024,txt, args);
    va_end (args);
    GLfloat *theMatrix;
    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat)(2.0 / CURR_HEIGHT);

    GLfloat matrix_array[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};
    if(matrix_hndl)
        theMatrix = matrix_hndl->matrix;
    else
    {
        theMatrix = (GLfloat *) matrix_array;
    }


    glGenBuffers(1, &text_vbo);
    glUseProgram(txt_program);


    GLfloat norm_color[4];

    norm_color[0] = color[0] / 255;
    norm_color[1] = color[1] / 255;
    norm_color[2] = color[2] / 255;
    norm_color[3] = color[3] / 255;

    /*   GLfloat point_coord[2];

       point_coord[0]= x;
       point_coord[1]= y;
     */
    //  glUniform4fv(txt_color,1,norm_color );

    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error wt:%d\n", err);
    }

    glUniformMatrix4fv(txt_matrix, 1, GL_FALSE,theMatrix );
    
    
    if(bold)
    {
     //   a = font_bold[atlas_nr-1];
        a = loadatlas("freesans",BOLD_TYPE, size);
        
    }
    else
    {
        //a = font_normal[atlas_nr-1];
        
        a = loadatlas("freesans",NORMAL_TYPE, size);
    }
    
    
    draw_it(norm_color,point_coord,point_offset, a, txt_box, txt_color, txt_coord2d, txt_tot,max_width, sx, sy);


    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error wt2:%d\n", err);
    }
    return 0;
}



static inline GLfloat max_f(GLfloat a, GLfloat b)
{
    if (b > a)
        return b;
    else
        return a;
}

int print_txtblock(GLfloat *point_coord, MATRIX *matrix_hndl, GLfloat *color,int max_width, TEXTBLOCK *tb)
{
    int i;
    GLfloat point_offset[] = {0,0};
    GLfloat *theMatrix;
    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat)(2.0 / CURR_HEIGHT);

    GLfloat matrix_array[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};
    if(matrix_hndl)
        theMatrix = matrix_hndl->matrix;
    else
    {
        theMatrix = (GLfloat *) matrix_array;
    }


    glGenBuffers(1, &text_vbo);
    glUseProgram(txt_program);


    GLfloat norm_color[4];

    norm_color[0] = color[0] / 255;
    norm_color[1] = color[1] / 255;
    norm_color[2] = color[2] / 255;
    norm_color[3] = color[3] / 255;

    /*   GLfloat point_coord[2];

       point_coord[0]= x;
       point_coord[1]= y;
     */
    //  glUniform4fv(txt_color,1,norm_color );

    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error wt:%d\n", err);
    }

    glUniformMatrix4fv(txt_matrix, 1, GL_FALSE,theMatrix );
    
    
    for (i=0;i<tb->n_txts;i++)
    {
        draw_it(norm_color,point_coord,point_offset, tb->font[i], txt_box, txt_color, txt_coord2d, tb->txt[i]->txt ,max_width, sx, sy);
    }
    
    
    return 0;
}
