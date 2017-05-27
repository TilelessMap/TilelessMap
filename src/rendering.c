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

int loadPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{


    if(oneLayer->show_text)
    {

        render_text(oneLayer,theMatrix);
        return 0;
    }
    GLESSTRUCT *rb = oneLayer->res_buf;

    glGenBuffers(1, &(oneLayer->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->first_free-rb->vertex_array), rb->vertex_array, GL_STATIC_DRAW);

    renderPoint( oneLayer, theMatrix);
    return 0;
}

int renderPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    int ndims = 2;
    uint32_t i;//, np, pi;
    GLfloat *color;
    GLfloat c[4];
    //  GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;
    // glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(2);
    glUseProgram(std_program);
    glEnableVertexAttribArray(std_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
        std_coord2d, // attribute
        ndims,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
    /*   while ((err = glGetError()) != GL_NO_ERROR) {
           log_this(10, "Problem1\n");
           fprintf(stderr,"opengl error:%d\n", err);
       }*/
//    glUniform1fv(uniform_bbox, 4, bbox);
    glUniformMatrix4fv(std_matrix, 1, GL_FALSE,theMatrix );

    /*
        while ((err = glGetError()) != GL_NO_ERROR) {
            log_this(10, "Problem3: %d\n", err);
            fprintf(stderr,"opengl error:%d\n", err);
        }*/
    if(oneLayer->show_text && oneLayer->text->used_n_vals!=rb->used_n_pa)
        printf("There is a mismatch between number of labels and number of corresponding points\n");

    for (i=0; i<rb->used_n_pa; i++)
    {
        total_points += *(rb->npoints+i);
        Uint32 styleID = *(rb->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            color = global_styles[styleID].color;
        }
        else
        {
            c[0] = c[1] = c[2] = 100;
            c[3] = 255;
            color = c;
        }
        glUniform4fv(std_color,1,color );
        glDrawArrays(GL_POINTS, *(rb->start_index+i), *(rb->npoints+i));

    }
    glDisableVertexAttribArray(std_coord2d
                              );

    return 0;

}



int loadLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{


    GLESSTRUCT *rb = oneLayer->res_buf;

    glGenBuffers(1, &(oneLayer->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->first_free-rb->vertex_array), rb->vertex_array, GL_STATIC_DRAW);

    if(oneLayer->line_width)
        renderLineTri(oneLayer,theMatrix);
    else
        renderLine( oneLayer, theMatrix, 0);
    return 0;
}



int renderLineTri(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
//void render_tri(SDL_Window* window, OUTBUFFER *linje, GLuint vb)
{

    uint32_t  i;
    GLfloat *color, lw;
    GLfloat c[4];
    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat) (2.0 / CURR_HEIGHT);

    GLfloat px_Matrix[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};

    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;





    glUseProgram(lw_program);

    glEnableVertexAttribArray(lw_coord2d);

    glEnableVertexAttribArray(lw_norm);

    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error 1 :%d\n", err);
    }

    glVertexAttribPointer(
        lw_coord2d, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        4 * sizeof(GLfloat),                 // no extra data between each position
        0                  // offset of first element
    );


    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error 2 :%d\n", err);
    }

    glVertexAttribPointer(
        lw_norm, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        4 * sizeof(GLfloat),                 // no extra data between each position
        (GLvoid*) (2 * sizeof(GLfloat))                  // offset of first element
    );


    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error 3 :%d\n", err);
    }

    glUniformMatrix4fv(lw_matrix, 1, GL_FALSE,theMatrix );
    glUniformMatrix4fv(lw_px_matrix, 1, GL_FALSE,px_Matrix );



    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error 4 :%d\n", err);
    }

    for (i=0; i<rb->used_n_pa; i++)
    {


        Uint32 styleID = *(rb->styleID+i);



        total_points += *(rb->npoints+i);



        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            lw = (GLfloat) (global_styles[styleID].lineWidth * 0.5);
            if(!lw)
                lw = 0.5;
            color = global_styles[styleID].color;

        }
        else
        {
            c[0] = c[1] = c[2] = 100;
            c[3] = 255;
            color = c;
        }


        glUniform4fv(lw_color,1,color );



        glUniform1fv(lw_linewidth,1,&lw );



        glDrawArrays(GL_TRIANGLE_STRIP, *(rb->start_index+i), *(rb->npoints+i));


        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



    }
    glDisableVertexAttribArray(lw_norm);

    glDisableVertexAttribArray(lw_coord2d);

    return 0;

}


int renderLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix, int outline)
{



    log_this(10, "Entering renderLine\n");
    uint32_t i;//, np, pi;
    GLfloat *color, lw;
    GLfloat c[4];
//   GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;




    glUseProgram(std_program);
    glEnableVertexAttribArray(std_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
        std_coord2d, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
    /*   while ((err = glGetError()) != GL_NO_ERROR) {
           log_this(10, "Problem1\n");
           fprintf(stderr,"opengl error:%d", err);
       }*/
//    glUniform1fv(uniform_bbox, 4, bbox);
    glUniformMatrix4fv(std_matrix, 1, GL_FALSE,theMatrix );
    /*   while ((err = glGetError()) != GL_NO_ERROR) {
           log_this(10, "Problem 2\n");
           fprintf(stderr,"opengl error:%d", err);
       }*/



    n_lines += rb->used_n_pa;
    for (i=0; i<rb->used_n_pa; i++)
    {
        lw = 1;


        Uint32 styleID = *(rb->styleID+i);



        total_points += *(rb->npoints+i);



        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            lw = global_styles[styleID].lineWidth;
            if(!lw)
                continue;

            if(outline)
            {
                color = global_styles[styleID].outlinecolor;
                glUniform4fv(std_color,1,color );
                glDrawArrays(GL_LINE_LOOP, *(rb->start_index+i), *(rb->npoints+i));
            }
            else
            {
                color = global_styles[styleID].color;
                glUniform4fv(std_color,1,color );
                glDrawArrays(GL_LINE_STRIP, *(rb->start_index+i), *(rb->npoints+i));
            }

        }
        else
        {
            c[0] = c[1] = c[2] = 100;
            c[3] = 255;
            color = c;

            if (outline)
            {
                glUniform4fv(std_color, 1, color);
                glDrawArrays(GL_LINE_LOOP, *(rb->start_index + i), *(rb->npoints + i));
            }
            else
            {
                glUniform4fv(std_color, 1, color);
                glDrawArrays(GL_LINE_STRIP, *(rb->start_index + i), *(rb->npoints + i));
            }

        }








    }

    glDisableVertexAttribArray(std_coord2d);

    return 0;

}




int loadPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{

    GLESSTRUCT *rb = oneLayer->res_buf;
//	 int i,j, offset=0;
    glGenBuffers(1, &(oneLayer->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->first_free-rb->vertex_array), rb->vertex_array, GL_STATIC_DRAW);



    ELEMENTSTRUCT *ti = oneLayer->tri_index;

    //   int size =  sizeof(GLshort)*(ti->first_free-ti->vertex_array);
    glGenBuffers(1, &(oneLayer->ebo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oneLayer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort)*(ti->first_free-ti->index_array), ti->index_array, GL_STATIC_DRAW);



    if(oneLayer->render_area)
        renderPolygon( oneLayer, theMatrix);
    renderLine(oneLayer, theMatrix,1);
    return 0;
}



int renderPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    log_this(10, "Entering renderPolygon\n");
    uint32_t i;//, np, pi;
    GLfloat *color;
    GLfloat c[4];
//    GLenum err;



    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);


    GLESSTRUCT *rb = oneLayer->res_buf;
    ELEMENTSTRUCT *ti = oneLayer->tri_index;



    glUseProgram(std_program);
    glEnableVertexAttribArray(std_coord2d);

    n_polys += ti->used_n_pa;

    for (i=0; i<ti->used_n_pa; i++)
    {
        size_t  vertex_offset = *(rb->polygon_offset+i)  ;
        glVertexAttribPointer(
            std_coord2d, // attribute
            2,                 // number of elements per vertex, here (x,y)
            GL_FLOAT,          // the type of each element
            GL_FALSE,          // take our values as-is
            0,                 // no extra data between each position
            (GLvoid*) vertex_offset                  // offset of first element
        );
        /*    while ((err = glGetError()) != GL_NO_ERROR) {
                log_this(10, "Problem1\n");
                fprintf(stderr,"opengl error:%d", err);
            }*/


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oneLayer->ebo);

        glUniformMatrix4fv(std_matrix, 1, GL_FALSE,theMatrix );
        /*  while ((err = glGetError()) != GL_NO_ERROR) {
              log_this(10, "Problem 2\n");
              fprintf(stderr,"opengl error:%d", err);
          }*/

        Uint32 styleID = *(ti->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            color = global_styles[styleID].color;
        }
        else
        {
            c[0] = c[1] = c[2] = 100;
            c[3] = 255;
            color = c;
        }

        glUniform4fv(std_color,1,color );



        size_t index_offset = *(ti->start_index+i) * 3 *sizeof(GLushort) ;
        //  offset = i * 100;
        //int n = *(ti->npoints+i) * 3 ;
        n_tri += *(ti->npoints+i);
        glDrawElements(GL_TRIANGLES, *(ti->npoints+i) * 3,GL_UNSIGNED_SHORT,(GLvoid*) index_offset);

    }
    glDisableVertexAttribArray(std_coord2d);

    return 0;

}


int render_data(SDL_Window* window,GLfloat *theMatrix)
{
    log_this(10, "Entering render_data\n");
    int i;
    LAYER_RUNTIME *oneLayer;


    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    total_points=0;
    for (i=0; i<nLayers; i++)
    {
        oneLayer = layerRuntime + i;


        if(oneLayer->visible)
        {

//~ log_this(10, "render : %d\n",oneLayer->geometryType);
            switch(oneLayer->geometryType) {
            case POINTTYPE :
                //     log_this(10, "render point");
                if (oneLayer->show_text) {
                    render_text(oneLayer, theMatrix);
                } else {
                    renderPoint(oneLayer, theMatrix);
                }
                break;
            case LINETYPE :
                if(oneLayer->line_width)
                    renderLineTri(oneLayer,theMatrix);
                else
                    renderLine( oneLayer, theMatrix, 0);
                //renderLine( oneLayer, theMatrix, 0);
                break;
            case POLYGONTYPE :
                renderPolygon( oneLayer, theMatrix);
                renderLine(oneLayer, theMatrix,1);
                break;
            }
        }


    }

    SDL_GL_SwapWindow(window);

    //  pthread_mutex_destroy(&mutex);
//render(window,res_buf);
    return 0;
}





/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
int  render_text(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{

    log_this(10, "Entering renderText\n");
    uint32_t i;
    GLfloat *color;
    GLfloat c[4];
    int ndims=2;
    //FT_GlyphSlot g = face->glyph;
//   GLuint text_vbo;
    char *txt;
//   glGenBuffers(1, &text_vbo);
    /* Create a texture that will be used to hold one "glyph" */

    glGenBuffers(1, &text_vbo);
    GLfloat point_coord[2];

    GLESSTRUCT *rb = oneLayer->res_buf;
    glUseProgram(txt_program);

    glUniformMatrix4fv(txt_matrix, 1, GL_FALSE,theMatrix );


    GLfloat sx = (GLfloat)(2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat)(2.0 / CURR_HEIGHT);


    if(oneLayer->show_text && oneLayer->text->used_n_vals!=rb->used_n_pa)
        printf("There is a mismatch between number of labels and number of corresponding points\n");
    int used=0;

    n_words += rb->used_n_pa;
    for (i=0; i<rb->used_n_pa; i++)
    {

        total_points += 1;
        int psz =  *(oneLayer->text->size+i);

//log_this(10, "psz = %d \n",psz);

        Uint32 styleID = *(rb->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
            color = global_styles[styleID].color;
        else
        {
            c[0] = c[1] = c[2] = 100;
            c[3] = 255;
            color = c;
        }

        point_coord[0] =  *(rb->vertex_array+ *(rb->start_index+i)*ndims);
        point_coord[1] =  *(rb->vertex_array+ *(rb->start_index+i)*ndims + 1);
        txt = oneLayer->text->char_array+used;
        used+=strlen(txt)+1;
        draw_it(color,point_coord, psz, txt_box, txt_color, txt_coord2d, txt, sx, sy);





    }


    return 0;
}



int draw_it(GLfloat *color,GLfloat *point_coord, int atlas_nr,GLint txt_box,GLint txt_color,GLint txt_coord2d,char *txt, float sx, float sy)
{


    ATLAS *a = atlases[atlas_nr-1];
    const char *u;
    GLfloat x,y;
    uint32_t p;

    glBindTexture(GL_TEXTURE_2D, a->tex);
    //    glUniform1i(text_uniform_tex, 0);
    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(txt_box);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
    glVertexAttribPointer(txt_box, 4, GL_FLOAT, GL_FALSE, 0, 0);


    glUniform4fv(txt_color,1,color);

    glUniform2fv(txt_coord2d,1,point_coord);



    POINT_T coords[600];
    int c = 0;


    x = 0;
    y = 0;
    /* Loop through all characters */
    u = txt;
    n_letters += strlen(txt);
    while(*u) {

        p = utf82unicode(u,&u);
        /* Calculate the vertex and texture coordinates */
        float x2 = x + a->metrics[p].bl * sx;
        float y2 = -y - a->metrics[p].bt * sy;
        float w = a->metrics[p].bw * sx;
        float h = a->metrics[p].bh * sy;

        /* Advance the cursor to the start of the next character */
        x += a->metrics[p].ax * sx;
        y += a->metrics[p].ay * sy;

        /* Skip glyphs that have no pixels */
        if (!w || !h)
            continue;

        coords[c++] = (POINT_T) {
            x2, -y2, a->metrics[p].tx, a->metrics[p].ty
        };
        coords[c++] = (POINT_T) {
            x2 + w, -y2, a->metrics[p].tx + a->metrics[p].bw / a->w, a->metrics[p].ty
        };
        coords[c++] = (POINT_T) {
            x2, -y2 - h, a->metrics[p].tx, a->metrics[p].ty + a->metrics[p].bh / a->h
        };
        coords[c++] = (POINT_T) {
            x2 + w, -y2, a->metrics[p].tx + a->metrics[p].bw / a->w, a->metrics[p].ty
        };
        coords[c++] = (POINT_T) {
            x2, -y2 - h, a->metrics[p].tx, a->metrics[p].ty + a->metrics[p].bh / a->h
        };
        coords[c++] = (POINT_T) {
            x2 + w, -y2 - h, a->metrics[p].tx + a->metrics[p].bw / a->w, a->metrics[p].ty + a->metrics[p].bh / a->h
        };
    }

    /* Draw all the character on the screen in one go */
    glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, c);

    glDisableVertexAttribArray(txt_box);

    return 0;
}
