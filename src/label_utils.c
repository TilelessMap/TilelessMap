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


static inline float max_f(float a, float b)
{
    if (b > a)
        return b;
    else
        return a;
}



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

int calc_dims(TEXTBLOCK *tb,int max_width, int alignment)
{
    GLfloat x,y, *x_p, *y_p;
    uint8_t p;
    int i, c=0;
    float max_used_width;
        char *error_txt = "text error";
    
        size_t str_len;
    char *txt_start, *txt, *txt_end;
    int list_size;
    list_size = tb->formating->txt_index->used;
    
    
    if(list_size < 2)
    {
        log_this(100,"Error not initialized text in %s\n",__func__);
        txt_start = txt = error_txt;
        txt_end = txt_start + strlen(error_txt);
    }
    else
    {       
        txt_start = txt = tb->formating->txt_index->list[list_size-2];
        txt_end = tb->formating->txt_index->list[list_size-1];  
    }
    
    char* last_block_start, *last_block_end;
    list_size = tb->dims->txt_index->used;
    last_block_start = tb->dims->txt_index->list[list_size-2];
    last_block_end = tb->dims->txt_index->list[list_size-1];
    
     str_len = txt_end-txt_start;
    size_t npoints = 6 * str_len;
    size_t coordssize = npoints * sizeof(POINT_T);
    
    check_and_realloc_txt_coords(tb->dims->coords,coordssize); 
    
    POINT_T *coords = tb->dims->coords->coords + tb->dims->coords->used;
    
    x = tb->cursor_x;
    y = tb->cursor_y;

    
    reset_wc_txt(tmp_unicode_txt);
    add_n_utf8_2_wc_txt(tmp_unicode_txt, txt, str_len);
    
    ATLAS *a;
    
    list_size = tb->formating->font->used;
    a = tb->formating->font->list[list_size-1];
    
    GLfloat rh = max_f(tb->rowheight, a->ch); //If there has been used a larger font on the same row we have to use that height
    
    
        GLfloat line_width = 0, word_width = 0;
        int nlines=0;
        unsigned int line_start=0;
    if(max_width)
    {
        unsigned int n_chars_in_line = 0, n_chars_in_word = 0; 
        //uint32_t *last_word = 0;
        for(i = 0; i<tmp_unicode_txt->used; i++)
        {
            p = *(tmp_unicode_txt->txt + i);

            word_width += a->metrics[p].ax;
            n_chars_in_word++;

            if(p==32)
            {
                n_chars_in_line += n_chars_in_word;
                line_width += word_width;
                n_chars_in_word = 0;
                word_width = 0;
            }
            else if (p=='\n')
            {
                add2pointer_list(tb->dims->linebreaks, txt_start+i);
                add2glfloat_list(tb->dims->line_widths, line_width);
                n_chars_in_line += n_chars_in_word;
                c += add_line(a,&x,y - rh*nlines,tmp_unicode_txt->txt + line_start,n_chars_in_line,  coords+c) ;
                line_start = i;
                max_used_width = max_f(max_used_width, line_width);
                word_width = line_width = 0;
                n_chars_in_line = n_chars_in_word =0;
                nlines++;
                x = 0;
            }
            if(line_width + word_width > max_width)
            {
                if(n_chars_in_line == 0) //there is only 1 word in line, we have to cut the word
                {
                    c += add_line(a,&x,y - rh*nlines,tmp_unicode_txt->txt + line_start,n_chars_in_word-1,  coords+c) ;
                    line_start = i;
                    word_width = line_width = 1;
                    n_chars_in_line = n_chars_in_word =1;
                    max_used_width = max_width;
                }
                else //we put the last word on the next line instead
                {
                    c += add_line(a,&x,y - rh*nlines,tmp_unicode_txt->txt + line_start,n_chars_in_line,  coords+c) ;
                    max_used_width = max_f(max_used_width, line_width);
                    line_start += n_chars_in_line;
                    line_width = 0;
                    n_chars_in_line =0;
                }

                    add2pointer_list(tb->dims->linebreaks, txt_start+line_start);
                nlines++;
                x = 0;
            }
        }
        if(word_width > 0 || line_width > 0)
        {

            n_chars_in_line += n_chars_in_word;
            line_width += word_width;
            c += add_line(a,&x,y - rh*nlines,tmp_unicode_txt->txt + line_start,n_chars_in_line, coords+c) ;
        }

        max_used_width = max_f(max_used_width, line_width);
    }
    else
    {
        c += add_line(a,&x,y,tmp_unicode_txt->txt,str_len,  coords);
        nlines++;
        max_used_width = x;
    }
    tb->dims->coords->used+=c;
    
    tb->cursor_x =x;
    tb->cursor_y -= rh*nlines;    
    
    tb->dims->max_widths->list[tb->dims->max_widths->used-1] = max_f(tb->dims->max_widths->list[tb->dims->max_widths->used-1], max_width);
    tb->dims->widths->list[tb->dims->widths->used-1] = max_f(tb->dims->widths->list[tb->dims->widths->used-1], max_used_width);
    tb->dims->heights->list[tb->dims->heights->used-1] -=rh*nlines;
    

}


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
        theMatrix = (GLfloat *) matrix_array;



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


    for (i=0; i<tb->formating->txt_index->used; i++)
    {
        draw_it(norm_color,point_coord,point_offset, tb->formating->font->list[i], txt_box, txt_color, txt_coord2d, tb->formating->txt_index->list[i],max_width, sx, sy);
    }


    return 0;
}
