
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


#include <string.h>
#define NULL ((void *)0)
 
typedef struct
{
    char *txt;
    size_t alloced;
    size_t used;
}
TEXT;


TEXT* init_txt(size_t s);
int add_txt(TEXT *t,const char *in);
char* get_txt(TEXT *t);
int reset_txt(TEXT *t);
int destroy_txt(TEXT *t);
