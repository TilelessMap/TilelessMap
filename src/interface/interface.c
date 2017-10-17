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
#include "../utils.h"


static inline int32_t max_i(int a, int b)
{
    if (b > a)
        return b;
    else
        return a;
}





int check_screen_size()
{
    
    int character_size, text_size;
    if(CURR_WIDTH > 1000)
    {
        log_this(100,"BIGSCREEN");
        screensize = BIGSCREEN;
        size_factor = 2;
        character_size = 40;
        text_size = 30;
    }
    else if (CURR_WIDTH >500)
    {
        log_this(100,"MIDDLESCREEN");
        screensize = MIDDLESCREEN;
        size_factor = 1.5;
        character_size = 30;
        text_size = 25;
    }
    else
    {
        log_this(100,"SMALLSCREEN");
        screensize = SMALLSCREEN;
        size_factor = 1;
        character_size = 20;
        text_size = 20;
    }
    
    text_font_normal = NULL;
    text_font_bold = NULL;
    char_font = NULL;
        text_font_normal = loadatlas("freesans",NORMAL_TYPE, text_size);
        text_font_bold = loadatlas("freesans",BOLD_TYPE, text_size);
        char_font = loadatlas("freesans",BOLD_TYPE, character_size);
    return 0;

}

int get_parent_origo(struct CTRL *t, GLshort *p)
{
    p[0] = p[1] = 0;

    struct CTRL *parent = t->relatives->parent;
    while (parent)
    {
        p[0] += parent->box[0];
        p[1] += parent->box[1];
        parent = parent->relatives->parent;
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

    if (!(t->n_children == 0))
    {
        log_this(100,"Something strange, there are %d children left\n", t->n_children);
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

    while (t->caller->n_children)
    {
        struct CTRL *child = t->caller->children[t->caller->n_children -1];
        if(child)
        {
            r ++;
            destroy_control(child);
        }
    }
    free(t->caller->children);

    while (t->relatives->n_children)
    {
        struct CTRL *child = t->relatives->children[t->relatives->n_children -1];
        if(child)
        {
            r ++;
            destroy_control(child);
        }
    }
    free(t->relatives->children);
    if(t->caller->parent)
        remove_child(t->caller->parent->caller, t);
    if(t->relatives->parent)
        remove_child(t->relatives->parent->relatives, t);
    destroy_family(t->caller);
    destroy_family(t->relatives);
    if(t->txt && t->txt->txt_info->points)
        destroy_point_list(t->txt->txt_info->points);
    if(t->txt)
        destroy_textblock(t->txt);

    if(t->matrix_handler)
        free(t->matrix_handler);
    
    
    free(t);
    t=NULL;

    return 0;
}

void clone_box(GLshort *box_dest, GLshort *box_src)
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
CTRL* register_control(int type,struct CTRL* spatial_parent,struct CTRL* caller, tileless_event_function click_func, void* onclick_arg, tileless_event_func_in_func func_in_func, GLshort* box, GLfloat* color, TEXTBLOCK* txt, GLshort* txt_margin, int default_active, int z)
{
    struct CTRL *ctrl = st_malloc(sizeof(struct CTRL));
    ctrl->type = type;
    ctrl->active = default_active;
    ctrl->z = z;
    ctrl->caller = init_family(caller);
    ctrl->relatives = init_family(spatial_parent);

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

    ctrl->alignment = 0;
    ctrl->txt = txt;
    clone_box(ctrl->box, box);
    if(spatial_parent)
        add_child(spatial_parent->relatives, ctrl);
    if(caller)
        add_child(caller->caller, ctrl);

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
    multiply_short_array(box_text_margins, size_factor, 2);
    
    GLshort click_size = 30 * size_factor;
    GLshort startx, starty, p[] = {0,0};
    get_top_right(ctrl, p);

    startx = p[0] - 50*size_factor;
    starty = p[1] - 50*size_factor;

    GLshort close_box[] = {startx, starty,startx + click_size,starty + click_size};

    GLfloat close_color[]= {200,100,100,200};
    
    
    TEXTBLOCK *x_txt = init_textblock(1);
    GLfloat fontcolor[] = {0,0,0,255};
    append_2_textblock(x_txt,"X", char_font, fontcolor,0, NEW_STRING, tmp_unicode_txt);
    return register_control(BUTTON, ctrl,ctrl, close_ctrl,NULL,NULL,close_box,close_color,x_txt,box_text_margins, 1,10);

    
    
    
}

int close_ctrl(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);

    struct CTRL *t = (struct CTRL *) ctrl;
    incharge = NULL; //move focus back to map
    destroy_control(t->caller->parent);


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

    log_this(10, "checkcontrol, x = %d, y = %d\n",x,y);
    n_children = ctrl->relatives->n_children;
    for (i=0; i<n_children; i++)
    {
        struct CTRL *child = *(ctrl->relatives->children+i);
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

int check_click(struct  CTRL *controls, int x, int y)
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

        if(ctrl->type == TABLE_CELL)
        {
            short v_marg = ctrl->relatives->parent->txt_margin[1];
         ctrl->box[3] = ctrl->relatives->parent->box[3] - v_marg;  
         ctrl->box[1] = ctrl->relatives->parent->box[1] + v_marg;   
        }
    render_simple_rect(ctrl->box, ctrl->color, matrix_hndl);
    if(ctrl->txt)
    {
        GLfloat point_coord[2] = {ctrl->box[0] + ctrl->txt_margin[0], ctrl->box[3] - ctrl->txt_margin[1]};
        
        unsigned int alignment = ctrl->alignment; 
        float anchor[2]={0,0};
        float displacement[2]={0,0};
        
        
        
        if(alignment & H_CENTER_ALIGNMENT)
        {            
            point_coord[0] = ctrl->box[0] + 0.5 * (ctrl->box[2] - ctrl->box[0]);
            anchor[0] = 0.5;            
        }
        else if(alignment & H_RIGHT_ALIGNMENT)
        {
            point_coord[0] = ctrl->box[2] - ctrl->txt_margin[0];
            anchor[0] = 1;            
        }
            
        else
        {
            point_coord[0] = ctrl->box[0] + ctrl->txt_margin[0];
            anchor[0] = 0;            
        }

       if(alignment & V_CENTER_ALIGNMENT)
       {
            point_coord[1] = ctrl->box[1] + 0.5 * (ctrl->box[3] - ctrl->box[1]);
            anchor[1] = 0.5;            
       }
        else if(alignment & V_TOP_ALIGNMENT)
        {
            point_coord[1] = ctrl->box[3] - ctrl->txt_margin[1];
            anchor[1] = 0;            
        }
        else
        {
            point_coord[1] = ctrl->box[1] + ctrl->txt_margin[1];
            anchor[1] = 1;            
        }
        print_txtblock(point_coord, matrix_hndl, ctrl->txt,anchor, displacement);


    }
    return 0;
}

int render_controls(struct CTRL *ctrl, MATRIX *matrix_hndl)
{
    log_this(10, "entering %s with %p\n",__func__, ctrl);
    int i;
    if(!ctrl->active)
        return 0;

    if(ctrl == incharge)
        matrix_hndl = ctrl->matrix_handler;
    render_control(ctrl, matrix_hndl);
    for (i=0; i < ctrl->relatives->n_children; i++)
    {
        render_controls(ctrl->relatives->children[i], matrix_hndl);
    }
    return 0;
}



int calc_text_widthandheight(const char *txt, ATLAS *font, int *width, int *height)
{
    int w=0, h=0,current_row_height=0, current_row_width=0,pw, ph; 
    int len, i;
    
    //using tmp_unicode_txt here makes it not thread-safe.
    //But it saves a lot of malloc
    
    reset_wc_txt(tmp_unicode_txt);
    add_utf8_2_wc_txt(tmp_unicode_txt,txt);
    
    
    len = strlen(txt);
    uint8_t p;
    for(i=0;i<len;i++)
    {
        p = tmp_unicode_txt->txt[i];
        
        if(p=='\n')
        {
            h+=current_row_height;
            current_row_height = 0;
            w = max_i(w, current_row_width);
            current_row_width = 0;
        }
        else
        {
            ph = font->ch;
            pw = font->metrics[p].ax;
            
            current_row_height = max_i(current_row_height, ph);
            current_row_width+=pw;
        }                    
    }    
    *(height) = h+=current_row_height;
    *(width) = max_i(w, current_row_width);
    return 0;
    
}

int print_controls(CTRL *ctrl,int level)
{
char txt[1024], c, *type_txt;
int i, r;

if (!ctrl)
    ctrl = get_master_control();


if(ctrl->active)
    c = '+';
else
    c = '-';
 
 memset(txt,' ',level);
 r = level;
 txt[r] = c;
 r++;
 
 if(ctrl->type == MASTER)
     type_txt = "MASTER";
 else if(ctrl->type == BOX)
     type_txt = "BOX";
 else if(ctrl->type == BUTTON)
     type_txt = "BUTTON";
 else if(ctrl->type == TEXTBOX)
     type_txt = "TEXTBOX";
 else if(ctrl->type == CHECKBOX)
     type_txt = "CHECKBOX";
 else if(ctrl->type == RADIOBUTTON)
     type_txt = "RADIOBUTTON";
 else if(ctrl->type == TABLE)
     type_txt = "TABLE";
 else if(ctrl->type == TABLE_ROW)
     type_txt = "TABLE_ROW";
 else if(ctrl->type == TABLE_CELL)
     type_txt = "TABLE_CELL";
 else 
     type_txt = "Unknown type";
     
     
 int len = strlen(type_txt);
  //  memcpy(txt+r, type_txt, len);
 
snprintf(txt+r, 1024-r, "%s, %p",type_txt,(void*) ctrl);
r+=len+2+sizeof(void*);
 
 if(ctrl->txt)
 {
     snprintf(txt+r, 1024-r, " - %s",ctrl->txt->txt->txt);
    r+=strlen(ctrl->txt->txt->txt) +3;
 }
 txt[r] = '\0';
 log_this(100,"%s\n", txt);
 
 for (i=0;i<ctrl->relatives->n_children;i++)
 {
  CTRL *child = ctrl->relatives->children[i];
  print_controls(child, level+1);
 }
 
 
 return 0;
     
 
}
