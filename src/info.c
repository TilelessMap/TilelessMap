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


#include "theclient.h"
#include "buffer_handling.h"
#include "info.h"
#include "text/text.h"
#include "interface/interface.h"
#include "utils.h"


static int printinfo(LAYER_RUNTIME *theLayer,uint64_t twkb_id)
{


    sqlite3_stmt *prepared_info;
    sqlite3_stmt *prepared_layer_info;
    TEXTBLOCK *tb;
    int i = 0;

int max_width = 1000;
    char number_text[32];
    char header_tot[32];


    GLshort click_box_width = 50;
    GLshort click_box_height = 50;

    char *info_sql = "select field, row, column, header from info where layerID = ? order by row, column";
	check_sql(info_sql);
    int rc = sqlite3_prepare_v2(projectDB, info_sql, -1,&prepared_info, 0);
    if (rc != SQLITE_OK ) {
        log_this(100, "SQL error in %s\n",info_sql );
        sqlite3_close(projectDB);
        return 1;
    }

    TEXT *layer_info_sql = init_txt(512);


    sqlite3_bind_int(prepared_info, 1,theLayer->layer_id);



    add_txt(layer_info_sql, "SELECT ");
    while (sqlite3_step(prepared_info)==SQLITE_ROW)
    {

        if(i>0)
            add_txt(layer_info_sql, ", ");

        const unsigned char *field = sqlite3_column_text(prepared_info, 0);
//            printf("test header = %s, field = %s\n",(char*) sqlite3_column_text(prepared_info, 3), (char*) field);
        add_txt(layer_info_sql, (const char*) field);

        i++;
    }
    add_txt(layer_info_sql, " FROM ");
    if(theLayer->info_rel)
        add_txt(layer_info_sql, theLayer->info_rel);
    else {
        add_txt(layer_info_sql,  theLayer->db );
        add_txt(layer_info_sql, ".");
        add_txt(layer_info_sql, theLayer->name);
    }
    //TODO get rid of hardcoded id field
    add_txt(layer_info_sql, " where twkb_id = ?;");


    sqlite3_reset(prepared_info);
                check_sql(layer_info_sql->txt);
    rc = sqlite3_prepare_v2(projectDB, layer_info_sql->txt, -1,&prepared_layer_info, 0);
    if (rc != SQLITE_OK ) {
        log_this(100, "SQL error in %s\n",layer_info_sql->txt );
        sqlite3_close(projectDB);
        return 1;
    }



    sqlite3_bind_int(prepared_layer_info, 1,twkb_id);

    tb = init_textblock();

            GLfloat fontcolor[] = {0,0,0,255};
            append_2_textblock(tb, "Info\n", text_font_bold, fontcolor,max_width,NEW_STRING, tmp_unicode_txt);
    while (sqlite3_step(prepared_layer_info)==SQLITE_ROW)
    {

        i=0;
        while (sqlite3_step(prepared_info)==SQLITE_ROW)
        {
            int type = sqlite3_column_type(prepared_layer_info, i);


            const unsigned char *header = sqlite3_column_text(prepared_info, 3);
            printf("header = %s\n", header);
            int row = sqlite3_column_int(prepared_info, 1);
            int col = sqlite3_column_int(prepared_info, 2);

            
            append_2_textblock(tb, (const char*) header_tot, text_font_bold, fontcolor,max_width,APPENDING_STRING, tmp_unicode_txt);
            if(type == SQLITE_INTEGER)
            {
                int val_int = sqlite3_column_int(prepared_layer_info, i);
                snprintf(number_text, 32, "%d\n", val_int);
                printf("header = %s, row = %d, col = %d, value = %d    \n",header, row, col, val_int);
                append_2_textblock(tb, (const char*) number_text, text_font_normal, fontcolor,max_width, APPENDING_STRING, tmp_unicode_txt);

            }
            else if (type == SQLITE_FLOAT)
            {
                double val_float = sqlite3_column_double(prepared_layer_info, i);
                snprintf(number_text, 32, "%f\n", val_float);
                printf("header = %s, row = %d, col = %d, value = %lf    \n",header, row, col, val_float);
                append_2_textblock(tb, (const char*) number_text, text_font_normal, fontcolor,max_width, APPENDING_STRING, tmp_unicode_txt);

            }
            else if (type == SQLITE_TEXT)
            {
                const unsigned char *val_txt = sqlite3_column_text(prepared_layer_info, i);
                printf("header = %s, row = %d, col = %d, value = %s    \n",header, row, col, val_txt);
                append_2_textblock(tb, (const char*) val_txt, text_font_normal, fontcolor,max_width, APPENDING_STRING, tmp_unicode_txt);
                append_2_textblock(tb, "\n", text_font_normal, fontcolor,max_width, APPENDING_STRING, tmp_unicode_txt);

            }
            i++;
        }

    }
    GLshort box[4];
    box[0] = box[1] = 30;
    box[2] = 1000;
    box[3] = CURR_HEIGHT - 30;
    GLfloat color[] = {200,255,200,150};
    GLshort txt_margin[] = {50,50};
    multiply_short_array(txt_margin, size_factor, 2);
    
    CTRL *controls = get_master_control();
    struct CTRL *textbox = init_textbox(controls, controls, box, color, txt_margin, 1,20);
    add_txt_2_textbox(textbox, tb);
    textbox->alignment = V_TOP_ALIGNMENT|H_LEFT_ALIGNMENT;
    
    init_matrix_handler(textbox, 1, 0, 0);
    incharge = textbox;


    matrixFromBBOX(textbox->matrix_handler);

    GLshort p[] = {0,0};
    GLshort box_text_margins[] = {2,2};
    get_top_right(textbox, p);

    GLshort startx = p[0] - 50 * size_factor;
    GLshort starty = p[1] - 50 * size_factor;

    GLshort close_box[] = {startx, starty,startx + click_box_width,starty + click_box_height};
    GLfloat close_color[]= {200,100,100,200};
    TEXTBLOCK *x_txt = init_textblock();
    append_2_textblock(x_txt,"X", char_font, fontcolor,0,NEW_STRING, tmp_unicode_txt);
    register_control(CHECKBOX, textbox,textbox, close_ctrl,NULL,NULL,close_box,close_color,x_txt,box_text_margins, 1,22); //register text label and


    return 1;
}




int identify(MATRIX *map_matrix, int x, int y)
{
    log_this(10,"info, x=%d, y=%d\n",x,y);
    GLfloat w_x, w_y;
    int i;
    int inside,n_dims, n_elements;
    px2m(map_matrix->bbox,x,y,&w_x,&w_y);
    int64_t id;
    GLfloat meterPerPixel = (map_matrix->bbox[2]-map_matrix->bbox[0])/CURR_WIDTH;
//  printf("w_x = %f, w_y = %f\n", w_x, w_y);

    GLfloat box[4];
    GLfloat point[2];

    box[0] = point[0] = w_x;
    box[1] = point[1] = w_y;
    box[2] = w_x;
    box[3] = w_y;



    LAYER_RUNTIME *theLayer;
    POLYGON_LIST *poly = infoLayer->polygons;
    POLYGON_LIST *renderpoly = infoRenderLayer->polygons;
    infoLayer->style_key_type = 1; //just to avoid protests when parsing data;

    reset_buffers(infoRenderLayer);
    infoRenderLayer->visible = 0;
    for (i = 0; i<global_layers->nlayers; i++)
    {
        theLayer = global_layers->layers + i;
        //     printf("layer name %s\n",theLayer->name);
        if(theLayer->visible && theLayer->minScale<=meterPerPixel && theLayer->maxScale>meterPerPixel)
        {
            //reset all used buffers in our infoLayer
            reset_buffers(infoLayer);


            //init more buffers if needed
            //Manipulate type, to not get unnessecary info
            if(theLayer->type & 6)
                infoLayer->type = 4; //set type to simple polygon
            else if(theLayer->type & 24)
                infoLayer->type = 16; //set type to simple line
            else if(theLayer->type & 192)
                infoLayer->type = 128; //set type to simple point

            // init_buffers(infoLayer);

            //Check if infoLayer holds an old prepared statemenst
            //if so unsubscribe from it
            if(infoLayer->preparedStatement->usage)
                infoLayer->preparedStatement->usage--;
            //borrow prepared statement
            infoLayer->preparedStatement = theLayer->preparedStatement;
            infoLayer->preparedStatement->usage++;
            
            //If we in the future will handle 3D, we are prepared
            infoLayer->n_dims = theLayer->n_dims;

            //get the right UTM-zone and hemisphere
            infoLayer->utm_zone = theLayer->utm_zone;
            infoLayer->hemisphere = theLayer->hemisphere;



            //add the "box, well it is just a point, but it will do
            infoLayer->BBOX = box;

            twkb_fromSQLiteBBOX(infoLayer);


            if((theLayer->type & 6) && theLayer->info_active)
            {


                n_dims = infoLayer->n_dims;

                unsigned int poly_n = 0;
                int next_polystart;
                if (poly->polygon_start_indexes->used>1)
                    next_polystart = poly->polygon_start_indexes->list[poly_n+1];
                else
                    next_polystart =  poly->vertex_array->used;
                int curr_pa_start = 0;
                int ring_n = 0;
                int next_pa_start = 0;
                int curr_poly_start = 0;
                int n_elements_acc = 0;
                unsigned int pa;
                inside = 0;
                for (pa=0; pa<poly->pa_start_indexes->used; pa++)
                {
                    curr_pa_start = next_pa_start;
                    next_pa_start = poly->pa_start_indexes->list[pa];



                    if(ring_n == 0 || inside)
                    {

                        if(wn_PnPoly( point,poly->vertex_array->list + curr_pa_start, (next_pa_start - curr_pa_start)/n_dims, n_dims))
                        {
                            if(ring_n == 0) //outer boundary
                                inside = 1;
                            else
                                inside = 0;
                        }
                        else
                        {
                            if(ring_n == 0)
                                inside = 0;
                            else
                                inside = 1;
                        }

                    }


                    ring_n++;
                    if(next_polystart == next_pa_start)
                    {
                        if(inside)
                        {
                            id = infoLayer->twkb_id->list[pa];
                            log_this(100,"ok, poly for rendering");
                            add2gluint_list(renderpoly->polygon_start_indexes, renderpoly->vertex_array->used); //register start of new polygon to render
                            addbatch2glfloat_list(renderpoly->vertex_array, next_polystart - curr_poly_start, poly->vertex_array->list + curr_poly_start); //memcpy all vertexes in polygon

                            add2pointer_list(renderpoly->style_id,system_default_info_style);


                            n_elements = *(poly->element_start_indexes->list + poly_n) - n_elements_acc;

                            addbatch2glushort_list(renderpoly->element_array, n_elements, poly->element_array->list + n_elements_acc); //memcpy all vertexes in polygon
                            add2gluint_list(renderpoly->element_start_indexes, renderpoly->element_array->used); //register start of new polygon to render

                            printinfo(theLayer,id);
                            infoRenderLayer->visible = 1;
                        }
                        curr_poly_start = next_polystart;
                        n_elements_acc = *(poly->element_start_indexes->list + poly_n);
                        poly_n++;
                        if(poly_n < poly->polygon_start_indexes->used - 1)
                            next_polystart = poly->polygon_start_indexes->list[poly_n+1];
                        else
                            next_polystart = poly->vertex_array->used;
                        ring_n = 0;
                        inside = 0;
                    }
                }

            }


        }


    }

    //setzero2pointer_list(renderpoly->style_id, renderpoly->polygon_start_indexes->used);


    //render_info(window,map_matrix->matrix);

    return 0;
}
































