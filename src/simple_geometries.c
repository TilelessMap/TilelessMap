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




int render_simple_Polygon()
{

    GLuint vbo;
    GLuint ebo;
    GLenum err;


    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat)(2.0 / CURR_HEIGHT);


    GLfloat theMatrix[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};
    //  GLfloat theMatrix[16] = {1, 0,0,0,0,1,0,0,0,0,1,0,-0.5,0,0,1};


    GLfloat punkter[] = {5,75,5, 225,300,225,300,75};
    //  GLfloat punkter[] = {-0.2,-0.2,-0.2,0.2,0.5,0.2,0.5,-0.2};
    GLshort tri_index[] = {0,1,3,1,2,3};

    GLfloat color[] = {(GLfloat) 1.0,(GLfloat) 1.0,(GLfloat) 1.0,(GLfloat) 0.9};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, punkter, GL_STATIC_DRAW);


    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort)*6, tri_index, GL_STATIC_DRAW);





//    GLenum err;
    glUseProgram(std_program);


    glUniform4fv(std_color,1,color );
    glUniformMatrix4fv(std_matrix, 1, GL_FALSE,theMatrix );

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(std_coord2d);

    glVertexAttribPointer(
        std_coord2d, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
    /*    while ((err = glGetError()) != GL_NO_ERROR) {
            log_this(10, "Problem1\n");
            fprintf(stderr,"opengl error:%d", err);
        }*/


//glDrawArrays(GL_TRIANGLES, 0, 3);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"opengl error 100:%d\n", err);
    }








    glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_SHORT,NULL);


    glDisableVertexAttribArray(std_coord2d);

    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"opengl error 900:%d\n", err);
    }
    return 0;

}


