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


#include "../theclient.h"
#include "../mem.h"
#include "interface.h"
#include "../text/fonts.h"
#include "../utils.h"

static uint8_t show_layer_control;
static int create_layers_meny(struct CTRL *spatial_parent, struct CTRL *logical_parent);
static int switch_map_modus(void *ctrl, void *val, tileless_event_func_in_func func_in_func);
static int show_menu(void *ctrl, void *val, tileless_event_func_in_func func_in_func);
static int show_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func);
static int hide_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func);
static int set_layer_visibility(void *ctrl, void *val,tileless_event_func_in_func func_in_func);
static int create_layers_meny(struct CTRL *spatial_parent, struct CTRL *logical_parent);
static struct CTRL* add_tileless_info(struct CTRL *ctrl);




static struct CTRL *controls;



struct CTRL* init_controls()
{
    GLshort box[] = {0,0,0,0}; //This is just a dummy box that we use for the master control.
    controls =  register_control(MASTER, NULL,NULL,NULL,NULL, NULL, box,NULL, NULL, NULL,7,0);


    return controls;
}

int add_default_controls()
{


    TEXTBLOCK *txt;
    ATLAS *font = loadatlas("freesans",BOLD_TYPE, 25);

    //This is the margin (left and bottom) that will be used for the 2 buttons defined below
    GLshort txt_margin[] = {20,20};
    multiply_short_array(txt_margin, size_factor, 2);

    GLfloat color[]= {0,0,0,100};
    
    
    GLfloat table_border[]= {0,0,0,255};
    GLfloat row_color[]= {255,200,220,255};
    GLfloat fontcolor[]= {0,0,0,255};

    /**** define start_button ******/
    GLshort start_box[] = {5,5,100,100};

    
   // multiply_short_array(start_box, size_factor, 4);

    txt = init_textblock();
    append_2_textblock(txt,">>", font, fontcolor,0, NEW_STRING, tmp_unicode_txt);
    
    

    CTRL *start = register_control(BUTTON, controls,controls, show_menu,NULL,NULL,start_box,color, txt,txt_margin, 7,0.01);

    
    start->alignment = H_CENTER_ALIGNMENT|V_CENTER_ALIGNMENT;
    
    
    GLshort table_box[] = {50,50,650,650};
    
    
    GLshort table_margins[] = {3,3};
    CTRL *row;
    CTRL *cell;
    CTRL *table = add_table(start, start, table_border,table_box,table_margins, NULL);
    table->active = 0;
    short column_widths[] = {300};
    /**** define info-button at main screen ******/
    short cell_margins[] = {0, 0};
    row = add_row(table, row_color, 1, column_widths);
    cell = add_cell(row, "INFO", row_color, fontcolor, cell_margins, switch_map_modus, NULL, 40);
    
    show_layer_control = 0;
    row = add_row(table, row_color, 1, column_widths);
    cell = add_cell(row, "LAYERS", row_color, fontcolor, cell_margins, show_layer_selecter, NULL, 40);

    
    /*
    GLshort info_box[] = {5,5,135,65};
    multiply_short_array(info_box, size_factor, 4);
    

    txt = init_textblock();
    append_2_textblock(txt,"INFO", font, fontcolor,0, NEW_STRING, tmp_unicode_txt);

    CTRL *info = register_control(BUTTON, start,start, switch_map_modus,NULL,NULL,info_box,color, txt,txt_margin, 0,1);
    info->alignment = H_CENTER_ALIGNMENT|V_CENTER_ALIGNMENT;
*/
    /**** define layer menu -button at main screen ******/
   /* GLshort layers_box[] = {5,85,155,155};
    multiply_short_array(layers_box, size_factor, 4);


    txt = init_textblock();
    append_2_textblock(txt,"LAYERS", font, fontcolor,0,NEW_STRING, tmp_unicode_txt);

    CTRL *layers_button = register_control(BUTTON, start,start, show_layer_selecter,NULL,NULL, layers_box,color, txt,txt_margin, 0,1);
    layers_button->obj = &show_layer_control; // we register the variable show_layer_control to the button so we can get the status from there
    layers_button->alignment = H_CENTER_ALIGNMENT|V_CENTER_ALIGNMENT;
*/
    add_tileless_info(controls);

    return 0;
}


CTRL* get_master_control()
{
    return controls;
}

static int switch_map_modus(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);
    struct CTRL *t = (struct CTRL *) ctrl;
    GLfloat *color = t->color;
    if(map_modus)
    {

        log_this(100, "remove map_modus");
        color[0] = 150;
        color[1] = 255;
        color[2] = 150;
        color[3] = 255;
        map_modus = 0;

        return 0;
    }
    else
    {

        log_this(100, "set map_modus");
        color[0] = 200;
        color[1] = 255;
        color[2] = 200;
        color[3] = 200;
        map_modus = 1;
        infoRenderLayer->visible = 0;
        return 1;
    }

    return 0;
}

static int show_menu(void* ctrl, void* val, tileless_event_func_in_func func_in_func)
{
    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);
    struct CTRL *t = (struct CTRL *) ctrl;
    int n_children = t->relatives->n_children;
    int i; 
    t->active = 1;
    for (i=0;i<n_children;i++)
    {
     t->relatives->children[i]->active = 7; 
    }
    
    return 0;
}


static int show_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);
    struct CTRL *t = (struct CTRL *) ctrl;

    create_layers_meny(t, t)
    
    t->relatives->parent->relatives->parent->active = 0; 
    
    

    return 0;
}

static int hide_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);

    struct CTRL *t = (struct CTRL *) ctrl;
    incharge = NULL; //move focus back to map
    destroy_control(t->caller->parent);


    return 0;
}

static int set_layer_visibility(void *ctrl, void *val,tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);
    struct CTRL *t = (struct CTRL *) ctrl;
    LAYER_RUNTIME *oneLayer = (LAYER_RUNTIME*) t->obj;

    GLfloat fontcolor[]= {0,0,0,255};
    ATLAS *font = loadatlas("freesans",BOLD_TYPE, 40);
    uint8_t current_status = oneLayer->visible;

    if(current_status)
    {
        if(t->txt)
        {
            destroy_textblock(t->txt);
            t->txt = 0;
        }
        oneLayer->visible = 0;
    }
    else
    {
        //TEXT *txt = init_txt(5);
        TEXTBLOCK *txt = init_textblock();
        append_2_textblock(txt,"X", font, fontcolor,0,NEW_STRING, tmp_unicode_txt);
//        add_txt(txt, "X");
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
    ATLAS *font = loadatlas("freesans",BOLD_TYPE, 20);
    int i;
    TEXTBLOCK *txt, *x_txt;

    GLshort box_start_x = 10 * size_factor, box_start_y = 10 * size_factor;
    GLshort box_height = 50, box_width = CURR_WIDTH - 2 * box_start_y;
    GLshort box[] = {box_start_x,CURR_HEIGHT - box_start_y - box_height,box_start_x + box_width,CURR_HEIGHT - box_start_y};

    GLshort radio_width = 50 * size_factor;
    GLshort  text_width = 250 * size_factor;
    GLshort click_size = 30 * size_factor;
    GLshort col_dist = 20 * size_factor;

    GLshort row_dist = 20 * size_factor;

    GLshort row_height = click_size + row_dist;
    GLfloat color[]= {200,200,200,100};
    GLfloat txt_box_color[]= {0,0,0,0};
    GLfloat click_box_color[]= {255,255,255,255};
    GLfloat radio_box_color[]= {200,255,200,50};

    GLshort margins[] = {3,3};
    GLshort box_text_margins[] = {4,4};



    GLfloat fontcolor[]= {0,0,0,255};

    struct CTRL *layers_meny = register_control(BOX,  spatial_parent,logical_parent, NULL,NULL,NULL,box,color, NULL,NULL, 7,0);
    print_controls(NULL,0);
//    printf("layers_menu=%p\n", layers_meny);
    GLshort startx, starty, p[] = {0,0};


    multiply_short_array(box_text_margins, size_factor, 2);

    struct CTRL *new_ctrl;
    get_top_left(layers_meny, p);

    startx = box[0] + col_dist;
    starty = box[3] - 2 * row_height;

    init_matrix_handler(layers_meny, 1, 0, 0);
    matrixFromBBOX(layers_meny->matrix_handler);
    incharge = layers_meny;


    GLshort radiostart_x = startx + click_size + col_dist + text_width + col_dist;

    GLshort radio_box[] = {radiostart_x,starty, radiostart_x + radio_width,starty+row_dist};
    struct CTRL *radio_master = init_radio( layers_meny,layers_meny,radio_box,radio_box_color,  NULL, NULL,  7, 0 );
    LAYER_RUNTIME *oneLayer;

    for (i=0; i<global_layers->nlayers; i++)
    {

        GLshort rowstart_x = startx;
        GLshort rowstart_y = starty - i * row_height;

        oneLayer = global_layers->layers + i;


        txt = init_textblock();
        append_2_textblock(txt,oneLayer->title, font, fontcolor,0, NEW_STRING, tmp_unicode_txt);


        GLshort click_box[] = {rowstart_x, rowstart_y-click_size,rowstart_x + click_size,rowstart_y};
        GLshort text_box[] = {rowstart_x + click_size + col_dist, rowstart_y - click_size,rowstart_x + click_size + col_dist + text_width,rowstart_y};

        if(oneLayer->visible)
        {
            x_txt = init_textblock();
            append_2_textblock(x_txt,"X", font, fontcolor,0, NEW_STRING, tmp_unicode_txt);

            new_ctrl = register_control(CHECKBOX, layers_meny,layers_meny, set_layer_visibility,NULL,NULL,click_box,click_box_color,x_txt,box_text_margins, 7,10);
            new_ctrl->alignment = V_CENTER_ALIGNMENT | H_CENTER_ALIGNMENT;
        }
        else
            new_ctrl = register_control(CHECKBOX,layers_meny,layers_meny, set_layer_visibility,NULL,NULL,click_box,click_box_color,NULL,box_text_margins, 7,10);

        register_control(TEXTBOX, layers_meny,new_ctrl, NULL,NULL,NULL,text_box,txt_box_color,txt,margins, 7,10); //register text label and set checkbox as logical parent
        new_ctrl->obj = (void*) oneLayer;

        new_ctrl = add_radio_button(radio_master,set_info_layer, click_size, row_dist, 7, oneLayer->info_active);
        new_ctrl->obj = (void*) oneLayer;
    }

    layers_meny->box[1] = starty - i * row_height;
    radio_master->box[1] = starty -  i * row_height;

    //create close button

    //  initialBBOX(CURR_WIDTH/2, CURR_HEIGHT/2, CURR_WIDTH, layers_meny->matrix_handler);

    get_top_right(layers_meny, p);

    startx = p[0] - 50*size_factor;
    starty = p[1] - 50*size_factor;

    GLshort close_box[] = {startx, starty,startx + click_size,starty + click_size};


    GLfloat close_color[]= {200,100,100,200};

    x_txt = init_textblock();
    append_2_textblock(x_txt,"X", font, fontcolor,0, NEW_STRING, tmp_unicode_txt);
    CTRL *cb = register_control(CHECKBOX, layers_meny,layers_meny, hide_layer_selecter,NULL,NULL,close_box,close_color,x_txt,box_text_margins, 7,10); //register text label and set checkbox as logical parent
    cb->alignment = V_CENTER_ALIGNMENT|H_CENTER_ALIGNMENT;


    return 0;
}




int set_info_txt(void *ctrl, void *page_p, tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) page_p,func_in_func);

    struct CTRL *t = (struct CTRL *) ctrl;
    int page = *((int*) page_p);
    ATLAS *font;

    GLfloat fontcolor[] = {0,0,0,255};
    sqlite3_stmt *preparedinfo;
    char *sql = "select txt, text_size, bold, link_to_page from tilelessmap_info where page = ? order by orderby;";
    check_sql(sql);
    int rc = sqlite3_prepare_v2(projectDB, sql, -1, &preparedinfo, 0);

    if (rc != SQLITE_OK ) {
        log_this(100, "SQL error in %s\n",sql );
        sqlite3_close(projectDB);
        return 1;
    }


    sqlite3_bind_int(preparedinfo, 1,page);

    if(t->txt)
        destroy_textblock(t->txt);

    TEXTBLOCK *tb = init_textblock();

    while(sqlite3_step(preparedinfo) ==  SQLITE_ROW)
    {

        const unsigned char *txt = sqlite3_column_text(preparedinfo, 0);
        int bold = sqlite3_column_int(preparedinfo, 2);
        //      int link_to_page = sqlite3_column_int(preparedinfo, 3);

        if(bold)
            font = loadatlas("freesans",BOLD_TYPE, 12);
        else
            font = loadatlas("freesans",NORMAL_TYPE, 12);

        printf("txt = %s\n", (char*)txt);
        append_2_textblock(tb, (char*)txt, font, fontcolor,0, NEW_STRING, tmp_unicode_txt);

        append_2_textblock(tb," \n ", font, fontcolor,0, APPENDING_STRING, tmp_unicode_txt);

    }


    t->txt = tb;

    sqlite3_finalize(preparedinfo);

    return 0;
}

int init_show_info(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);

    struct CTRL *t = (struct CTRL *) ctrl;


    GLshort box_start_x = 10 * size_factor, box_start_y = 10 * size_factor;

    GLshort box_height = 1000, box_width = CURR_WIDTH - 2 * box_start_y;

    GLshort box[] = {box_start_x,CURR_HEIGHT - box_start_y - box_height,box_start_x + box_width,CURR_HEIGHT - box_start_y};


    GLfloat color[]= {255,255,255,220};



    GLshort box_text_margins[] = {4,4};
    multiply_short_array(box_text_margins, size_factor, 2);



    struct CTRL *info_box = register_control(TEXTBOX, controls,t, NULL,NULL,NULL,box,color, NULL,box_text_margins, 7,20);


    add_close_button(info_box);
    int page = 1;
    set_info_txt(info_box, &page, NULL);

    init_matrix_handler(info_box, 1, 0, 0);
    matrixFromBBOX(info_box->matrix_handler);

    incharge = info_box;
    return 0;

}


struct CTRL* add_tileless_info(struct CTRL *ctrl)
{
    if (!check_layer((const unsigned char*) "main",(const unsigned char*) "tilelessmap_info"))
        return 0;
    ATLAS *font = loadatlas("freesans",BOLD_TYPE, 40);
    int page = 1;
    GLshort box_text_margins[] = {20,10};
    multiply_short_array(box_text_margins, size_factor, 2);

    GLshort click_size = 60 * size_factor;
    GLshort startx, starty, p[] = {0,0};

    p[0] = CURR_WIDTH;

    startx = p[0] - 80*size_factor;
    starty = p[1] + 20*size_factor;

    GLshort info_box[] = {startx, starty,startx + click_size,starty + click_size};

    GLfloat info_color[]= {100,200, 100,200};
    GLfloat fontcolor[] = {0,0,0,255};

    TEXTBLOCK *x_txt = init_textblock();
    append_2_textblock(x_txt,"i", font, fontcolor,0, NEW_STRING, tmp_unicode_txt);
    return register_control(BUTTON, ctrl,ctrl, init_show_info,&page,NULL,info_box,info_color,x_txt,box_text_margins, 7,10);



}


