#ifndef _interface_H
#define _interface_H

#include "info.h"

typedef struct
{
    GLshort minx;
    GLshort maxx;
    GLshort miny;
    GLshort maxy;    
}CTRL_BOX;


typedef int (*tileless_event)(void*);

/*
typedef struct
{
    tileless_event cb;
    void *data;
}TL_EV;
*/
struct CTRL
{
    uint8_t id;
    uint8_t active;
    GLshort box[4];
    struct CTRL *calling_ctrl;// a child can be a fullscreen started from a small parent box.
    struct CTRL *parent; //parent have to contain child
    struct CTRL **children;
    int n_children;
    int max_children;
    tileless_event on_click;
    void *onclick_data;
};

struct CTRL *controls;


int check_click(int x, int y);


int parent_add_child(struct CTRL *parent, struct CTRL *child);
int init_controls();
#endif
