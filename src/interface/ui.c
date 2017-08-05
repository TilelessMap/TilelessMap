
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
static int switch_map_modus(void *ctrl, void *val, tileless_event_func_in_func func_in_func);
static int show_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func);
static int hide_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func);
static int set_layer_visibility(void *ctrl, void *val,tileless_event_func_in_func func_in_func);
static int create_layers_meny(struct CTRL *spatial_parent, struct CTRL *logical_parent);
static struct CTRL* add_tileless_info(struct CTRL *ctrl);

/**********************************************
 * This function must be present, and it defines
 * the buttons and labels and things
 * that apears on the main screen
 * *******************************************/
int init_controls()
{
    TEXTBLOCK *txt;
    GLshort box[] = {0,0,0,0}; //This is just a dummy box that we use for the master control.

    controls =  register_control(MASTER, NULL,NULL,NULL,NULL, NULL, box,NULL, NULL, NULL,1,0);

    //This is the margin (left and bottom) that will be used for the 2 buttons defined below
    GLshort txt_margin[] = {20,20};
    multiply_array(txt_margin, size_factor, 2);

    GLfloat color[]= {200,200,200,130};

    /**** define info-button at main screen ******/
    GLshort info_box[] = {5,5,135,65};
    multiply_array(info_box, size_factor, 4);

    txt = init_textblock(1);
    append_2_textblock(txt,"INFO", fonts[0]->fss->fs[character_size].bold);

    register_control(BUTTON, controls,controls, switch_map_modus,NULL,NULL,info_box,color, txt,txt_margin, 1,1);


    /**** define layer menu -button at main screen ******/
    GLshort layers_box[] = {5,85,155,155};
    multiply_array(layers_box, size_factor, 4);

    show_layer_control = 0;

    txt = init_textblock(1);
    append_2_textblock(txt,"LAYERS", fonts[0]->fss->fs[character_size].bold);

    struct CTRL *layers_button = register_control(BUTTON, controls,controls, show_layer_selecter,NULL,NULL, layers_box,color, txt,txt_margin, 1,1);
    layers_button->obj = &show_layer_control; // we register the variable show_layer_control to the button so we can get the status from there
    add_tileless_info(controls);
    return 0;
}




static int switch_map_modus(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);
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
        color[3] = 130;
        map_modus = 1;
        infoRenderLayer->visible = 0;
        return 1;
    }

    return 0;
}


static int show_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);
    struct CTRL *t = (struct CTRL *) ctrl;

    create_layers_meny(controls, t);


    return 0;
}

static int hide_layer_selecter(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);

    struct CTRL *t = (struct CTRL *) ctrl;
    incharge = NULL; //move focus back to map
    destroy_control(t->logical_family->parent);


    return 0;
}

static int set_layer_visibility(void *ctrl, void *val,tileless_event_func_in_func func_in_func)
{

    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) val,func_in_func);
    struct CTRL *t = (struct CTRL *) ctrl;
    LAYER_RUNTIME *oneLayer = (LAYER_RUNTIME*) t->obj;


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
        TEXTBLOCK *txt = init_textblock(1);
        append_2_textblock(txt,"X", fonts[0]->fss->fs[character_size].bold);
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




    struct CTRL *layers_meny = register_control(BOX,  spatial_parent,logical_parent, NULL,NULL,NULL,box,color, NULL,NULL, 1,0);
//    printf("layers_menu=%p\n", layers_meny);
    GLshort startx, starty, p[] = {0,0};


    multiply_array(box_text_margins, size_factor, 2);

    struct CTRL *new_ctrl;
    get_top_left(layers_meny, p);

    startx = box[0] + col_dist;
    starty = box[3] - 2 * row_height;

    init_matrix_handler(layers_meny, 1, 0, 0);
    incharge = layers_meny;


    matrixFromBBOX(layers_meny->matrix_handler);

    GLshort radiostart_x = startx + click_size + col_dist + text_width + col_dist;

    GLshort radio_box[] = {radiostart_x,starty, radiostart_x + radio_width,starty+row_dist};
    struct CTRL *radio_master = init_radio( layers_meny,layers_meny,radio_box,radio_box_color,  NULL, NULL,  1, 0 );
    LAYER_RUNTIME *oneLayer;
    for (i=0; i<nLayers; i++)
    {

        GLshort rowstart_x = startx;
        GLshort rowstart_y = starty - i * row_height;

        oneLayer = layerRuntime + i;
        /*
                txt = init_txt(20);

                printf("title = %s\n", oneLayer->title);
                add_txt(txt, oneLayer->title);*/


        txt = init_textblock(1);
        append_2_textblock(txt,oneLayer->title, fonts[0]->fss->fs[text_size].bold);


        GLshort click_box[] = {rowstart_x, rowstart_y-click_size,rowstart_x + click_size,rowstart_y};
        GLshort text_box[] = {rowstart_x + click_size + col_dist, rowstart_y - click_size,rowstart_x + click_size + col_dist + text_width,rowstart_y};

        if(oneLayer->visible)
        {
            /*x_txt = init_txt(5);

            add_txt(x_txt, "X");
            */
            x_txt = init_textblock(1);
            append_2_textblock(x_txt,"X", fonts[0]->fss->fs[character_size].bold);

            new_ctrl = register_control(CHECKBOX, layers_meny,layers_meny, set_layer_visibility,NULL,NULL,click_box,click_box_color,x_txt,box_text_margins, 1,10);
        }
        else
            new_ctrl = register_control(CHECKBOX,layers_meny,layers_meny, set_layer_visibility,NULL,NULL,click_box,click_box_color,NULL,box_text_margins, 1,10);

        register_control(TEXTBOX, layers_meny,new_ctrl, NULL,NULL,NULL,text_box,txt_box_color,txt,margins, 1,10); //register text label and set checkbox as logical parent
        new_ctrl->obj = (void*) oneLayer;

        new_ctrl = add_radio_button(radio_master,set_info_layer, click_size, row_dist, 1, oneLayer->info_active);
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
    /*x_txt = init_txt(5);

    add_txt(x_txt, "X");
    */
    x_txt = init_textblock(1);
    append_2_textblock(x_txt,"X", fonts[0]->fss->fs[character_size].bold);
    register_control(CHECKBOX, layers_meny,layers_meny, hide_layer_selecter,NULL,NULL,close_box,close_color,x_txt,box_text_margins, 1,10); //register text label and set checkbox as logical parent

    return 0;
}




int set_info_txt(void *ctrl, void *page_p, tileless_event_func_in_func func_in_func)
{
    
    log_this(10, "Entering function %s with val %d and pointer to func in func %p\n", __func__, (int*) page_p,func_in_func);
    
    struct CTRL *t = (struct CTRL *) ctrl;
    int page = *((int*) page_p);
    ATLAS *font;
    
    sqlite3_stmt *preparedinfo;
    char *sql = "select txt, text_size, bold, link_to_page from tilelessmap_info where page = ? order by orderby;";
    
    int rc = sqlite3_prepare_v2(projectDB, sql, -1, &preparedinfo, 0);

    if (rc != SQLITE_OK ) {
        log_this(100, "SQL error in %s\n",sql );
        sqlite3_close(projectDB);
        return 1;
    }
    
    
    sqlite3_bind_int(preparedinfo, 1,page);
    
    if(t->txt)
        destroy_textblock(t->txt);
    
    TEXTBLOCK *tb = init_textblock(4);
    
     while(sqlite3_step(preparedinfo) ==  SQLITE_ROW)
     {
    
        const unsigned char *txt = sqlite3_column_text(preparedinfo, 0);
        int text_size = sqlite3_column_int(preparedinfo, 1);
        int bold = sqlite3_column_int(preparedinfo, 2);
        int link_to_page = sqlite3_column_int(preparedinfo, 3);
        
        if(bold)
            font = fonts[0]->fss->fs[text_size].bold;
        else
            font = fonts[0]->fss->fs[text_size].normal;
            
    
            append_2_textblock(tb, (char*)txt, font);
        
            append_2_textblock(tb," \n ", font);
        
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
    multiply_array(box_text_margins, size_factor, 2);
    


    struct CTRL *info_box = register_control(TEXTBOX, controls,t, NULL,NULL,NULL,box,color, NULL,box_text_margins, 1,20);
    
    
    add_close_button(info_box);
    int page = 1;
    set_info_txt(info_box, &page, NULL);
    
    return 0;
    
}


struct CTRL* add_tileless_info(struct CTRL *ctrl)
{
    if (!check_layer((const unsigned char*) "main",(const unsigned char*) "tilelessmap_info"))
     return 0;
     
    int page = 1;
    GLshort box_text_margins[] = {7,5};
    multiply_array(box_text_margins, size_factor, 2);
    
    GLshort click_size = 30 * size_factor;
    GLshort startx, starty, p[] = {0,0};
    
    p[0] = CURR_WIDTH;

    startx = p[0] - 50*size_factor;
    starty = p[1] + 20*size_factor;

    GLshort info_box[] = {startx, starty,startx + click_size,starty + click_size};

    GLfloat info_color[]= {100,200, 100,200};
    
    TEXTBLOCK *x_txt = init_textblock(1);
    append_2_textblock(x_txt,"i", fonts[0]->fss->fs[character_size].bold);
    return register_control(BUTTON, ctrl,ctrl, init_show_info,&page,NULL,info_box,info_color,x_txt,box_text_margins, 1,10);

    
    
}


