#ifndef _interface_H
#define _interface_H

#include "../matrix_handling.h"


#define MASTER      0
#define BOX         1
#define BUTTON      2
#define TEXTBOX     3
#define CHECKBOX    4
#define RADIOBUTTON 5
#define RADIOMASTER 6



typedef struct
{
    GLshort minx;
    GLshort maxx;
    GLshort miny;
    GLshort maxy;
} CTRL_BOX;


typedef int (*tileless_event_func_in_func)(void *ctrl, void*);

typedef int (*tileless_event_function)(void *ctrl, void*, tileless_event_func_in_func func_in_func);



/*
typedef struct
{
    tileless_event cb;
    void *data;
}TL_EV;
*/

typedef struct
{
    struct CTRL *parent;
    struct CTRL **children;
    int n_children;
    int max_children;

} RELATIONS;


typedef struct
{
    void *caller; //caller controll
    void *data;
    tileless_event_function te_func;
    tileless_event_func_in_func te_func_in_func;

} tileless_event;

struct CTRL
{
    uint8_t id;
    uint8_t active;
    GLshort box[4];
    RELATIONS *logical_family;// a child can be a fullscreen started from a small parent box.
    RELATIONS *spatial_family;
    tileless_event on_click;
    GLfloat color[4];
    TEXT *txt;
    GLfloat txt_margin[2]; //distance in pixels from control border where text starts
    int txt_size; //text size, or more correct index of atlas to use (1,2 or 3)
    void *obj; //used to connect a layer for instance to a control
    int z; //used both for rendering, but more importingly to decide which click event wins if many are triggered
    MATRIX *matrix_handler;
    int type;
};



struct CTRL *controls;


int check_click(int x, int y);


int parent_add_child(struct CTRL *parent, struct CTRL *child);
int init_controls();
int render_controls(struct CTRL *ctrl, MATRIX *matrix_hndl);
struct CTRL* register_control(int type, struct CTRL *spatial_parent,struct CTRL *logical_parent, tileless_event_function click_func,void *onclick_arg,tileless_event_func_in_func func_in_func, GLshort *box,GLfloat *color,TEXT *txt, GLfloat *txt_margin,int txt_size, int default_active, int z);



struct CTRL* init_radio(struct CTRL *spatial_parent,struct CTRL *logical_parent, GLshort *box,GLfloat *color,TEXT *txt, GLfloat *txt_margin,int txt_size,int default_active, int z);

int set_info_layer(void *ctrl, void *val);


//struct CTRL* add_radio_button(struct CTRL *radio_master, GLshort size, GLshort row_dist, int default_active);


struct CTRL* add_radio_button(struct CTRL *radio_master, tileless_event_func_in_func set_unset, GLshort size, GLshort row_dist, int default_active, int set);



struct CTRL *incharge; //This variable handles what to mov on events. NULL means map, otherwise there will be a ccontrol that will be scrolled or move in other ways


#endif


