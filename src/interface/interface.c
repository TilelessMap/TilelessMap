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

int multiply_array(GLshort *a, GLfloat v, GLshort ndims)
{
    int i;
    for(i=0; i<ndims; i++)
    {
        printf("a = %d\n", *(a+i));
        *(a+i) *= v;
    }
    return 0;
}

int check_screen_size()
{
    if(CURR_WIDTH > 1000)
    {
        log_this(100,"BIGSCREEN");
        screensize = BIGSCREEN;
        size_factor = 2;
        character_size = 4;
        text_size = 3;
    }
    else if (CURR_WIDTH >500)
    {
        log_this(100,"MIDDLESCREEN");
        screensize = MIDDLESCREEN;
        size_factor = 1.5;
        character_size = 3;
        text_size = 2;
    }
    else
    {
        log_this(100,"SMALLSCREEN");
        screensize = SMALLSCREEN;
        size_factor = 1;
        character_size = 2;
        text_size = 1;
    }

    return 0;

}

int get_parent_origo(struct CTRL *t, GLshort *p)
{
    p[0] = p[1] = 0;

    struct CTRL *parent = t->spatial_family->parent;
    while (parent)
    {
        p[0] += parent->box[0];
        p[1] += parent->box[1];
        parent = parent->spatial_family->parent;
    }
    return 0;
}


int get_bottom_left(struct CTRL *t, GLshort *p)
{

    p[0] = p[1] = 0;
    //get_parent_origo(t, p);

    p[0] += t->box[0];
    p[1] += t->box[1];

    return 0;
}

int get_top_left(struct CTRL *t, GLshort *p)
{
    p[0] = p[1] = 0;
    //get_parent_origo(t, p);

    p[0] += t->box[0];
    p[1] += t->box[3];

    return 0;
}

int get_bottom_right(struct CTRL *t, GLshort *p)
{
    p[0] = p[1] = 0;
    //get_parent_origo(t, p);

    p[0] += t->box[2];
    p[1] += t->box[1];

    return 0;
}

int get_top_right(struct CTRL *t, GLshort *p)
{
    p[0] = p[1] = 0;
    //get_parent_origo(t, p);

    p[0] += t->box[2];
    p[1] += t->box[3];

    return 0;
}

static RELATIONS *init_family(struct CTRL *parent)
{
    RELATIONS *t = st_malloc(sizeof(RELATIONS));


    t->children = st_malloc(sizeof(struct CTRL));


    t->max_children = 1;
    t->n_children = 0;
    t->parent = parent;

    return t;
}

static int realloc_family(RELATIONS *t)
{
    size_t new_s = t->max_children * 2;


    t->children = realloc(t->children, new_s * sizeof(struct CTRL *));
    if(!t->children)
    {
        log_this(100,"Failed to realloc memory in func %s",__func__);
        t->max_children = 0;
        return 1;
    }
    t->max_children = new_s;
    return 0;
}


static int add_child(RELATIONS *t, struct CTRL *child)
{

    if(t->max_children <= t->n_children)
        if(realloc_family(t))
            return 1;

    *(t->children + t->n_children) = child;
    t->n_children++;
    return 0;
}

static int remove_child(RELATIONS *t, struct CTRL *child)
{
    int i, len = t->n_children;
    for (i=0; i<len; i++)
    {
        if(t->children[i] == child)
        {
            if(i<len-1) //if there is children after the one we want to remove
            {
                t->children[i] = t->children[len-1]; //since we don't care about the order of the children we just copy the last child to the removed childs place
                t->children[len-1] = NULL; // and sets the last child slot to NULL
            }
            else
                t->children[i] = NULL; //if the child to remove is the last child we just set the slot to NULL


        }
    }
    t->n_children --;
    return 0;
}


static int destroy_family(RELATIONS *t)
{

    if (!t->n_children == 0)
    {
        printf("Something strange, there are %d children left\n", t->n_children);
        return 0;
    }
    t->max_children=0;
    t->n_children=0;
    t->parent = NULL;
    free(t);
    t=NULL;
    return 0;
}


int destroy_control(struct CTRL *t)
{
    int r=0;

    while (t->logical_family->n_children)
    {
        struct CTRL *child = t->logical_family->children[t->logical_family->n_children -1];
        if(child)
        {
            r ++;
            destroy_control(child);
        }
    }
    free(t->logical_family->children);

    while (t->spatial_family->n_children)
    {
        struct CTRL *child = t->spatial_family->children[t->spatial_family->n_children -1];
        if(child)
        {
            r ++;
            destroy_control(child);
        }
    }
    if(t == incharge)
        incharge = NULL;
    
    free(t->spatial_family->children);
    remove_child(t->logical_family->parent->logical_family, t);
    remove_child(t->spatial_family->parent->spatial_family, t);
    destroy_family(t->logical_family);
    destroy_family(t->spatial_family);
    if(t->txt)
        destroy_textblock(t->txt);

    if(t->matrix_handler)
        free(t->matrix_handler);
    
    if(t->on_click.data)
        free(t->on_click.data);
    free(t);
    t=NULL;

    return 0;
}

static inline void clone_box(GLshort *box_src, GLshort *box_dest)
{
    box_dest[0] = box_src[0];
    box_dest[1] = box_src[1];
    box_dest[2] = box_src[2];
    box_dest[3] = box_src[3];
}

int init_matrix_handler(struct CTRL *ctrl, uint8_t vertical_enabled, uint8_t horizontal_enabled, uint8_t zoom_enabled)
{
    ctrl->matrix_handler = st_malloc(sizeof(MATRIX));
    ctrl->matrix_handler->vertical_enabled = vertical_enabled;
    ctrl->matrix_handler->horizontal_enabled = horizontal_enabled;
    ctrl->matrix_handler->zoom_enabled = zoom_enabled;

    ctrl->matrix_handler->bbox[0] = 0;
    ctrl->matrix_handler->bbox[1] = 0;
    ctrl->matrix_handler->bbox[2] = CURR_WIDTH;
    ctrl->matrix_handler->bbox[3] = CURR_HEIGHT;

    return 0;

}


//struct CTRL* register_control(struct CTRL *parent, tileless_event_function click_func,void *onclick_arg, GLshort *box,int default_active)
struct CTRL* register_control(int type, struct CTRL *spatial_parent,struct CTRL *logical_parent, tileless_event_function click_func,void *onclick_arg,tileless_event_func_in_func func_in_func, GLshort *box,GLfloat *color,TEXTBLOCK *txt, GLshort *txt_margin, int default_active, int z)
{
    struct CTRL *ctrl = st_malloc(sizeof(struct CTRL));
    ctrl->type = type;
    ctrl->active = default_active;
    ctrl->z = z;
    ctrl->logical_family = init_family(logical_parent);
    ctrl->spatial_family = init_family(spatial_parent);

    ctrl->on_click.te_func = click_func;
    ctrl->on_click.data = onclick_arg;
    ctrl->on_click.caller = ctrl;
    ctrl->on_click.te_func_in_func = func_in_func;

    if(color)
    {
        ctrl->color[0] = color[0];
        ctrl->color[1] = color[1];
        ctrl->color[2] = color[2];
        ctrl->color[3] = color[3];
    }
    if(txt_margin)
    {
        ctrl->txt_margin[0] = txt_margin[0];
        ctrl->txt_margin[1] = txt_margin[1];
    }

    ctrl->txt = txt;
    clone_box(box, ctrl->box);
    if(spatial_parent)
        add_child(spatial_parent->spatial_family, ctrl);
    if(logical_parent)
        add_child(logical_parent->logical_family, ctrl);

    ctrl->matrix_handler = NULL;

    return ctrl;
}


/*

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
}*/

struct CTRL* add_close_button(struct CTRL *ctrl)
{
    
    GLshort box_text_margins[] = {4,4};
    multiply_array(box_text_margins, size_factor, 2);
    
    GLshort click_size = 30 * size_factor;
    GLshort startx, starty, p[] = {0,0};
    get_top_right(ctrl, p);

    startx = p[0] - 50*size_factor;
    starty = p[1] - 50*size_factor;

    GLshort close_box[] = {startx, starty,startx + click_size,starty + click_size};

    GLfloat close_color[]= {200,100,100,200};
    
    TEXTBLOCK *x_txt = init_textblock(1,0);
    append_2_textblock(x_txt,"X", fonts[0]->fss->fs[character_size].bold);
    return register_control(BUTTON, ctrl,ctrl, close_ctrl,NULL,NULL,close_box,close_color,x_txt,box_text_margins, 1,10);

    
    
    
}


int close_ctrl(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);

    struct CTRL *t = (struct CTRL *) ctrl;
    //incharge = NULL; //move focus back to map
    destroy_control(t->logical_family->parent);


    return 0;
}




static int check_box(GLshort *box,int x, int y)
{

    if(box[0] < x && box[1] < y && box[2] > x && box[3] > y)
        return 1;

    return 0;

}
static int check_controls(struct CTRL *ctrl, int x, int y, tileless_event *event, int *z, int *any_hit)
{
    int i, n_children;
    if(!ctrl->active)
        return 0;


//    matrix_hndl = ctrl->matrix_handler;

    log_this(100, "checkcontrol, x = %d, y = %d\n",x,y);
    n_children = ctrl->spatial_family->n_children;
    for (i=0; i<n_children; i++)
    {
        struct CTRL *child = *(ctrl->spatial_family->children+i);
        if(child == incharge)
        {
            GLfloat ny_x = (GLfloat) x, ny_y = (GLfloat) y;
            GLfloat *matrix = child->matrix_handler->matrix;

            // printf("matrix[0] = %f, 5 = %f, 12 = %f, 13 = %f, x = %f, y=%f\n", matrix[0],matrix[5],matrix[12],matrix[13], ny_x, ny_y);
            ny_x = ny_x * CURR_WIDTH * matrix[0] / 2 - CURR_WIDTH * (1 + matrix[12])/2;
            ny_y = ny_y * CURR_HEIGHT * matrix[5] / 2 - CURR_HEIGHT * (1+matrix[13])/2;
            x = (int) roundf(ny_x);
            y = (int) roundf(ny_y);
            //  printf("nyX = %f, nyY = %f\n", ny_x,ny_y);
        }
        if(child->active && check_box(child->box, x,y))
        {
            *any_hit = 1;
            check_controls(child, x, y, event, z, any_hit);

            if(child->on_click.te_func && child->z >= *z)
            {
                *z = child->z;
                event->te_func = child->on_click.te_func;
                event->data = child->on_click.data;
                event->caller = child->on_click.caller;
                event->te_func_in_func = child->on_click.te_func_in_func;
            }
        }

    }
    return 0;
}

int check_click(int x, int y)
{

    tileless_event te;
    int any_hit = 0;
    int z = 0;
    te.te_func = NULL;
    check_controls(controls, x,CURR_HEIGHT -  y, &te, &z, &any_hit);
    if(te.te_func)
        te.te_func(te.caller, te.data, te.te_func_in_func);
    return any_hit;
}


static int render_control(struct CTRL *ctrl, MATRIX *matrix_hndl)
{

    //log_this(100,"render control\n");
    render_simple_rect(ctrl->box, ctrl->color, matrix_hndl);
    if(ctrl->txt)
    {

        //log_this(100,"render control text %s\n", ctrl->txt->txt);
        GLfloat point_coord[2] = {ctrl->box[0] + ctrl->txt_margin[0], ctrl->box[3] - ctrl->txt_margin[1]};
        GLfloat color[4] = {0,0,0,255};
        //   print_txt(point_coord, color,1,0,300, "Normal text ");
        //   printf("x = %f, y = %f\n", point_coord[0], point_coord[1]);
        GLfloat max_width = ctrl->box[2]-ctrl->box[0] - 2 *ctrl->txt_margin[0]; //here we say that max text width = box width - 2 margins (left and right margin)

        int y = print_txtblock(point_coord, matrix_hndl, color,max_width, ctrl->txt);
        printf("returning y = %d and box[1] = %d\n", y, ctrl->box[1]);
        if(ctrl->box[1] > y)
            ctrl->box[1] = y;

    }    
    
    return 0;
}

int render_controls(struct CTRL *ctrl, MATRIX *matrix_hndl)
{
    int i;
    if(!ctrl->active)
        return 0;

    if(ctrl == incharge)
        matrix_hndl = ctrl->matrix_handler;
    render_control(ctrl, matrix_hndl);
    for (i=0; i < ctrl->logical_family->n_children; i++)
    {

        render_controls(ctrl->logical_family->children[i], matrix_hndl);

    }
    return 0;
}

