#include "theclient.h"
#include "mem.h"
#include "interface.h"





static inline void clone_box(GLshort *box_src, GLshort *box_dest)
{
    box_dest[0] = box_src[0];
    box_dest[1] = box_src[1];
    box_dest[2] = box_src[2];
    box_dest[3] = box_src[3];
}




struct CTRL* register_control(struct CTRL *parent, tileless_event click_func,void *onclick_arg, GLshort *box,int default_active)
{
    struct CTRL *ctrl = st_malloc(sizeof(struct CTRL));
    ctrl->active = default_active;
    ctrl->parent = parent;
    ctrl->children = NULL;
    ctrl->n_children = 0;
    ctrl->max_children = 0;
    ctrl->on_click = click_func;
    ctrl->onclick_data = onclick_arg;
    clone_box(box, ctrl->box);
    if(parent)
        parent_add_child(parent, ctrl);

    return ctrl;
}




int parent_add_child(struct CTRL *parent, struct CTRL *child)
{
    const int standard_add_n = 5;
    int new_n;
    if(parent->max_children ==0)
    {
        parent->children = st_malloc( standard_add_n * sizeof(struct CTRL*));
        parent->max_children = standard_add_n;
    }
    else if(parent->max_children-parent->n_children < 1)
    {
        new_n = parent->max_children + standard_add_n;
        parent->children = st_realloc(parent->children,new_n * sizeof(struct CTRL*));
        parent->max_children = new_n;
    }

    *(parent->children + parent->n_children) = child;
    parent->n_children++;
    return 0;
}


int switch_map_modus(void *val)
{
    if(map_modus)
    {
        log_this(100, "remove map_modus");
        info_box_color[0] = 0.8;
        info_box_color[1] = 1;
        info_box_color[2] = 0.8;
        info_box_color[3] = 0.9;
        map_modus = 0;
        return 0;
    }
    else
    {

        log_this(100, "set map_modus");
        info_box_color[0] = 1;
        info_box_color[1] = 1;
        info_box_color[2] = 1;
        info_box_color[3] = 0.9;
        map_modus = 1;
        return 1;
    }

    printf("Yes\n");
    return 0;
}



int init_controls()
{
    GLshort box[] = {0,0,0,0};
    controls =  register_control(NULL,NULL, NULL, box,1);
    GLshort box2[] = {5,75,300,225};
    register_control(controls, switch_map_modus, NULL,box2, 1);
    return 0;
}

static int check_box(GLshort *box,int x, int y)
{
    if(box[0] < x && box[1] < y && box[2] > x && box[3] > y)
        return 1;

    return 0;

}
static int check_controls(struct CTRL *ctrl, int x, int y)
{

    int i;
    if(!ctrl->active)
        return 0;

    log_this(100, "checkcontrol, x = %d, y = %d\n",x,y);
    for (i=0; i<ctrl->n_children; i++)
    {
        struct CTRL *child = *(ctrl->children+i);

        if(child->active && check_box(child->box, x,y))
        {
            child->on_click(child->onclick_data);
            check_controls(child, x, y);
        }

    }
    return 0;
}

int check_click(int x, int y)
{

    check_controls(controls, x,CURR_HEIGHT -  y);
    return 0;
}

