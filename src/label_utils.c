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
#include "buffer_handling.h"
#include "utils.h"
#include "rendering.h"



static inline int add_line(ATLAS *a,GLfloat *x, GLfloat y, uint32_t *txt, unsigned int n_chars, POINT_T *coords )
{

    uint32_t p;
    unsigned int i, c=0;
    for(i = 0; i<n_chars; i++)
    {

        p = *(txt + i);
        /* Calculate the vertex and texture coordinates */
       
        float x2 = *x + a->metrics[p].bl;
        float y2 = -(y) - a->metrics[p].bt;
        float w = a->metrics[p].bw;
        float h = a->metrics[p].bh;
 //       float h = a->ch * sy;

        /* Advance the cursor to the start of the next character */
        *x += a->metrics[p].ax;
        y += a->metrics[p].ay;

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
    return c;
}

int calc_dims(TEXTBLOCK *tb,int max_width, WCHAR_TEXT *unicode_txt)
{
    GLfloat x,y;
    uint8_t p;
    unsigned int i, c=0;
    float max_used_width = 0;
        char *error_txt = "text error";
    
        size_t str_len;
    char *txt_start, *txt, *txt_end;
    int list_size;
    list_size = tb->formating->txt_index->used;
    unsigned int text_start_index, total_line_length;
    
    if(list_size < 2)
    {
        log_this(100,"Error not initialized text in %s\n",__func__);
        txt_start = txt = error_txt;
        txt_end = txt_start + strlen(error_txt);
    }
    else
    {    
        text_start_index = total_line_length = tb->formating->txt_index->list[list_size-2];
        txt_start = txt = tb->txt->txt + text_start_index;
        txt_end = tb->txt->txt + tb->formating->txt_index->list[list_size-1];  
    }
    
     str_len = txt_end-txt_start;
    size_t npoints = 6 * str_len;
    size_t coordssize = npoints * sizeof(POINT_T);
    
    check_and_realloc_txt_coords(tb->dims->coords,coordssize); 
    
    POINT_T *coords = tb->dims->coords->coords + tb->dims->coords->used;
    
    x = tb->cursor_x;
    y = tb->cursor_y;

    
    reset_wc_txt(unicode_txt);
    add_n_utf8_2_wc_txt(unicode_txt, txt, str_len);
    
    ATLAS *a;
    
    list_size = tb->formating->font->used;
    a = tb->formating->font->list[list_size-1];
    
    GLfloat rh = max_f(tb->rowheight, a->ch); //If there has been used a larger font on the same row we have to use that height
    
    
        GLfloat line_width = x, word_width = 0;
        
        int nlines=0;
        if(x==0 && y==0)
            nlines++;
        unsigned int line_start=0;
    if(max_width)
    {
        unsigned int n_chars_in_line = 0, n_chars_in_word = 0; 
        //uint32_t *last_word = 0;
        for(i = 0; i<unicode_txt->used; i++)
        {
            p = *(unicode_txt->txt + i);
            word_width += a->metrics[p].ax;
            n_chars_in_word++;
            if(p=='\0')
                break;
            else if(p==32)
            {
                n_chars_in_line += n_chars_in_word;
                line_width += word_width;
                n_chars_in_word = 0;
                word_width = 0;
            }
            else if (p=='\n')
            {
                total_line_length += i;
                add2gluint_list(tb->dims->linestart,total_line_length);
                add2glfloat_list(tb->dims->line_widths, line_width);
                n_chars_in_line += n_chars_in_word;
                c += add_line(a,&x,y - rh*nlines,unicode_txt->txt + line_start,n_chars_in_line,  coords+c) ;
                line_start = i;
                max_used_width = max_f(max_used_width, line_width);
                word_width = line_width = 0;
                n_chars_in_line = n_chars_in_word =0;
                nlines++;
                x = 0;
            }
            if(line_width + word_width > max_width)
            {
                if(line_width == 0) //there is only 1 word in line, we have to cut the word
                {
                    
                total_line_length += n_chars_in_word-1;
                    c += add_line(a,&x,y - rh*nlines,unicode_txt->txt + line_start,n_chars_in_word-1,  coords+c) ;
                    line_start = i;
                    word_width = line_width = 1;
                    n_chars_in_word =1;
                    max_used_width = max_width;
                }
                else //we put the last word on the next line instead
                {
                    total_line_length += n_chars_in_line;
                    c += add_line(a,&x,y - rh*nlines,unicode_txt->txt + line_start,n_chars_in_line,  coords+c) ;
                    max_used_width = max_f(max_used_width, line_width);
                    line_start += n_chars_in_line;
                    line_width = 0;
                    n_chars_in_line =0;
                }

                    add2gluint_list(tb->dims->linestart,total_line_length);
                nlines++;
                x = 0;
            }
        }
        if(word_width > 0 || line_width > 0)
        {

            n_chars_in_line += n_chars_in_word;
            line_width += word_width;
            c += add_line(a,&x,y - rh*nlines,unicode_txt->txt + line_start,n_chars_in_line, coords+c) ;
        }

        max_used_width = max_f(max_used_width, line_width);
    }
    else
    {
        c += add_line(a,&x,y - rh*nlines,unicode_txt->txt,str_len,  coords);
        max_used_width = x;
    }
    tb->dims->coords->used+=c;
    tb->cursor_x =x;
    tb->cursor_y -= rh*nlines;    
    
    tb->dims->max_widths->list[tb->dims->max_widths->used-1] = max_f(tb->dims->max_widths->list[tb->dims->max_widths->used-1], max_width);
    tb->dims->widths->list[tb->dims->widths->used-1] = max_f(tb->dims->widths->list[tb->dims->widths->used-1], max_used_width);
    tb->dims->heights->list[tb->dims->heights->used-1] +=rh*nlines;
  //  printf("height = %f in func %s with text %s\n", tb->dims->heights->list[tb->dims->heights->used-1], __func__, txt);
    
return 0;
}


//TODO, need to be reworked for TEXTBOX, but don't know if this function is of any use
int print_txt(GLfloat *point_coord,GLfloat *point_offset, MATRIX *matrix_hndl,GLfloat *color,int size,int bold,int max_width, const char *txt, ... )
{


    char txt_tot[1024];
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

    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error wt:%d\n", err);
    }

    glUniformMatrix4fv(txt_matrix, 1, GL_FALSE,theMatrix );


//TODO, need to be reworked for TEXTBOX, but don't know if this function is of any use
   // draw_it(norm_color,point_coord,point_offset, a, txt_box, txt_color, txt_coord2d, txt_tot,max_width, sx, sy);


    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error wt2:%d\n", err);
    }
    return 0;
}



int print_txtblock(GLfloat *point_coord, MATRIX *matrix_hndl,  TEXTBLOCK *tb,float *anchor, float *displacement)
{
    GLfloat *theMatrix;
    GLfloat sx = (GLfloat) (2.0 / CURR_WIDTH);
    GLfloat sy = (GLfloat)(2.0 / CURR_HEIGHT);

    GLfloat matrix_array[16] = {sx, 0,0,0,0,sy,0,0,0,0,1,0,-1,-1,0,1};
    if(matrix_hndl)
        theMatrix = matrix_hndl->matrix;
    else
        theMatrix = (GLfloat *) matrix_array;

    if(!tb->txt_info->points)
    {
        tb->txt_info->points = init_tb_point_list();
        addbatch2glfloat_list(tb->txt_info->points->points,2,point_coord);
        add2gluint_list(tb->txt_info->points->point_start_indexes, 0); 
    glBindBuffer(GL_ARRAY_BUFFER, tb->txt_info->points->tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*(tb->dims->coords->used), tb->dims->coords->coords, GL_STATIC_DRAW);       
    }
       while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"1 - opengl error:%d in func %s\n", err, __func__);
        }
   
          while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"3 - opengl error:%d in func %s\n", err, __func__);
        }
 
    
    glUseProgram(txt2_program);



    /*   GLfloat point_coord[2];

       point_coord[0]= x;
       point_coord[1]= y;
     */
    //  glUniform4fv(txt_color,1,norm_color );

    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error wt:%d\n", err);
    }
    


    draw_txt(tb,theMatrix, theMatrix,anchor, displacement);


    return 0;
}
