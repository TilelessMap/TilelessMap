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
#ifndef _interface_H
#define _interface_H

#include "../structures.h"
#include "../text/text.h"
#include "../handle_input/matrix_handling.h"
#include "../text/fonts.h"


#define MASTER      0
#define BOX         1
#define BUTTON      2
#define TEXTBOX     3
#define CHECKBOX    4
#define RADIOBUTTON 5
#define RADIOMASTER 6
#define TABLE 7
#define TABLE_ROW 8
#define TABLE_CELL 9

#define BIGSCREEN       3
#define MIDDLESCREEN    2
#define SMALLSCREEN     1

#define DEFAULT_TXT_MARGIN 10

#define ACTIVE 1
#define CHECK_CLICK 2
#define RENDER 4

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

typedef struct CTRL CTRL;

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

typedef struct
{
    short max_children;
    short *widths_list;
    short *heights_list;
    short *x_list;
    short *y_list;
} CTRL_CHILD_CONSTRINTS;

struct CTRL
{
    char *id;
    uint8_t active; 
    GLshort box[4];
    RELATIONS *caller;// a child can be a fullscreen started from a small parent box.
    RELATIONS *relatives;
    tileless_event on_click;
    GLfloat color[4];
    TEXTBLOCK *txt;
    GLshort txt_margin[2]; //distance in pixels from control border where text starts
    uint8_t alignment;
    int txt_size; //text size, or more correct index of atlas to use (1,2 or 3)
    void *obj; //used to connect a layer for instance to a control
    int z; //used both for rendering, but more importingly to decide which click event wins if many are triggered
    MATRIX *matrix_handler;
    int type;
    CTRL_CHILD_CONSTRINTS *child_constriants;
};

int show_timing_yes;
int add_timing_info(struct timeval tv,int total_points,int n_lines,int n_polys,int n_tri);

int set_ctrl_id(CTRL *ctrl, char *id);

int check_click(struct CTRL *controls, int x, int y);


int parent_add_child(struct CTRL *parent, struct CTRL *child);
CTRL* init_controls();
int add_default_controls();
CTRL* get_master_control();
int render_controls(struct CTRL *ctrl, MATRIX *matrix_hndl);
int ctrl_add_onclick(CTRL *ctrl, tileless_event_function click_func, void* onclick_arg);
struct CTRL* register_control(int type, struct CTRL *spatial_parent,struct CTRL *caller, tileless_event_function click_func,void *onclick_arg,tileless_event_func_in_func func_in_func, GLshort *box,GLfloat *color,TEXTBLOCK *txt, GLshort *txt_margin, int default_active, int z);

struct CTRL* add_close_button(struct CTRL *ctrl);
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


void clone_box(GLshort *box_dest, GLshort *box_src);

int get_parent_origo(struct CTRL *t, GLshort *p);
int get_bottom_left(struct CTRL *t, GLshort *p);
int get_top_left(struct CTRL *t, GLshort *p);
int get_bottom_right(struct CTRL *t, GLshort *p);
int get_top_right(struct CTRL *t, GLshort *p);

int destroy_control(struct CTRL *t);
int check_screen_size();
ATLAS *text_font_normal,*text_font_bold, *char_font;
//CTRL* add_button(struct CTRL* caller, struct CTRL* spatial_parent, GLshort box_in[],const char *txt, tileless_event_function click_func, GLfloat* color,int font_size,short *txt_margin, int default_active);

CTRL* add_button(struct CTRL* caller, struct CTRL* spatial_parent, GLshort box_in[],const char *txt, tileless_event_function click_func,void *val, GLfloat* color,int font_size,short *txt_margin, int default_active);


int calc_text_widthandheight(const char *txt, ATLAS *font, int *width, int *height);

CTRL* add_table(struct CTRL *caller, struct CTRL *spatial_parent, GLfloat *color, short box[4],short *margin, tileless_event_function click_func);

CTRL* add_row(struct CTRL *spatial_parent, GLfloat *color, short ncols, short *column_widths);

CTRL* add_cell(struct CTRL *spatial_parent,char *txt, GLfloat *color,GLfloat *font_color, short *margin, tileless_event_function click_func,void *val, int font_size);
int print_controls(CTRL *ctrl,int level);
int destroy_control(struct CTRL *t);
#endif


