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
#include "interface/interface.h"
#include "mem.h"
#include "SDL_image.h"
#include "uthash.h"
#include "utils.h"

int loadPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{


    if(oneLayer->type & 32)
    {
        //render_text(oneLayer,theMatrix);
        load_text(oneLayer);
    render_text(oneLayer,theMatrix);
        return 0;
    }
    /*  POINT_LIST *rb = oneLayer->points;

           glBindBuffer(GL_ARRAY_BUFFER, rb->vbo);
           glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->points->used), rb->points->list, GL_STATIC_DRAW);
    */
    renderPoint( oneLayer, theMatrix);
    return 0;
}

int renderPoint(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{


    unsigned int symbol = 2, last_symbol = 0, i;

    POINT_LIST *points = oneLayer->points;

    GLfloat radius;
    GLfloat *color;
    GLfloat *p;


    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat) (2.0 / CURR_HEIGHT);

    GLfloat px_Matrix[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};


    glBindBuffer(GL_ARRAY_BUFFER, global_symbols->points->vbo);

    glUseProgram(sym_program);


    glEnableVertexAttribArray(sym_norm);


    glUniformMatrix4fv(sym_matrix, 1, GL_FALSE,theMatrix );


    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,16);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);


    p = points->points->list;

    int sym_npoints;
    for (i=0; i<points->point_start_indexes->used; i++)
    {



        struct STYLES *styles = (struct STYLES *) *((struct STYLES **)points->style_id->list +i);
        if(!styles)
            styles=system_default_style;
        POINT_STYLE *style = styles->point_styles;



        //  printf("start checking %p\n", style);
        //  printf("style->nsyms=%d\n",style->nsyms);
        if(!style)
            continue;
        int r;
        for (r = 0; r<style->nsyms; r++)
        {

            // printf("r=%d\n",r);

            symbol = *(style->symbol->list + r);
            color = style->color->list + 4*r;
            radius = *(style->size->list + r);


            if(symbol != last_symbol)
            {
                last_symbol = symbol;
                glVertexAttribPointer(
                    sym_norm, // attribute
                    2,                 // number of elements per vertex, here (x,y)
                    GL_FLOAT,          // the type of each element
                    GL_FALSE,          // take our values as-is
                    0,                 // no extra data between each position
                    (GLvoid*) (sizeof(GLfloat) *  *(global_symbols->points->point_start_indexes->list + symbol - 1) )               // offset of first element
                );

                log_this(10, "%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f",theMatrix[0],theMatrix[1],theMatrix[2],theMatrix[3],theMatrix[4],theMatrix[5],theMatrix[6],theMatrix[7],theMatrix[8],theMatrix[9],theMatrix[10],theMatrix[11],theMatrix[12],theMatrix[13],theMatrix[14],theMatrix[15]);





                if (symbol == 0)
                    sym_npoints = global_symbols->points->point_start_indexes->list[0];
                else
                    sym_npoints = global_symbols->points->point_start_indexes->list[symbol] - global_symbols->points->point_start_indexes->list[symbol -1];

            }




            //    printf("used points = %d, startindex = %d, used vals\n",points->point_start_indexes->used, points->point_start_indexes->list[i], points->points->used);
            GLfloat z = style->z->list[r] - 0.001*r;
            
            if(!z)
                z = 0;

            glUniform1fv(sym_z,1,&z );
            //   printf("p: %f, %f\n", *(p), *(p+1));
            glUniform2fv(sym_coord2d,1,p);

            int unit = style->units->list[r];
            if(unit == PIXEL_UNIT)
                glUniformMatrix4fv(sym_px_matrix, 1, GL_FALSE,px_Matrix );
            else
                glUniformMatrix4fv(sym_px_matrix, 1, GL_FALSE,theMatrix );

            // glUniformMatrix4fv(sym_px_matrix, 1, GL_FALSE,theMatrix );
            glUniform4fv(sym_color,1,color );
            glUniform1fv(sym_radius,1,&radius );
            glDrawArrays(GL_TRIANGLE_FAN, 0, sym_npoints/2);

        }
        p = points->points->list + points->point_start_indexes->list[i];
    }
    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(100, "gl problem\n");
        fprintf(stderr,"opengl problem :%d\n", err);
    }

    glDisableVertexAttribArray(sym_norm);

    glDisable (GL_DEPTH_TEST);
    glUseProgram(0);
    
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}

    return 0;

}



int loadLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    /*
       printf(".............................................................\n");
    for (i=0;i<oneLayer->wide_lines->vertex_array->used; i++)
    {
    printf("load, i = %d, v = %f\n", i,*(oneLayer->wide_lines->vertex_array->list+i));
    }*/
    if(oneLayer->geometryType == RASTER)
        return 0;
    if(oneLayer->type & 8)
    {
        LINESTRING_LIST *line = oneLayer->wide_lines;
        glBindBuffer(GL_ARRAY_BUFFER, line->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*line->vertex_array->used,line->vertex_array->list, GL_STATIC_DRAW);
        renderLineTri(oneLayer,theMatrix);
    }
    else
    {
        LINESTRING_LIST *line = oneLayer->lines;
        //	 int i,j, offset=0;
        glBindBuffer(GL_ARRAY_BUFFER, line->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*line->vertex_array->used,line->vertex_array->list, GL_STATIC_DRAW);
        renderLine( oneLayer, theMatrix);
    }
    return 0;
}



int renderLineTri(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
//void render_tri(SDL_Window* window, OUTBUFFER *linje, GLuint vb)
{
    if(oneLayer->geometryType == RASTER)
        return 0;

    
    
    while ((err = glGetError()) != GL_NO_ERROR) 
    {
        fprintf(stderr,"0 - opengl error:%d before func %s layer %s\n", err, __func__,oneLayer->name);
    }


    LINESTRING_LIST *line = oneLayer->wide_lines;
    uint32_t  i;
    GLfloat *color,z, lw=0;
    unsigned int n_vals = 0, n_vals_acc = 0;
    uint8_t ndims = oneLayer->n_dims;
    uint8_t vals_per_point = ndims*2;
    
    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat) (2.0 / CURR_HEIGHT);

    GLfloat px_Matrix[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};
    GLint unit = -1;
    glBindBuffer(GL_ARRAY_BUFFER, line->vbo);

    glUseProgram(lw_program);

    glEnableVertexAttribArray(lw_coord2d);

    glEnableVertexAttribArray(lw_norm);


    glVertexAttribPointer(
        lw_coord2d, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        4 * sizeof(GLfloat),                 // no extra data between each position
        0                  // offset of first element
    );


    glVertexAttribPointer(
        lw_norm, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        4 * sizeof(GLfloat),                 // no extra data between each position
        (GLvoid*) (2 * sizeof(GLfloat))                  // offset of first element
    );

    log_this(10, "%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f",theMatrix[0],theMatrix[1],theMatrix[2],theMatrix[3],theMatrix[4],theMatrix[5],theMatrix[6],theMatrix[7],theMatrix[8],theMatrix[9],theMatrix[10],theMatrix[11],theMatrix[12],theMatrix[13],theMatrix[14],theMatrix[15]);
    glUniformMatrix4fv(lw_matrix, 1, GL_FALSE,theMatrix );

    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,16);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
//    n_lines += line->line_start_indexes->used;
    //  total_points += line->vertex_array->used/ndims;


    /*    printf("------------------------------------------------------\n");
    for (i=0;i<line->vertex_array->used; i++)
    {
     printf("load, i = %d, v = %f\n", i,*(line->vertex_array->list+i));
    }*/



        while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"2 - opengl error:%d in func %s layer %s\n", err, __func__,oneLayer->name);
}


    for (i=0; i<line->line_start_indexes->used; i++)
    {

    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s, i = %d\n", err, __func__,i);
}
        //    Uint32 styleID = *(rb->styleID+i);

        struct STYLES *styles = (struct STYLES *) *((struct STYLES **)line->style_id->list +i);
        if(!styles)
        {
            styles=system_default_style;
        log_this(100, "yes use system default\n");
        }
        LINE_STYLE *style = styles->line_styles;


        n_vals = *(line->line_start_indexes->list + i)/vals_per_point - n_vals_acc;

        if(!style)
            continue;
        int r;
        for (r = 0; r<style->nsyms; r++)
        {
            lw = style->width->list[r];


            z = style->z->list[r] - 0.001*r;
            if(!z)
                z = 0;
            
            unit = style->units->list[r];
            if(unit == PIXEL_UNIT)
                glUniformMatrix4fv(lw_px_matrix, 1, GL_FALSE,px_Matrix );
            else
                glUniformMatrix4fv(lw_px_matrix, 1, GL_FALSE,theMatrix );

            color = style->color->list + 4*r;

            //  printf("color = %f, %f, %f, %f, z=%f, unit=%d, width=%f\n",color[0],color[1], color[2],color[3], z, unit, lw);


            glUniform1fv(lw_z,1,&z );
            glUniform4fv(lw_color,1,color );
            glUniform1fv(lw_linewidth,1,&lw );
            glDrawArrays(GL_TRIANGLE_STRIP, n_vals_acc, n_vals);
            while ((err = glGetError()) != GL_NO_ERROR) {
                log_this(10, "Problem1\n");
                fprintf(stderr,"opengl error:%d in %s\n", err, __func__);
            }
        }

//printf("i = %d, startindex = %d, npoints = %d\n", i, n_vals_acc, n_vals);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


        n_vals_acc = *(line->line_start_indexes->list + i)/vals_per_point;

    }
        while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"1 - opengl error:%d in func %s layer %s\n", err, __func__,oneLayer->name);
}
    glDisableVertexAttribArray(lw_norm);

    glDisableVertexAttribArray(lw_coord2d);
    glDisable (GL_DEPTH_TEST);
    
    glUseProgram(0);
    
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s layer %s\n", err, __func__,oneLayer->name);
}

    return 0;

}


int renderLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    log_this(10, "Entering renderLine\n");
    uint32_t i;//, np, pi;
    GLfloat *color;
    LINESTRING_LIST *line = oneLayer->lines;

    unsigned int n_vals = 0, n_vals_acc = 0;
    uint8_t ndims = oneLayer->n_dims;
    glBindBuffer(GL_ARRAY_BUFFER, line->vbo);



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

    glUniformMatrix4fv(std_matrix, 1, GL_FALSE,theMatrix );

    unsigned int used_n_pa = line->line_start_indexes->used;

    n_lines += used_n_pa;
    total_points += line->vertex_array->used/ndims;



    for (i=0; i<used_n_pa; i++)
    {


        struct STYLES *styles = (struct STYLES *) *((struct STYLES **)line->style_id->list +i);
        if(!styles)
            styles=system_default_style;
        LINE_STYLE *style = styles->line_styles;

        n_vals = *(line->line_start_indexes->list + i)/ndims - n_vals_acc;

        if(!style)
            continue;


        int r;
        for (r = 0; r<style->nsyms; r++)
        {
            color = style->color->list + 4*r;


            glUniform4fv(std_color,1,color );


            glDrawArrays(GL_LINE_STRIP, n_vals_acc, n_vals);


        }
        n_vals_acc = *(line->line_start_indexes->list + i)/ndims;

    }
    glDisableVertexAttribArray(std_coord2d);

    glUseProgram(0);
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}
    return 0;

}




int loadPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{

    POLYGON_LIST *poly = oneLayer->polygons;


    glBindBuffer(GL_ARRAY_BUFFER, poly->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*poly->vertex_array->used,poly->vertex_array->list, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, poly->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort)*poly->element_array->used, poly->element_array->list, GL_STATIC_DRAW);

    if(oneLayer->type & 4)
        renderPolygon( oneLayer, theMatrix);
    return 0;
}



int renderPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    log_this(10, "Entering renderPolygon\n");
    uint32_t i;//, np, pi;
    GLfloat *color;
    uint8_t ndims = oneLayer->n_dims;
    size_t index_offset = 0;
    POLYGON_LIST *poly = oneLayer->polygons;

    glBindBuffer(GL_ARRAY_BUFFER, poly->vbo);
    unsigned int n_vals = 0, n_vals_acc = 0;

    unsigned int used_n_pa;
    unsigned int used_n_poly;



    if(oneLayer->type & 4)
    {
        POLYGON_STYLE *style = NULL;
        used_n_poly = poly->polygon_start_indexes->used;

        glUseProgram(std_program);
        glEnableVertexAttribArray(std_coord2d);

        n_polys += used_n_poly;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, poly->ebo);
        glUniformMatrix4fv(std_matrix, 1, GL_FALSE,theMatrix );

        // n_polys += poly->pa_start_indexes->used;
        //total_points += poly->vertex_array->used/ndims;
        //n_tri += poly->element_array->used/3;


        for (i=0; i<used_n_poly; i++)
        {
            size_t  vertex_offset = sizeof(GLfloat) * *(poly->polygon_start_indexes->list + i);

            // printf("pa_list_index = %d, vertex_list_index_0 = %d\n", poly->polygon_start_indexes->list[i], poly->pa_start_indexes->list[0]);
            // size_t  vertex_offset = sizeof(GLuint) * poly->pa_start_indexes->list[poly->polygon_start_indexes->list[i]];

            //printf("vertex_offfset = %d\n", vertex_offset);
            glVertexAttribPointer(
                std_coord2d, // attribute
                2,                 // number of elements per vertex, here (x,y)
                GL_FLOAT,          // the type of each element
                GL_FALSE,          // take our values as-is
                0,                 // no extra data between each position
                (GLvoid*) vertex_offset                  // offset of first element
            );


            struct STYLES *styles = (struct STYLES *) *((struct STYLES **)poly->style_id->list +i);
            if(!styles)
                styles=system_default_style;
            style = styles->polygon_styles;



            n_vals = *(poly->element_start_indexes->list + i) - n_vals_acc;
            n_vals_acc = *(poly->element_start_indexes->list + i);

            if(!style)
                continue;
            int r;
            for (r = 0; r<style->nsyms; r++)
            {


                color = style->color->list + 4*r;
                glUniform4fv(std_color,1,color );

                //    printf("color = %f, %f, %f,  %f\n", color[0], color[1], color[2], color[3]);
                glDrawElements(GL_TRIANGLES, n_vals,GL_UNSIGNED_SHORT,(GLvoid*) index_offset);
            }

            index_offset = sizeof(GLushort) * *(poly->element_start_indexes->list + i);

        }
        n_vals = n_vals_acc = 0;
        glDisableVertexAttribArray(std_coord2d);
    }
    if(!(oneLayer->type & 8))
    {
        LINE_STYLE *style = NULL;
        glBindBuffer(GL_ARRAY_BUFFER, oneLayer->polygons->vbo);




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

        glUniformMatrix4fv(std_matrix, 1, GL_FALSE,theMatrix );

        used_n_pa = poly->pa_start_indexes->used;

        for (i=0; i<used_n_pa; i++)
        {


            n_vals = *(poly->pa_start_indexes->list + i)/ndims - n_vals_acc;


            struct STYLES *styles = (struct STYLES *) *((struct STYLES **)poly->line_style_id->list +i);
            if(!styles)
                styles=system_default_style;
            style = styles->line_styles;



            if(!style)
                continue;
            int r;
            for (r = 0; r<style->nsyms; r++)
            {
                color = style->color->list + 4*r;
                GLfloat width = *(style->width->list + r);
                //  printf("width = %f\n",width);
                if(width>0)
                {
                    glUniform4fv(std_color,1,color );


                    glDrawArrays(GL_LINE_LOOP, n_vals_acc, n_vals);
                }
            }
            n_vals_acc = *(poly->pa_start_indexes->list + i)/ndims;

        }
        glDisableVertexAttribArray(std_coord2d);
    }
    
    glUseProgram(0);
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}
    return 0;

}




static int render_data_layers(GLfloat *theMatrix, CTRL *controls)
{
    log_this(10, "Entering render_data\n");
    int i;
    LAYER_RUNTIME *oneLayer;


    total_points=0;
    for (i=0; i<global_layers->nlayers; i++)
    {
        oneLayer = global_layers->layers + i;

        /* if(oneLayer->geometryType >= RASTER)
             continue;*/
        int type = oneLayer->type;
//printf("render layer %s\n",oneLayer->name);
        if(oneLayer->visible)
        {

//~ log_this(10, "render : %d\n",oneLayer->geometryType);
            if(oneLayer->geometryType >= RASTER)
                loadandRenderRaster( oneLayer, theMatrix);
            //     log_this(10, "render point");
            if (type & 32)
                render_text(oneLayer, theMatrix);

            if(type & (224-32))
                renderPoint(oneLayer, theMatrix);

            if(type & 8)
                renderLineTri(oneLayer,theMatrix);
            if (type & 16)
                renderLine( oneLayer, theMatrix);
            if(type & 4)
                renderPolygon( oneLayer, theMatrix);
            //renderLine(oneLayer, theMatrix,1);


        }


    }
    renderGPS(theMatrix);

    //render_simple_rect(5,75,300,225);

    render_controls(controls, NULL);
    //  pthread_mutex_destroy(&mutex);
//render(window,res_buf);
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}
    return 0;
}



int render_data(SDL_Window* window,GLfloat *theMatrix, CTRL *controls)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    render_data_layers(theMatrix, controls);

    if(infoRenderLayer->visible)
    {
        loadPolygon(infoRenderLayer, theMatrix);
    }
    SDL_GL_SwapWindow(window);

    return 0;
}
/*
int render_info(SDL_Window* window,GLfloat *theMatrix)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    render_data_layers(theMatrix);

    loadPolygon(infoRenderLayer, theMatrix);

    SDL_GL_SwapWindow(window);
    return 0;
}
*/

int load_text(LAYER_RUNTIME *oneLayer)
{
    
    

    TEXTBLOCK *tb = oneLayer->text->tb;
    tb->txt_info->points = oneLayer->points;
    
    glBindBuffer(GL_ARRAY_BUFFER, tb->txt_info->points->tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*(tb->dims->coords->used), tb->dims->coords->coords, GL_STATIC_DRAW);       
    
     while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error wt:%d\n", err);
    }
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}
    return 0;
}



/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
int  render_text(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    

    log_this(10, "Entering %s with layer %s\n", __func__, oneLayer->name);
    int npoints = 0;
    if(!oneLayer->text->tb || !oneLayer->text->tb->txt_info->ntexts)
        return 0;
int tot_points = 0;
float delta[2];
//    if(oneLayer->show_text && oneLayer->text->used_n_vals!=rb->used_n_pa)
    if(oneLayer->type & 32 && oneLayer->points->point_start_indexes->used!=oneLayer->text->tb->txt_info->ntexts)
    {
        log_this(100,"There is a mismatch between number of labels and number of corresponding points\n");
        return 1;
    }

    TEXTBLOCK *tb = oneLayer->text->tb;
    
    POINT_LIST *point = oneLayer->points;

    GLfloat sx = (GLfloat)(2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat)(2.0 / CURR_HEIGHT);

    GLfloat pxMatrix[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};

    uint32_t i, j;//, np, pi;
    GLfloat *color, *startp;
    
    TXT_INFO *ti = tb->txt_info;

    
    glUseProgram(txt2_program);
    glUniformMatrix4fv(txt2_matrix, 1, GL_FALSE,theMatrix );
    
    glUniformMatrix4fv(txt2_px_matrix, 1, GL_FALSE,pxMatrix );
    
    glBindBuffer(GL_ARRAY_BUFFER, point->tbo);
    
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"4 - opengl error:%d in func %s\n", err, __func__);
}
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"3 - opengl error:%d in func %s\n", err, __func__);
}
    glEnableVertexAttribArray(txt2_box);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    
    
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"2 - opengl error:%d in func %s\n", err, __func__);
}
    glVertexAttribPointer(
        txt2_box,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
    );

    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"1 - opengl error:%d in func %s\n", err, __func__);
}
        int r = 0;
    for (i=0; i<ti->ntexts; i++)
    {
        struct STYLES *styles = (struct STYLES *) *((struct STYLES **)point->style_id->list +i);
        if(!styles)
            styles=system_default_style;
        TEXT_STYLE *style = styles->text_styles;

        if(!style)
            return 1;
        
    uint8_t ndims = oneLayer->n_dims;
        startp = point->points->list + point->point_start_indexes->list[i]-ndims;
        glUniform2fv(txt2_coord2d,1,startp);
        
        float w = tb->dims->widths->list[i];
        float h = tb->dims->heights->list[i];
        float *anchor = style->anchorpoint->list;
        
            delta[0] = -w*anchor[0];
            delta[1] = h*anchor[1];
        
        
      //  delta[1] = 0;
        glUniform2fv(txt2_delta,1,delta);
        for (j=ti->formating_index->list[i]; j<ti->formating_index->list[i+1]; j++)
        {
            //int format_index = ti->formating_index->list[j];
            ATLAS *a = tb->formating->font->list[j];
            
            glBindTexture(GL_TEXTURE_2D, a->tex);
            
            //int n_points = 6 * ((char*) (tb->formating->txt_index->list[j+1]) - ((char*) tb->formating->txt_index->list[j]));
            
            npoints = tb->dims->coord_index->list[j+1]-tb->dims->coord_index->list[j];
            
            color = style->color->list + 4*r;

            
         //   log_this(10,"txt = %s, tex=%d,npoints=%d,  color = %f, %f, %f, %f\n",tb->txt->txt + tb->formating->txt_index->list[j], a->tex,npoints, color[0], color[1], color[2], color[3]);

                glUniform4fv(txt2_color,1,color );
                
                glDrawArrays(GL_TRIANGLES, tot_points,npoints);       
       //           glDrawArrays(GL_TRIANGLE_STRIP, tot_points,n_points);       
                tot_points+=npoints;
                
        }
    }
    glDisableVertexAttribArray(txt2_box);

    glUseProgram(0);

    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}
    return 0;
}




int draw_txt(TEXTBLOCK *tb,GLfloat *theMatrix,GLfloat *pxMatrix, float *anchor, float *displacement)
{
    
    log_this(10, "Entering renderLine\n");
    uint32_t i, j;//, np, pi;
    GLfloat *color, *startp;
    
    TXT_INFO *ti = tb->txt_info;
    POINT_LIST *point = ti->points;

    
    glUniformMatrix4fv(txt2_matrix, 1, GL_FALSE,theMatrix );
    
    glUniformMatrix4fv(txt2_px_matrix, 1, GL_FALSE,pxMatrix );
    
    glBindBuffer(GL_ARRAY_BUFFER, point->tbo);
    
    glUseProgram(txt2_program);
    glEnableVertexAttribArray(txt2_box);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    
    
    glVertexAttribPointer(
        txt2_box,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
    );
    
    
int tot_points = 0;
float delta[2];

    for (i=0; i<ti->ntexts; i++)
    {
        startp = point->points->list + point->point_start_indexes->list[i];
        glUniform2fv(txt2_coord2d,1,startp);
        
        float w = tb->dims->widths->list[i];
        float h = tb->dims->heights->list[i];
   
        
            delta[0] = -w*anchor[0];
            delta[1] = h*anchor[1];
        
        
      //  delta[1] = 0;
        glUniform2fv(txt2_delta,1,delta);
        for (j=ti->formating_index->list[i]; j<ti->formating_index->list[i+1]; j++)
        {
            //int format_index = ti->formating_index->list[j];
            ATLAS *a = tb->formating->font->list[j];
            
            glBindTexture(GL_TEXTURE_2D, a->tex);
            color = tb->formating->color->list + j*4;
            
            //int n_points = 6 * ((char*) (tb->formating->txt_index->list[j+1]) - ((char*) tb->formating->txt_index->list[j]));
            n_points = tb->dims->coord_index->list[j+1]-tb->dims->coord_index->list[j];
            
            
        //    printf("txt = %s, tex=%d,npoints=%d,  color = %f, %f, %f, %f\n",tb->txt->txt + tb->formating->txt_index->list[j], a->tex,n_points, color[0], color[1], color[2], color[3]);

                glUniform4fv(txt2_color,1,color );
                
                glDrawArrays(GL_TRIANGLES, tot_points,n_points);       
       //           glDrawArrays(GL_TRIANGLE_STRIP, tot_points,n_points);       
                tot_points+=n_points;
        }
    }
    glDisableVertexAttribArray(txt2_box);

    glUseProgram(0);
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}
    return 0;
    
}

int loadSymbols()
{

    glGenBuffers(1, &(global_symbols->points->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, global_symbols->points->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*global_symbols->points->points->used, global_symbols->points->points->list, GL_STATIC_DRAW);
    return 0;
}


int loadGPS(GLfloat *gps_circle)
{

    glGenBuffers(1, &gps_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gps_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*((gps_npoints+2) * 2), gps_circle, GL_STATIC_DRAW);
    return 0;
}





int renderGPS(GLfloat *theMatrix)
//void render_tri(SDL_Window* window, OUTBUFFER *linje, GLuint vb)
{

    log_this(10,"rendering gps with x=%f, y = %f",gps_point.x,gps_point.y);
    GLfloat radius1=0, radius2=0, radius3 = 0;
    GLfloat color1[4] = {0.5,0.5,0.5,0.2};
    GLfloat color2[4] = {0,0,0,1};
    GLfloat color3[4] = {0,1,0,1};
    GLfloat p[2];


    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat) (2.0 / CURR_HEIGHT);

    GLfloat px_Matrix[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};


    glBindBuffer(GL_ARRAY_BUFFER, gps_vbo);

    glUseProgram(gps_program);


    glEnableVertexAttribArray(gps_norm);


    glVertexAttribPointer(
        gps_norm, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                 // offset of first element
    );

 //   log_this(10, "%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f,%f, %f",theMatrix[0],theMatrix[1],theMatrix[2],theMatrix[3],theMatrix[4],theMatrix[5],theMatrix[6],theMatrix[7],theMatrix[8],theMatrix[9],theMatrix[10],theMatrix[11],theMatrix[12],theMatrix[13],theMatrix[14],theMatrix[15]);


    glUniformMatrix4fv(gps_matrix, 1, GL_FALSE,theMatrix );




    radius1 = gps_point.s;
    radius2 = 11;
    radius3 = 10;
    p[0] = gps_point.x;
    p[1] = gps_point.y;
    glUniform2fv(gps_coord2d,1,p);
    glUniformMatrix4fv(gps_px_matrix, 1, GL_FALSE,theMatrix );
    glUniform4fv(gps_color,1,color1 );
    glUniform1fv(gps_radius,1,&radius1 );
    glDrawArrays(GL_TRIANGLE_FAN, 0, (gps_npoints+2) * 2);

    glUniformMatrix4fv(gps_px_matrix, 1, GL_FALSE,px_Matrix );
    glUniform4fv(gps_color,1,color2 );
    glUniform1fv(gps_radius,1,&radius2 );
    glDrawArrays(GL_TRIANGLE_FAN, 0, (gps_npoints+2) * 2);

    glUniformMatrix4fv(gps_px_matrix, 1, GL_FALSE,px_Matrix );
    glUniform4fv(gps_color,1,color3 );
    glUniform1fv(gps_radius,1,&radius3 );
    glDrawArrays(GL_TRIANGLE_FAN, 0, (gps_npoints+2) * 2);



    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);




    glDisableVertexAttribArray(gps_norm);

    glUseProgram(0);
    return 0;

}


int loadandRenderRaster(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{

    GLuint i;
    unsigned int tot_index = 0;
    RASTER_LIST *rast = oneLayer->rast;
    LINESTRING_LIST *line = oneLayer->lines;
    size_t  vertex_offset = 0;
    GLuint vbo_cube_texcoords;
    GLfloat cube_texcoords[2*4] = {
        // front
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
    };
    vbo_cube_texcoords = rast->cvbo;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);

    GLuint ibo_cube_elements;
    GLushort cube_elements[] = {
        // front
        0,  1,  2,
        2,  3,  0
    };

    ibo_cube_elements = rast->cibo;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);




    glUseProgram(raster_program);

    glActiveTexture(GL_TEXTURE0);

    glUniformMatrix4fv(raster_matrix, 1, GL_FALSE,theMatrix );
    for (i=0; i<rast->raster_start_indexes->used; i++)
    {
        uint8_t *data = rast->data->list + tot_index;
        size_t data_len = rast->raster_start_indexes->list[i];
        SDL_Surface* res_texture =  IMG_Load_RW(SDL_RWFromMem(data, data_len), 1);

        if (res_texture == NULL)
        {
            continue;
        }
        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, // target
                     0,  // level, 0 = base, no minimap,
                     GL_RGB, // internalformat
                     res_texture->w,  // width
                     res_texture->h,  // height
                     0,  // border, always 0 in OpenGL ES
                     GL_RGB,  // format
                     GL_UNSIGNED_BYTE, // type
                     res_texture->pixels);
        SDL_FreeSurface(res_texture);




        GLfloat *d = line->vertex_array->list+vertex_offset;
        /*
        printf("vertex_offset = %zu of %zu\n", vertex_offset, line->vertex_array->used);
            int z;
            for (z=0;z<8;z++)
            {
             printf("val %d = %f and tile_size = %zu \n",z, d[z], data_len);
            }*/





        GLuint vbo_cube_vertices;
        
        vbo_cube_vertices = rast->vbo;
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, d, GL_STATIC_DRAW);







        glUniform1i(raster_texture, /*GL_TEXTURE*/0);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glEnableVertexAttribArray(raster_coord2d);
        // Describe our vertices array to OpenGL (it can't guess its format automatically)
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
        glVertexAttribPointer(
            raster_coord2d, // attribute
            2,                 // number of elements per vertex, here (x,y,z)
            GL_FLOAT,          // the type of each element
            GL_FALSE,          // take our values as-is
            0,                 // no extra data between each position
            0 //(GLvoid*) vertex_offset                  // offset of first element
        );




        // size_t  vertex_offset = 8*i*sizeof(GLfloat);
        glEnableVertexAttribArray(raster_texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
        glVertexAttribPointer(
            raster_texcoord, // attribute
            2,                  // number of elements per vertex, here (x,y)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
        );

        /* Push each element in buffer_vertices to the vertex shader */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
        int size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

        glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

        glDisableVertexAttribArray(raster_coord2d);
        glDisableVertexAttribArray(raster_texcoord);


        vertex_offset =  *(line->line_start_indexes->list + i);
        tot_index+=data_len;
    }


    glUseProgram(0);
    return 0;

}




