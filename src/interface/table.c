/**********************************************************************
 *
 * TilelessMap
 *
 * TilelessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TilelessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TilelessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/



#include "interface.h"
#include "../mem.h"
#include "../log.h"
#include <stddef.h>
CTRL* add_table(struct CTRL *caller, struct CTRL *spatial_parent, GLfloat *color, short box[4],short *margin, tileless_event_function click_func)
{

    int z = spatial_parent->z + 1;

    return register_control(TABLE,spatial_parent,caller,click_func, NULL, NULL,box,color,NULL,margin, 7, z);


}


CTRL* add_row(struct CTRL *spatial_parent, GLfloat *color, short ncols, short *column_widths)
{

    if(!(spatial_parent->type == TABLE))
    {
        log_this(100,"Spatial parent type must be TABLE to a TABLE_ROW");
        return NULL;
    }

    int z = spatial_parent->z + 0.001;

    int n_siblings = spatial_parent->relatives->n_children;
    short x1, x2, y1, y2;
    x1 = spatial_parent->box[0] + spatial_parent->txt_margin[0];
    x2 = spatial_parent->box[2] - spatial_parent->txt_margin[0];
    if(n_siblings == 0)
        y2 = spatial_parent->box[3] - spatial_parent->txt_margin[1];
    else y2 = spatial_parent->relatives->children[n_siblings-1]->box[1];
    y1 = y2;

    int i;
    short sum_width = 0;
    for (i=0; i<ncols; i++)
    {
        sum_width+=column_widths[i];
    }
    short margin[2];
    if(sum_width>x2-x1)
    {
        x2 = x1+sum_width;
        margin[0] = 0;
    }
    else
        margin[0] = (short) (x2-x1-sum_width)*0.5;

    margin[1] = 10;

    short box[4] = {x1,y1,x2,y2};

    CTRL *ctrl = register_control(TABLE_ROW,spatial_parent,spatial_parent,NULL, NULL, NULL,box,color,NULL,margin, 7, z);

    ctrl->child_constriants = st_malloc(sizeof(CTRL_CHILD_CONSTRINTS));
    ctrl->child_constriants->max_children = ncols;
    ctrl->child_constriants->widths_list = st_malloc(sizeof(short)*ncols);
    memcpy(ctrl->child_constriants->widths_list, column_widths,sizeof(short) * ncols);

    return ctrl;

}

static short check_row_height(TEXTBLOCK *tb)
{
    unsigned int i;


    short height =0;
    for (i=0; i<tb->txt_info->ntexts; i++)
    {
        height += tb->dims->heights->list[i];
    }


    return height;
}


CTRL* add_cell(struct CTRL *spatial_parent,char *txt, GLfloat *color,GLfloat *font_color, short *margin, tileless_event_function click_func,void *val, int font_size)
{
    if(!(spatial_parent->type == TABLE_ROW))
    {
        log_this(100,"Spatial parent type must be TABLE_ROW for a TABLE_CELL");
        return NULL;
    }
    int n_siblings = spatial_parent->relatives->n_children;
    if(n_siblings>= spatial_parent->child_constriants->max_children)
        log_this(100,"There is no more space on this row. It is already %d cells", n_siblings);


    short max_width = spatial_parent->child_constriants->widths_list[n_siblings];

    int z = spatial_parent->z + 0.001;
    TEXTBLOCK *tb = init_textblock();

    ATLAS *font = loadatlas("freesans",BOLD_TYPE, font_size);
    append_2_textblock(tb,txt, font, font_color,max_width - 2*margin[0], NEW_STRING, tmp_unicode_txt);

    short x1, x2, y1, y2;
    if(n_siblings == 0)
        x1 = spatial_parent->box[0] + spatial_parent->txt_margin[0];
    else
        x1 = spatial_parent->relatives->children[n_siblings-1]->box[2];

    x2 = x1+max_width;
    y2 = spatial_parent->box[3];
    short cellheight = check_row_height(tb);
    cellheight += 2* margin[1];
    y1 = y2-cellheight;

    short rowheight = spatial_parent->box[3]-spatial_parent->box[1] - 2 * spatial_parent->txt_margin[1];


    if(cellheight > rowheight)
        spatial_parent->box[1] -= cellheight-rowheight;

    short box[] = {x1, y1, x2, y2};

    CTRL *cell = register_control(TABLE_CELL,spatial_parent,spatial_parent,click_func, val, NULL,box,color, tb,margin, 7, z);
    return cell;
}




