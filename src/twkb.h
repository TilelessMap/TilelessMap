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
#ifndef _twkb_H
#define _twkb_H

#include "text.h"
#include "buffer_handling.h"
#include "structures.h"
/***************************************************************
			DECODING TWKB						*/
/*Holds a buffer with the twkb-data during read*/



/*Maximum number of dimmensions that a twkb geoemtry
can hold according to the specification*/












GLFLOAT_LIST* get_coord_list(LAYER_RUNTIME *l, TWKB_PARSE_STATE *ts);
GLFLOAT_LIST* get_wide_line_list(LAYER_RUNTIME *l, TWKB_PARSE_STATE *ts);



int text_write(const char *the_text,uint32_t styleID, GLshort size, float rotation,uint32_t anchor, TEXTSTRUCT *text_buf);
void text_reset_buffer(TEXTSTRUCT *text_buf);
void text_destroy_buffer(TEXTSTRUCT *text_buf);


//int id; //just for debugging, remove later

/* Functions for decoding twkb*/
int read_header (TWKB_PARSE_STATE *ts);
int decode_twkb_start(uint8_t *buf, size_t buf_len);
int decode_twkb(TWKB_PARSE_STATE *old_ts);
int* decode_element_array(TWKB_PARSE_STATE *old_ts);

/*a type holding pointers to our parsing functions*/
typedef int (*parseFunctions_p)(TWKB_PARSE_STATE*);

/*Functions for decoding varInt*/
int64_t unzigzag64(uint64_t val);
uint64_t buffer_read_uvarint(TWKB_BUF *tb);
int64_t buffer_read_svarint(TWKB_BUF *tb);
uint8_t buffer_read_byte(TWKB_BUF *tb);
void buffer_jump_varint(TWKB_BUF *tb,int n);



/*resetting GLESSTRUCT buffer*/
void reset_buffer();

/*resetting ELEMENTSTRUCT buffer*/
void element_reset_buffer();


#endif
