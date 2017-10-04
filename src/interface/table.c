


#include "interface.h"
#include "../mem.h"
CTRL* add_table(struct CTRL *caller, struct CTRL *spatial_parent, GLfloat *color, short box[4],short *margin)
{
    
        int z = spatial_parent->z + 1;
       
    return register_control(TABLE,spatial_parent,caller,NULL, NULL, NULL,box,color,NULL,margin, 0, z);

    
}


CTRL* add_row(struct CTRL *caller, struct CTRL *spatial_parent, GLfloat *color, short box[4], short ncols, short *column_widths)
{
    
        int z = spatial_parent->z + 1;
       
        CTRL *ctrl = register_control(TABLE,spatial_parent,caller,NULL, NULL, NULL,box,color,NULL,margin, 0, z);
        
     ctrl->child_constriants = st_malloc(sizeof(CTRL_CHILD_CONSTRINTS));     
     ctrl->child_constriants->max_children = ncols;
     ctrl->child_constriants->widths_list = st_malloc(sizeof(short)*ncols);
     memcpy(ctrl->child_constriants->widths_list, column_widths, ncols);

     return ctrl;
    
}


CTRL add_cell(struct CTRL *caller, struct CTRL *spatial_parent, GLfloat *color, short *margin, tileless_event_function click_func,void *val, int font_size)
{
 
    int n_used = spatial_parent->relatives->n_children;
    
    short max_width = spatial_parent->child_constriants->widths_list[n_used];
    
    
    
}
