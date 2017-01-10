/**********************************************************************
 *
 * NoTile
 *
 * NoTile is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * NoTile is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NoTile.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016 Nicklas Avén
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
  //  GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;
    // glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(2);
    glUseProgram(oneLayer->program);
    glEnableVertexAttribArray(oneLayer->attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
        oneLayer->attribute_coord2d, // attribute
        ndims,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
 /*   while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem1\n"));
        fprintf(stderr,"opengl error:%d\n", err);
    }*/
//    glUniform1fv(uniform_bbox, 4, bbox);
    glUniformMatrix4fv(oneLayer->uniform_theMatrix, 1, GL_FALSE,theMatrix );

/*
    while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem3: %d\n", err));
        fprintf(stderr,"opengl error:%d\n", err);
    }*/

    if(oneLayer->show_text && oneLayer->text->used_n_vals!=rb->used_n_pa)
      printf("There is a mismatch between number of labels and number of corresponding points\n");
    int used=0;
    for (i=0; i<rb->used_n_pa; i++)
    {
        Uint32 styleID = *(rb->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            color = global_styles[styleID].color;
        }
        glUniform4fv(oneLayer->uniform_color,1,color );
        glDrawArrays(GL_POINTS, *(rb->start_index+i), *(rb->npoints+i));
	     if(oneLayer->show_text)
	    {
	      int antal = oneLayer->text->used_n_vals;
	      
	      printf("start, print text %d\n",antal);

		char *txt=oneLayer->text->char_array+used;
		printf("text %s, x %f, y %f, startindex = %d\n",txt, *(rb->vertex_array+ *(rb->start_index+i)*ndims), *(rb->vertex_array+ *(rb->start_index+i)*ndims+1), *(rb->start_index+i));
		used+=strlen(txt)+1;
	      
	      printf("printed %d texts\n",i);
	      
	    }
    }
    glDisableVertexAttribArray(oneLayer->attribute_coord2d);

    return 0;

}



int loadLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    GLESSTRUCT *rb = oneLayer->res_buf;

    glGenBuffers(1, &(oneLayer->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(rb->first_free-rb->vertex_array), rb->vertex_array, GL_STATIC_DRAW);

    renderLine( oneLayer, theMatrix, 0);
    return 0;
}



int renderLine(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix, int outline)
{
    DEBUG_PRINT(("Entering renderLine\n"));
    uint32_t i;//, np, pi;
    GLfloat *color, lw;
 //   GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;

    glUseProgram(oneLayer->program);
    glEnableVertexAttribArray(oneLayer->attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
        oneLayer->attribute_coord2d, // attribute
        2,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
 /*   while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem1\n"));
        fprintf(stderr,"opengl error:%d", err);
    }*/
//    glUniform1fv(uniform_bbox, 4, bbox);
    glUniformMatrix4fv(oneLayer->uniform_theMatrix, 1, GL_FALSE,theMatrix );
 /*   while ((err = glGetError()) != GL_NO_ERROR) {
        DEBUG_PRINT(("Problem 2\n"));
        fprintf(stderr,"opengl error:%d", err);
    }*/

  
    
    for (i=0; i<rb->used_n_pa; i++)
    {
        lw = 0;
        /*    np = *(rb->npoints+i);
            pi = *(rb->start_index+i);*/
//	style = oneLayer->styles[*(rb->styleID+i)] ;

        Uint32 styleID = *(rb->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            if(outline)
                color = global_styles[styleID].outlinecolor;
            else
                color = global_styles[styleID].color;

            lw = global_styles[styleID].lineWidth;

        }
        else
        {
            return 1;

        }
        glLineWidth(lw);
        if(lw)
        {
            glUniform4fv(oneLayer->uniform_color,1,color );
            glDrawArrays(GL_LINE_STRIP, *(rb->start_index+i), *(rb->npoints+i));
        }
    }
    glDisableVertexAttribArray(oneLayer->attribute_coord2d);

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
    renderPolygon( oneLayer, theMatrix);
    renderLine(oneLayer, theMatrix,1);
    return 0;
}



int renderPolygon(LAYER_RUNTIME *oneLayer,GLfloat *theMatrix)
{
    DEBUG_PRINT(("Entering renderPolygon\n"));
    uint32_t i;//, np, pi;
    GLfloat *color;
//    GLenum err;
    glBindBuffer(GL_ARRAY_BUFFER, oneLayer->vbo);
    GLESSTRUCT *rb = oneLayer->res_buf;
    ELEMENTSTRUCT *ti = oneLayer->tri_index;

    glUseProgram(oneLayer->program);
    glEnableVertexAttribArray(oneLayer->attribute_coord2d);


    for (i=0; i<ti->used_n_pa; i++)
    {
        size_t  vertex_offset = *(rb->polygon_offset+i)  ;
        glVertexAttribPointer(
            oneLayer->attribute_coord2d, // attribute
            2,                 // number of elements per vertex, here (x,y)
            GL_FLOAT,          // the type of each element
            GL_FALSE,          // take our values as-is
            0,                 // no extra data between each position
            (GLvoid*) vertex_offset                  // offset of first element
        );
    /*    while ((err = glGetError()) != GL_NO_ERROR) {
            DEBUG_PRINT(("Problem1\n"));
            fprintf(stderr,"opengl error:%d", err);
        }*/


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oneLayer->ebo);
	
        glUniformMatrix4fv(oneLayer->uniform_theMatrix, 1, GL_FALSE,theMatrix );
      /*  while ((err = glGetError()) != GL_NO_ERROR) {
            DEBUG_PRINT(("Problem 2\n"));
            fprintf(stderr,"opengl error:%d", err);
        }*/

        Uint32 styleID = *(ti->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            color = global_styles[styleID].color;
        }
        else
        {
            return 1;
        }

        glUniform4fv(oneLayer->uniform_color,1,color );






/*
        while ((err = glGetError()) != GL_NO_ERROR) {
            DEBUG_PRINT(("Problem3: %d\n", err));
            fprintf(stderr,"opengl error:%d", err);
        }
            np = *(ti->npoints+i);
            pi = *(ti->start_index+i);
            for (h=0;h<*(ti->npoints+i) * 3;h++)
            {
              int idx = *(ti->vertex_array + *(ti->start_index+i) * 3 + h);
              double coord1 = *(oneLayer->res_buf->vertex_array + 2 *  idx);
              double coord2 = *(oneLayer->res_buf->vertex_array + 2 *  idx + 1 );*/
        // DEBUG_PRINT(("idx : %d, %lf, %lf\n", idx, coord1, coord2));
        //     DEBUG_PRINT(("select 'point( %lf %lf)'::geometry geom union all\n",  coord1, coord2));
        //}
        //  int start =  * (ti->start_index+i);
        size_t index_offset = *(ti->start_index+i) * 3 *sizeof(GLushort) ;
        //  offset = i * 100;
        //int n = *(ti->npoints+i) * 3 ;
        glDrawElements(GL_TRIANGLES, *(ti->npoints+i) * 3,GL_UNSIGNED_SHORT,(GLvoid*) index_offset);

    }
    glDisableVertexAttribArray(oneLayer->attribute_coord2d);

    return 0;

}



/*This is actually not used now, but van be called to render without fetching data from db*/
int render_data(SDL_Window* window,GLfloat *bbox,GLfloat *theMatrix)
{


    DEBUG_PRINT(("Entering render_data\n"));
    int i;
    LAYER_RUNTIME *oneLayer;
    GLfloat meterPerPixel = (bbox[2]-bbox[0])/CURR_WIDTH;

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    for (i=0; i<nLayers; i++)
    {
        oneLayer = layerRuntime + i;


        if(oneLayer->visible && oneLayer->minScale<=meterPerPixel && oneLayer->maxScale>meterPerPixel)
        {


            switch(oneLayer->geometryType)
            {
            case POINTTYPE :
                renderPoint( oneLayer, theMatrix);
                break;
            case LINETYPE :
                renderLine( oneLayer, theMatrix, 0);
                break;
	    case POLYGONTYPE :
		renderPolygon( oneLayer, theMatrix);
		renderLine(oneLayer, theMatrix,1);
		break;
            }
        }
        
        
    SDL_GL_SwapWindow(window);
    }

    
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
  
    DEBUG_PRINT(("Entering renderText\n"));
    uint32_t i;
    GLfloat *color;
  int ndims=2;
	GLfloat x,y;
	const char *p;
	FT_GlyphSlot g = face->glyph;
	GLuint text_vbo;
	/* Create a texture that will be used to hold one "glyph" */
	GLuint tex;

	GLESSTRUCT *rb = oneLayer->res_buf;
	
	GLfloat point_coord[2];
	glUseProgram(oneLayer->txt_program);
	
	glUniformMatrix4fv(oneLayer->txt_theMatrix, 1, GL_FALSE,theMatrix );
	
	
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(text_uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(oneLayer->txt_box);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
	glVertexAttribPointer(oneLayer->txt_box, 4, GL_FLOAT, GL_FALSE, 0, 0);

	
	
	
	
	
		float sx = 2.0 / CURR_WIDTH;
	float sy = 2.0 / CURR_HEIGHT;

	
	if(oneLayer->show_text && oneLayer->text->used_n_vals!=rb->used_n_pa)
      printf("There is a mismatch between number of labels and number of corresponding points\n");
    int used=0;
    for (i=0; i<rb->used_n_pa; i++)
    {
        Uint32 styleID = *(rb->styleID+i);
        if(styleID<length_global_styles && global_styles[styleID].styleID == styleID)
        {
            color = global_styles[styleID].color;
        }
        glUniform4fv(oneLayer->txt_color,1,color );
	
	point_coord[0] =  *(rb->vertex_array+ *(rb->start_index+i)*ndims);
	point_coord[1] =  *(rb->vertex_array+ *(rb->start_index+i)*ndims + 1);
	
        glUniform2fv(oneLayer->txt_coord2d,1,point_coord );
	

	      
		char *txt=oneLayer->text->char_array+used;

		used+=strlen(txt)+1;
	 
		
		int psz =  (int) roundf(*(oneLayer->text->size+i)) * text_scale;
   FT_Set_Pixel_Sizes(face, 0,psz);
    
    
    x = 0;
    y = 0;
    
    
       
	/* Loop through all characters */
	for (p = txt; *p; p++) {
		/* Try to load and render the character */
		if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
			continue;

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		GLfloat x2 = x + g->bitmap_left * sx;
		GLfloat y2 = -y - g->bitmap_top * sy;
		GLfloat w = g->bitmap.width * sx;
		GLfloat h = g->bitmap.rows * sy;

		point box[4] = {
			{x2, -y2, 0, 0},
			{x2 + w, -y2, 1, 0},
			{x2, -y2 - h, 0, 1},
			{x2 + w, -y2 - h, 1, 1},
		};

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}
	
	
	 }
	

	glDisableVertexAttribArray(text_attribute_coord);
	glDeleteTextures(1, &tex);
 return 0;
  
 
}


/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
void render_text_test(const char *text, float x, float y, float sx, float sy)
{
	const char *p;
	FT_GlyphSlot g = face->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	GLuint tex;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(text_uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(text_attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
	glVertexAttribPointer(text_attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Loop through all characters */
	for (p = text; *p; p++) {
		/* Try to load and render the character */
		if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
			continue;

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point box[4] = {
			{x2, -y2, 0, 0},
			{x2 + w, -y2, 1, 0},
			{x2, -y2 - h, 0, 1},
			{x2 + w, -y2 - h, 1, 1},
		};

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glDisableVertexAttribArray(text_attribute_coord);
	glDeleteTextures(1, &tex);
}
