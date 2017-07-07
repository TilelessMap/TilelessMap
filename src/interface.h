#ifndef _interface_H
#define _interface_H

#include "info.h"

typedef struct
{
    GLshort minx;
    GLshort maxx;
    GLshort miny;
    GLshort maxy;
} CTRL_BOX;


typedef int (*tileless_event)(void *ctrl, void*);

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



struct CTRL
{
    uint8_t id;
    uint8_t active;
    GLshort box[4];
    RELATIONS *logical_family;// a child can be a fullscreen started from a small parent box.
    RELATIONS *spatial_family;
    tileless_event on_click;
    void *onclick_data;
    GLfloat color[4];
    TEXT *txt;
    GLfloat txt_margin[2]; //distance in pixels from control border where text starts
    int txt_size; //text size, or more correct index of atlas to use (1,2 or 3)
    void *obj; //used to connect a layer for instance to a control
};

struct CTRL *controls;


int check_click(int x, int y);


int parent_add_child(struct CTRL *parent, struct CTRL *child);
int init_controls();
int render_controls(struct CTRL *ctrl);

#endif


