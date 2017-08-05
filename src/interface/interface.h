#ifndef _interface_H
#define _interface_H

#include "../matrix_handling.h"
#include "../events.h"

#define MASTER      0
#define BOX         1
#define BUTTON      2
#define TEXTBOX     3
#define CHECKBOX    4
#define RADIOBUTTON 5
#define RADIOMASTER 6

#define BIGSCREEN       3
#define MIDDLESCREEN    2
#define SMALLSCREEN     1

typedef struct
{
    GLshort minx;
    GLshort maxx;
    GLshort miny;
    GLshort maxy;
} CTRL_BOX;




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
    GLfloat color[4];
    TEXTBLOCK *txt;
    GLshort txt_margin[2]; //distance in pixels from control border where text starts
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
struct CTRL* register_control(int type, struct CTRL *spatial_parent,struct CTRL *logical_parent, tileless_event_function click_func,void *onclick_arg,tileless_event_func_in_func func_in_func, GLshort *box,GLfloat *color,TEXTBLOCK *txt, GLshort *txt_margin, int default_active, int z);


/*Radiobutton*/
//struct CTRL* init_radio(struct CTRL *spatial_parent,struct CTRL *logical_parent, GLshort *box,GLfloat *color,TEXTBLOCK *txt, GLfloat *txt_margin,int txt_size,int default_active, int z);
struct CTRL* init_radio(struct CTRL *spatial_parent,struct CTRL *logical_parent, GLshort *box,GLfloat *color,TEXTBLOCK *txt, GLshort *txt_margin,int default_active, int z);

/*Textbox*/
struct CTRL* init_textbox(struct CTRL *spatial_parent,struct CTRL *logical_parent, GLshort *box,GLfloat *color, GLshort *txt_margin,int default_active, int z);
int add_txt_2_textbox(struct CTRL *ctrl, TEXTBLOCK *txt);

int set_info_layer(void *ctrl, void *val);


//struct CTRL* add_radio_button(struct CTRL *radio_master, GLshort size, GLshort row_dist, int default_active);
int init_matrix_handler(struct CTRL *ctrl, uint8_t vertical_enabled, uint8_t horizontal_enabled, uint8_t zoom_enabled);

struct CTRL* add_radio_button(struct CTRL *radio_master, tileless_event_func_in_func set_unset, GLshort size, GLshort row_dist, int default_active, int set);

int close_ctrl(void *ctrl, void *val, tileless_event_func_in_func func_in_func);

struct CTRL *incharge; //This variable handles what to mov on events. NULL means map, otherwise there will be a ccontrol that will be scrolled or move in other ways




int get_parent_origo(struct CTRL *t, GLshort *p);
int get_bottom_left(struct CTRL *t, GLshort *p);
int get_top_left(struct CTRL *t, GLshort *p);
int get_bottom_right(struct CTRL *t, GLshort *p);
int get_top_right(struct CTRL *t, GLshort *p);

int destroy_control(struct CTRL *t);
int multiply_array(GLshort *a, GLfloat v, GLshort ndims);
uint8_t screensize;

GLfloat size_factor;
int check_screen_size();
int character_size, text_size;

struct CTRL* add_close_button(struct CTRL *ctrl);
#endif


