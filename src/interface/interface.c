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

static uint8_t show_layer_control;
static int create_layers_meny(struct CTRL *spatial_parent, struct CTRL *logical_parent);

static inline int get_parent_origo(struct CTRL *t, GLshort *p)
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


static inline int get_bottom_left(struct CTRL *t, GLshort *p)
{
    
    p[0] = p[1] = 0;
    //get_parent_origo(t, p);
    
    p[0] += t->box[0];
    p[1] += t->box[1];
    
    return 0;  
}

static inline int get_top_left(struct CTRL *t, GLshort *p)
{
    p[0] = p[1] = 0;
    //get_parent_origo(t, p);
    
    p[0] += t->box[0];
    p[1] += t->box[3];
    
    return 0;  
}

static inline int get_bottom_right(struct CTRL *t, GLshort *p)
{
    p[0] = p[1] = 0;
    //get_parent_origo(t, p);
    
    p[0] += t->box[2];
    p[1] += t->box[1];
    
    return 0;  
}

static inline int get_top_right(struct CTRL *t, GLshort *p)
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
    for (i=0;i<len;i++)
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


static int destroy_control(struct CTRL *t)
{
    int i, r=0;
    
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
    free(t->spatial_family->children);
    remove_child(t->logical_family->parent->logical_family, t);
    remove_child(t->spatial_family->parent->spatial_family, t);
    destroy_family(t->logical_family);        
    destroy_family(t->spatial_family);
    if(t->txt)
        destroy_txt(t->txt);
    
    if(t->matrix_handler)
        free(t->matrix_handler);
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

static inline int init_matrix_handler(struct CTRL *ctrl, uint8_t vertical_enabled, uint8_t horizontal_enabled, uint8_t zoom_enabled)
{
  ctrl->matrix_handler = st_malloc(sizeof(MATRIX));  
 ctrl->matrix_handler->vertical_enabled = vertical_enabled;
 ctrl->matrix_handler->horizontal_enabled = horizontal_enabled;
 ctrl->matrix_handler->zoom_enabled = zoom_enabled;
 
 ctrl->matrix_handler->bbox[0] = 0;
 ctrl->matrix_handler->bbox[1] = 0;
 ctrl->matrix_handler->bbox[2] = CURR_WIDTH;
 ctrl->matrix_handler->bbox[3] = CURR_HEIGHT;
    
}


//struct CTRL* register_control(struct CTRL *parent, tileless_event_function click_func,void *onclick_arg, GLshort *box,int default_active)
struct CTRL* register_control(int type, struct CTRL *spatial_parent,struct CTRL *logical_parent, tileless_event_function click_func,void *onclick_arg,tileless_event_func_in_func func_in_func, GLshort *box,GLfloat *color,TEXT *txt, GLfloat *txt_margin,int txt_size, int default_active, int z)
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
    ctrl->txt_size = txt_size;
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


int switch_map_modus(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
     
    struct CTRL *t = (struct CTRL *) ctrl;
    GLfloat *color = t->color;
    if(map_modus)
    {
        
        log_this(100, "remove map_modus");
        color[0] = 200;
        color[1] = 255;
        color[2] = 200;
        color[3] = 220;
        map_modus = 0;
        
        return 0;
    }
    else
    {

        log_this(100, "set map_modus");
        color[0] = 200;
        color[1] = 200;
        color[2] = 200;
        color[3] = 220;
        map_modus = 1;
        
        return 1;
    }

    return 0;
}




int show_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
     
    struct CTRL *t = (struct CTRL *) ctrl;
    uint8_t current_status = *((uint8_t*) t->obj);
   
    create_layers_meny(controls, t);
    
    
    return 0;
}


int hide_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
     
    
    struct CTRL *t = (struct CTRL *) ctrl;
    incharge = NULL; //move focus back to map
    struct CTRL *parent = t->logical_family->parent;
    destroy_control(t->logical_family->parent);
    
    
    return 0;
}




static int check_box(GLshort *box,int x, int y)
{
 /*   if(matrix_hndl)
    {
     GLfloat p1[4], p2[4];
     p1[0]
    }    
    else
    {*/
        if(box[0] < x && box[1] < y && box[2] > x && box[3] > y)
            return 1;
 //   }
    return 0;

}
static int check_controls(struct CTRL *ctrl, int x, int y, tileless_event *event, int *z)
{

    int i, n_children;
    int max_z = *z;
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
            
            check_controls(child, x, y, event, z);
            
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
    int z = 0;
    te.te_func = NULL;
    check_controls(controls, x,CURR_HEIGHT -  y, &te, &z);
    if(te.te_func)
        te.te_func(te.caller, te.data, te.te_func_in_func);
    return 0;
}


static int render_control(struct CTRL *ctrl, MATRIX *matrix_hndl)
{
 
    //log_this(100,"render control\n");
    render_simple_rect(ctrl->box, ctrl->color, matrix_hndl);  
    if(ctrl->txt)
    {
     
    //log_this(100,"render control text %s\n", ctrl->txt->txt);
        GLfloat point_coord[2] = {ctrl->box[0] + ctrl->txt_margin[0], ctrl->box[1] + ctrl->txt_margin[1]};
        GLfloat color[4] = {0,0,0,255};
     //   print_txt(point_coord, color,1,0,300, "Normal text ");
     //   printf("x = %f, y = %f\n", point_coord[0], point_coord[1]);
        GLfloat max_width = ctrl->box[2]-ctrl->box[0] -2 *ctrl->txt_margin[0]; //here we say that max text width = box width - 2 margins (left and right margin)
        print_txt(point_coord,matrix_hndl, color,2, 1,max_width, ctrl->txt->txt);   
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
 for (i=0;i < ctrl->logical_family->n_children;i++)
 {
     
     render_controls(ctrl->logical_family->children[i], matrix_hndl);
     
 }   
return 0;    
}


static int set_layer_visibility(void *ctrl, void *val,tileless_event_func_in_func func_in_func)
{
    struct CTRL *t = (struct CTRL *) ctrl;
    LAYER_RUNTIME *oneLayer = (LAYER_RUNTIME*) t->obj;
    
    
    uint8_t current_status = oneLayer->visible;
    
    if(current_status)
    {
        if(t->txt)
        {
            destroy_txt(t->txt);
            t->txt = 0;
        }
        oneLayer->visible = 0;     
    }
    else
    {
        TEXT *txt = init_txt(5);
        add_txt(txt, "X");
        t->txt=txt;
        oneLayer->visible = 1;
    }
    
    
  return 0;  
}

int set_info_layer(void *ctrl, void *val)
{
 
    struct CTRL *t = (struct CTRL *) ctrl;
    
    LAYER_RUNTIME *oneLayer = (LAYER_RUNTIME*) t->obj;
    
    oneLayer->info_active = *((int*) val);
    
    
    
  return 0;  
}
static int create_layers_meny(struct CTRL *spatial_parent, struct CTRL *logical_parent)
{
    
    int i;
    TEXT *txt, *x_txt;
    
    GLshort box_start_x = 30, box_start_y = 30;
    GLshort box_height = 50, box_width = CURR_WIDTH - 2 * box_start_y;
    GLshort box[] = {box_start_x,CURR_HEIGHT - box_start_y - box_height,box_start_x + box_width,CURR_HEIGHT - box_start_y};
    
    GLshort radio_width = 100;
    GLshort check_width = 30;
    GLshort  text_width = 300;
    
    GLshort row_height = 30;
    GLshort row_dist = 20;
    
    GLfloat color[]={200,200,200,100};
    GLfloat txt_box_color[]={0,0,0,0};
    GLfloat click_box_color[]={255,255,255,255};
    GLfloat radio_box_color[]={200,255,200,255};
    
    GLfloat margins[] = {10,10};
    GLfloat box_text_margins[] = {3,3};
    
    struct CTRL *layers_meny = register_control(BOX,  spatial_parent,logical_parent, NULL,NULL,NULL,box,color, NULL,NULL, 0,1,0);
//    printf("layers_menu=%p\n", layers_meny);
    GLshort startx, starty, p[] = {0,0};
    GLshort click_box_width = 30;
    GLshort click_box_height = 30;
    GLshort text_box_width = 300;
    GLshort text_box_height = 30;
    GLshort col_dist = 30;
    
    struct CTRL *new_ctrl;
    get_top_left(layers_meny, p);
    
    startx = box[0] + col_dist;
    starty = box[3] - 2 * (row_dist + row_height);
    
    init_matrix_handler(layers_meny, 1, 0, 0);
    incharge = layers_meny;
    
    
    matrixFromBBOX(layers_meny->matrix_handler);
                   
    
    GLshort radio_box[] = {startx + check_width + col_dist + text_width + col_dist,box[1], startx + check_width + col_dist + text_width + col_dist + radio_width,box[1]};
    struct CTRL *radio_master = init_radio( layers_meny,layers_meny,radio_box,radio_box_color,  NULL, NULL, 0, 1, 0 );
    LAYER_RUNTIME *oneLayer;
    for (i=0; i<nLayers; i++)
    {
        
        GLshort rowstart_x = startx;
        GLshort rowstart_y = starty - i * (row_dist + row_height);
        
        oneLayer = layerRuntime + i;
        
        txt = init_txt(20);
        
        add_txt(txt, oneLayer->name);
            
        GLshort click_box[] = {rowstart_x, rowstart_y,rowstart_x + check_width,rowstart_y + row_height};
        GLshort text_box[] = {rowstart_x + check_width + col_dist, rowstart_y,rowstart_x + check_width + col_dist + text_width,rowstart_y + row_height};
            
        if(oneLayer->visible)
        {
            x_txt = init_txt(5);
            
            add_txt(x_txt, "X");
            new_ctrl = register_control(CHECKBOX, layers_meny,layers_meny, set_layer_visibility ,NULL,NULL,click_box,click_box_color,x_txt,box_text_margins, 3,1,10);
        }
        else
            new_ctrl = register_control(CHECKBOX,layers_meny,layers_meny, set_layer_visibility,NULL,NULL,click_box,click_box_color,NULL,box_text_margins, 3,1,10);
            
        register_control(TEXTBOX, layers_meny,new_ctrl, NULL,NULL,NULL,text_box,txt_box_color,txt,margins, 2,1,10); //register text label and set checkbox as logical parent
        new_ctrl->obj = (void*) oneLayer;
        
        new_ctrl = add_radio_button(radio_master,set_info_layer, 30, 20, 1, oneLayer->info_active);
        new_ctrl->obj = (void*) oneLayer;
    }
    
    layers_meny->box[1] -= i * (row_dist + row_height);
    radio_master->box[1] -= i * (row_dist + row_height);
    
    //create close button
    
    initialBBOX(CURR_WIDTH/2, CURR_HEIGHT/2, CURR_WIDTH, layers_meny->matrix_handler);
    
    get_top_right(layers_meny, p);
    
    startx = p[0] - 50;
    starty = p[1] - 50;
    
    GLshort close_box[] = {startx, starty,startx + click_box_width,starty + row_height};
    GLfloat close_color[]={200,100,100,200};
    x_txt = init_txt(5); 
    add_txt(x_txt, "X");
    register_control(CHECKBOX, layers_meny,layers_meny, hide_layer_selecter,NULL,NULL,close_box,close_color,x_txt,box_text_margins, 3,1,10); //register text label and set checkbox as logical parent

    return 0;
}


int init_controls()
{
    TEXT *txt;
    GLshort box[] = {0,0,0,0};
 //   controls =  register_control(NULL,NULL, NULL,NULL, box,1);
    controls =  register_control(MASTER, NULL,NULL,NULL,NULL, NULL, box,NULL, NULL, NULL,0,1,0);
 //   GLshort box2[] = {5,75,300,225};
  //  register_control(controls, switch_map_modus, NULL,box2, 1);    
    
    
    txt = init_txt(5);
    add_txt(txt,"INFO");
    GLshort box2[] = {5,75,90,115};
    GLfloat txt_margin[] = {10,10};
    GLfloat color[]={200,200,200,100};
    register_control(BUTTON, controls,controls, switch_map_modus,NULL,NULL,box2,color, txt,txt_margin, 2,1,1);
    
    show_layer_control = 0;
    txt = init_txt(7);
    add_txt(txt,"LAYERS");
    GLshort box3[] = {5,30,150,70};
    GLfloat txt_margin2[] = {10,10};
    GLfloat color2[]={255,200,200,100};
    struct CTRL *layers_button = register_control(BUTTON, controls,controls, show_layer_selecter,NULL,NULL, box3,color2, txt,txt_margin2, 2,1,1);
    layers_button->obj = &show_layer_control; // we register the variable show_layer_control to the button so we can get the status from there
    
   // create_layers_meny(controls, layers_button);
    
    
    
    return 0;
}
