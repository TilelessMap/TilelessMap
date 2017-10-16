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
#define START_MAX_N_VERTEX 1000
#define START_MAX_N_PA 100
#define START_MAX_CHARS 1000
#define START_MAX_labels 100








TEXTSTRUCT* init_text_buf()
{
    TEXTSTRUCT *text_buf;
    size_t char_size = sizeof(char)*START_MAX_CHARS; //in bytes

    text_buf = malloc(sizeof(TEXTSTRUCT));
    text_buf->char_array = malloc(char_size);
    text_buf->max_n_chars = START_MAX_CHARS;
    text_buf->used_n_chars = 0;
    text_buf->rotation= malloc(sizeof(float)*START_MAX_labels);
    text_buf->size= malloc(sizeof(float)*START_MAX_labels);
    text_buf->styleID= malloc(sizeof(uint32_t)*START_MAX_labels);
    text_buf->anchor= malloc(sizeof(uint32_t)*START_MAX_labels);

    text_buf->used_n_vals = 0;

    text_buf->max_n_vals=START_MAX_labels ;
    text_buf->tb = NULL;


    //printf("buffer size = %ld\n", res_buf->index_array-res_buf->buffer_end);
    return text_buf;
}





int text_write(const char *the_text,uint32_t styleID, GLshort size, float rotation,uint32_t anchor, TEXTSTRUCT *text_buf)
{
// log_this(10, "Entering %s with text = %s\n",__func__, the_text);


    if(!the_text)
        the_text = "";


    size_t new_size, new_n_vals;

    size_t len_of_str = strlen((const char*) the_text) + 1;

    char *new_array;

    while (text_buf->max_n_chars-text_buf->used_n_chars < len_of_str)
    {
        new_size = text_buf->max_n_chars * 2;					//number of floats
        log_this(10, "Ok, increase space for text_labels from  %d bytes\n",(int) text_buf->max_n_chars);
        new_array = realloc(text_buf->char_array, new_size*sizeof(char)); //In bytes

        if (!new_array)
        {
            log_this(100, "Problem allocating memory %s\n",__func__);
            return 1;
        }
        text_buf->char_array = new_array;
        text_buf->max_n_chars = new_size;
    }

    strncpy(text_buf->char_array + text_buf->used_n_chars, the_text, len_of_str);
    text_buf->used_n_chars += len_of_str;

    while(text_buf->max_n_vals - text_buf->used_n_vals < 1)
    {
        new_n_vals = text_buf->max_n_vals * 2;

        text_buf->size = realloc(text_buf->size, new_n_vals * sizeof(float));
        text_buf->rotation = realloc(text_buf->rotation, new_n_vals * sizeof(float));
        text_buf->anchor = realloc(text_buf->anchor, new_n_vals * sizeof(uint32_t));
        text_buf->styleID = realloc(text_buf->styleID, new_n_vals * sizeof(uint32_t));
        text_buf->max_n_vals = new_n_vals;
    }

    *(text_buf->size + text_buf->used_n_vals) = size;
    *(text_buf->rotation + text_buf->used_n_vals) = rotation;
    *(text_buf->styleID + text_buf->used_n_vals) = styleID;
    *(text_buf->anchor + text_buf->used_n_vals) = anchor;

    text_buf->used_n_vals++;

    return 0;
}


























