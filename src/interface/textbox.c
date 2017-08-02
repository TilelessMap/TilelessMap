
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


#include "../theclient.h"
#include "../mem.h"
#include "interface.h"




struct CTRL* init_textbox(struct CTRL *spatial_parent,struct CTRL *logical_parent, GLshort *box,GLfloat *color, GLshort *txt_margin,int default_active, int z)
{
 
    return register_control(TEXTBOX, spatial_parent, logical_parent,NULL, NULL, NULL,box, color, NULL,txt_margin,default_active, z);
    
}


int add_txt_2_textbox(struct CTRL *ctrl, TEXTBLOCK *txt)
{
    ctrl->txt = txt;
    return 0;    
}

