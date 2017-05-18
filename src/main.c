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



#include "theclient.h"



int init_resources(char *dir)
{
    log_this(10, "Entering init_resources\n");
    int i, rc;
    char     *err_msg;
    GLint attribute_coord2d, uniform_theMatrix, uniform_color, uniform_coord2d, box4d;

    LAYER_RUNTIME *oneLayer;
    char sql[2048];
    char *styleselect;
    char stylejoin[128];
    char stylewhere[128];
    char textselect[128];
    char textjoin[128];
    
    char missing_db[1024]; //just a string where we put names of db taht we cannot find. Then we use that for avoiding those layers. quite hackish
    int dbname_len;
    //char stylewhere[128];


    sqlite3_stmt *preparedLayerLoading;
    sqlite3_stmt *preparedCountLayers;
    sqlite3_stmt *preparedDb2Attach;
    sqlite3_stmt *preparedLayer;
    sqlite3_stmt *preparedCountStyle;
    sqlite3_stmt * preparedStylesLoading;

    build_program();
    /********************************************************************************
      Attach all databases with data for the project
    */

    char *sqlDb2Attach = " select distinct d.source, d.name from dbs d inner join layers l on d.name=l.source;";

    rc = sqlite3_prepare_v2(projectDB, sqlDb2Attach, -1, &preparedDb2Attach, 0);
    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlDb2Attach );
        sqlite3_close(projectDB);
        return 1;
    }
    int used_bytes = 0;
    while(sqlite3_step(preparedDb2Attach)==SQLITE_ROW)
    {

        const unsigned char *dbsource = sqlite3_column_text(preparedDb2Attach, 0);
        const unsigned char * dbname= sqlite3_column_text(preparedDb2Attach, 1);




        char sqlAttachDb[128];
        snprintf(sqlAttachDb,sizeof(sql), "ATTACH '%s//%s' AS %s;", dir, dbsource, dbname);
        log_this(10, "attachsql = %s\n",sqlAttachDb );
        rc = sqlite3_exec(projectDB,sqlAttachDb,NULL, NULL, &err_msg);
        if (rc != 0)
        {
            log_this(90, "failed to load db: %s. rc = %d errcode = %s, sql = %s\n",dbsource, rc,err_msg);  
            dbname_len = strlen(dbname);
            if(used_bytes + dbname_len + 3 >=1024)
            {
             log_this(10, "This is a hard day. You are missing so many databases so I cannot keep track of them \n%s\n",missing_db );   
             return 1;
            }
            if(used_bytes)
            {
                used_bytes += strlen(dbname) + 4;
                snprintf(missing_db, 1024-used_bytes,"%s, '%s'", missing_db, dbname);
            }
            else            
            {
                used_bytes += strlen(dbname)+2;               
                snprintf(missing_db, 1024-used_bytes,"'%s'", dbname);
            }
        }
        /* vs_source = sqlite3_column_text(preparedLayerLoading, 3);
         fs_source = sqlite3_column_text(preparedLayerLoading, 4);*/
    }
    sqlite3_finalize(preparedDb2Attach);

    /********************************************************************************
     Count the layers in the project and get maximum styleID in the project
     */
    char *sqlCountStyles = "SELECT COUNT(*), MAX(styleID)   "
                           "FROM styles s ; ";

    rc = sqlite3_prepare_v2(projectDB, sqlCountStyles, -1, &preparedCountStyle, 0);

    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlCountStyles );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_step(preparedCountStyle);
    int nStyles = sqlite3_column_int(preparedCountStyle, 0);
    int maxStyleID = sqlite3_column_int(preparedCountStyle, 1);
    sqlite3_finalize(preparedCountStyle);





    /********************************************************************************
     Count how many layers we are dealing with
    */
    char *sqlCountLayers = "SELECT COUNT(*)   "
                           "FROM layers l ; ";


    rc = sqlite3_prepare_v2(projectDB, sqlCountLayers, -1, &preparedCountLayers, 0);

    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlCountLayers );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_step(preparedCountLayers);
    nLayers = sqlite3_column_int(preparedCountLayers, 0);

    sqlite3_finalize(preparedCountLayers);


    /********************************************************************************
      Put all the styles in an array of style structures
    */
    length_global_styles = maxStyleID + 1;

    size_t thesize = length_global_styles * sizeof(STYLES_RUNTIME) + 1;
    global_styles = malloc(thesize); // + 1 is for making place for 1-based array
    memset(global_styles,0,thesize);

    char *sqlStyles = "SELECT "
                      /*fields for attaching the database*/
                      "styleID, color_r, color_g, color_b, color_a, out_r, out_g, out_b, line_w "
                      "from styles;";

    rc = sqlite3_prepare_v2(projectDB, sqlStyles, -1, &preparedStylesLoading, 0);

    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlStyles );
        sqlite3_close(projectDB);
        return 1;
    }

    for (i =0; i<nStyles; i++)
    {
        sqlite3_step(preparedStylesLoading);
        int styleID = sqlite3_column_int(preparedStylesLoading, 0);
        global_styles[styleID].styleID = styleID;
        global_styles[styleID].color[0] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 1)/255.0);
        global_styles[styleID].color[1] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 2)/255.0);
        global_styles[styleID].color[2] =  (GLfloat) (sqlite3_column_int(preparedStylesLoading, 3)/255.0);
        global_styles[styleID].color[3] =  (GLfloat) (sqlite3_column_int(preparedStylesLoading, 4)/255.0);
        global_styles[styleID].outlinecolor[0] =  (GLfloat) (sqlite3_column_int(preparedStylesLoading, 5)/255.0);
        global_styles[styleID].outlinecolor[1] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 6)/255.0);
        global_styles[styleID].outlinecolor[2] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 7)/255.0);
        global_styles[styleID].outlinecolor[3] = 1.0;
        global_styles[styleID].lineWidth =  sqlite3_column_int(preparedStylesLoading, 8);
    }

    sqlite3_finalize(preparedStylesLoading);
    /********************************************************************************
     Get information about all the layers in the project
     */
    char sqlLayerLoading[2048];
    char *sqlLayerLoading1 = "SELECT "
                            /*fields for attaching the database*/
                            "d.name dbname, "   // 0
                            /*fields for creating the prepared statement to get data later*/
                            "l.geometryField,"  // 1
                            "l.triIndexField,"  // 2
                            "l.idField,"  // 3
                            "l.name layername,"  // 4
                            "l.geometryindex, "  // 5
                            /*fields to inform comming processes about how and when to render*/
                            " l.defaultVisible,"  // 6
                            "l.minScale,"  // 7
                            "l.maxScale,"  // 8
                            "geometryType,"  // 9
                            "styleField,"  // 10
                            "showText,"  // 11
                            "linewidth,"  // 12
                            "l.layerID, "  // 13
                            "tc.size_fld,"  // 14
                            "rotation_fld,"  // 15
                            "anchor_fld,"  // 16
                            "txt_fld"  // 17
                            
                            " FROM layers l "
                            "INNER JOIN dbs d on l.source = d.name "
                            "LEFT JOIN text_conf tc on l.layerID=tc.layerID ";
                            
    if(used_bytes)
        snprintf(sqlLayerLoading, 2048, "%s where d.name not in (%s) order by l.orderby ;",sqlLayerLoading1, missing_db);
    else
        snprintf(sqlLayerLoading, 2048, "%s order by l.orderby ;",sqlLayerLoading1);


    log_this(10, "Get Layer sql : %s\n",sqlLayerLoading);
    rc = sqlite3_prepare_v2(projectDB, sqlLayerLoading, -1, &preparedLayerLoading, 0);

    if (rc != SQLITE_OK ) {
        log_this(110, "SQL error in %s\n",sqlLayerLoading);
        sqlite3_close(projectDB);
        return 1;
    }

    /********************************************************************************
     Time to iterate all layers in the project and add data about them in struct layerRuntime
    */

    layerRuntime = init_layer_runtime(nLayers);
  //  for (i =0; i<nLayers; i++)
    i=0;
while(sqlite3_step(preparedLayerLoading) ==  SQLITE_ROW)
    {
        oneLayer=layerRuntime + i;
     //   sqlite3_step(preparedLayerLoading);

        const unsigned char * dbname = sqlite3_column_text(preparedLayerLoading, 0);
        const unsigned char *geometryfield = sqlite3_column_text(preparedLayerLoading, 1);


        const unsigned char *tri_index_field = sqlite3_column_text(preparedLayerLoading, 2);


        const unsigned char *idfield = sqlite3_column_text(preparedLayerLoading, 3);
        const unsigned char *layername = sqlite3_column_text(preparedLayerLoading,4);
        const unsigned char *geometryindex = sqlite3_column_text(preparedLayerLoading, 5);
        const unsigned char *stylefield =  sqlite3_column_text(preparedLayerLoading, 10);
        int layerid =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 13);

        oneLayer->visible = sqlite3_column_int(preparedLayerLoading, 6);
        oneLayer->minScale = sqlite3_column_int(preparedLayerLoading, 7);
        oneLayer->maxScale = sqlite3_column_int(preparedLayerLoading, 8);
        oneLayer->geometryType =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 9);

        oneLayer->show_text =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 11);
        oneLayer->line_width =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 12);

        const unsigned char *size_fld = sqlite3_column_text(preparedLayerLoading,14);
        const unsigned char *rotation_fld = sqlite3_column_text(preparedLayerLoading, 15);
        const unsigned char *anchor_fld =  sqlite3_column_text(preparedLayerLoading, 16);
        const unsigned char *txt_fld =  sqlite3_column_text(preparedLayerLoading, 17);

        if (check_layer(dbname, layername))
        {
            i++;
        }
        else
        {
            log_this(90, "Cannot use layer %s",layername);
            continue;
        }
        
        //TODO free this
       oneLayer->name = malloc(strlen(layername)+1);
        
        strcpy(oneLayer->name,layername);
        
        
        //printf("name = %s\n", oneLayer->name);
        oneLayer->layer_id =  (uint8_t) layerid;

        oneLayer->render_area =! NULL;


        char tri_idx_fld[32];
        if(oneLayer->geometryType == POLYGONTYPE && oneLayer->render_area)
        {
            snprintf(tri_idx_fld, sizeof(tri_idx_fld), "%s", tri_index_field);
        }
        else
        {
            snprintf(tri_idx_fld, sizeof(tri_idx_fld), "%s","'a'" );
        }

        if(stylefield)
        {
            styleselect = ", styleID ";
            if(strcmp((const char *)stylefield, "__everything__"))
            {
                snprintf(stylejoin,sizeof(stylejoin), "%s%s%s", " inner join styles s on e.", stylefield, "=s.value " );
            }
            else
            {
                snprintf(stylejoin,sizeof(stylejoin), "%s", " , styles s " );
            }
            snprintf(stylewhere,sizeof(stylewhere), "%s%d", " and s.layerID=", layerid);
        }
        else
        {
            styleselect = "\0";
            stylejoin[0] = '\0';
            stylewhere[0] =  '\0';
        }

        if(oneLayer->show_text)
        {
            snprintf(textselect, sizeof(textselect), ",e.%s, e.%s,e.%s,e.%s",txt_fld, size_fld, rotation_fld, anchor_fld);
        }
        else
        {
            textselect[0] = '\0';
        }


        snprintf(sql,sizeof(sql), "%s%s %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                 "select ",
                 geometryfield,
                 ", ",tri_idx_fld,
                 ", e.", idfield, styleselect, textselect,  " from ",
                 dbname, ".",layername,
                 " e inner join ",
                 dbname, ".",geometryindex,
                 "  ei on e.",
                 idfield,
                 "=ei.id ",
                 stylejoin,
                 "where ",
                 " ei.minX<? and ei.maxX>? and ei.minY<? and ei.maxY >? ",
                 stylewhere );
        printf("sql = %s\n", sql);
        
        rc = sqlite3_prepare_v2(projectDB, sql, -1,&preparedLayer, 0);

        if (rc != SQLITE_OK ) {
            log_this(1, "SQL error in %s\n",sql );
            sqlite3_close(projectDB);
            return 1;
        }
        oneLayer->preparedStatement = preparedLayer;
        oneLayer->res_buf =  init_res_buf();

        if (oneLayer->geometryType == POLYGONTYPE)
            oneLayer->tri_index =  init_element_buf();

        if (oneLayer->show_text)
            oneLayer->text =  init_text_buf();

    }
nLayers = i;
    sqlite3_finalize(preparedLayerLoading);







    return 0;
}




int check_layer(char *dbname, char *layername)
{
    
    char sql[1024];
    int rc;
    sqlite3_stmt *prepared_sql;
    snprintf(sql, 1024, "select name from %s.sqlite_master where type = 'table' and name = '%s'", dbname, layername);
    
        rc = sqlite3_prepare_v2(projectDB, sql, -1, &prepared_sql, 0);

    if (rc != SQLITE_OK ) {
        log_this(110, "SQL error in %s\n",sql);
        return 0;
    }


if(sqlite3_step(prepared_sql) ==  SQLITE_ROW)
{
    sqlite3_finalize(prepared_sql);
    return 1;
}
else
{   return 0;

    sqlite3_finalize(prepared_sql);
}

}



void mainLoop(SDL_Window* window)
{
    log_this(10, "Entering mainLoop\n");
    GLfloat dDist, tx,ty,pr;
    int ti, fi;
    GLfloat currentBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    int mouse_down = 0;
    int wheel_y;
    SDL_Event ev;

    SDL_Event tmp_ev[10];
    int n_events;
    GLint px_x_clicked,px_y_clicked;

    FINGEREVENT *touches = init_touch_que();

    GLfloat mouse_down_x = 0, mouse_down_y = 0,mouse_up_x, mouse_up_y;

//     initialBBOX(380000, 6660000, 300000, newBBOX);
//initialBBOX(230000, 6660000, 5000, newBBOX);
   initialBBOX(325000, 6800000, 800000, newBBOX);


    GLfloat theMatrix[16];

    matrixFromBBOX(newBBOX, theMatrix);


    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error 65 :%d\n", err);
    }

    get_data(window, newBBOX, theMatrix);
    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"oerror on return: %d\n", err);
    }
    copyNew2CurrentBBOX(newBBOX, currentBBOX);

    while (1)
    {
        if (SDL_WaitEvent(&ev)) /* execution suspends here while waiting on an event */
        {
            while ((err = glGetError()) != GL_NO_ERROR) {
                log_this(10, "Problem 2\n");
                fprintf(stderr,"opengl error aaa000: %d\n", err);
            }
            switch (ev.type)
            {
#ifndef __ANDROID__
            case SDL_MOUSEBUTTONDOWN:
                mouse_down = 1;
                mouse_down_x = ev.button.x;
                mouse_down_y = ev.button.y;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_down = 0;
                mouse_up_x = ev.button.x;
                mouse_up_y = ev.button.y;
                matrixFromDeltaMouse(currentBBOX,newBBOX,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y, theMatrix);

                get_data(window, newBBOX, theMatrix);
                copyNew2CurrentBBOX(newBBOX, currentBBOX);
                break;
            case SDL_MOUSEWHEEL:
                wheel_y = ev.wheel.y;

                SDL_GetMouseState(&px_x_clicked, &px_y_clicked);

                if(wheel_y > 0)
                    matrixFromBboxPointZoom(currentBBOX,newBBOX,px_x_clicked, px_y_clicked, 0.5, theMatrix);
                else
                    matrixFromBboxPointZoom(currentBBOX,newBBOX,px_x_clicked, px_y_clicked, 2, theMatrix);

                get_data(window, newBBOX, theMatrix);

                copyNew2CurrentBBOX(newBBOX, currentBBOX);
                break;

            case SDL_MOUSEMOTION:





                if(mouse_down)
                {
                    n_events = 	SDL_PeepEvents(tmp_ev,3,SDL_PEEKEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT);

                    if(n_events<2)
                    {

                        mouse_up_x = ev.motion.x;
                        mouse_up_y = ev.motion.y;



                        matrixFromDeltaMouse(currentBBOX,newBBOX,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y, theMatrix);

                        while ((err = glGetError()) != GL_NO_ERROR) {
                            log_this(10, "Problem 2\n");
                            fprintf(stderr,"opengl error aaa: %d\n", err);
                        }
                        render_data(window, theMatrix);
                        //         copyNew2CurrentBBOX(newBBOX, currentBBOX);
                        while ((err = glGetError()) != GL_NO_ERROR) {
                            log_this(10, "Problem 2\n");
                            fprintf(stderr,"opengl error aaa999: %d\n", err);
                        }

                    }


                    break;

                }


#endif

            case SDL_FINGERDOWN:
                ti = ev.tfinger.touchId;
                fi = ev.tfinger.fingerId;
                tx = ev.tfinger.x;
                ty = ev.tfinger.y;
                pr = ev.tfinger.pressure;
                //DEBUG_PRINT(("DOWN: fi=%d, x = %f, y = %f, pr = %f, ti = %d\n",fi, tx, ty,pr,ti));

                register_touch_down(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
                break;
            case SDL_FINGERUP:
                log_this(10,"SDL_FINGERUP");

                ti = ev.tfinger.touchId;
                fi = ev.tfinger.fingerId;
                tx = ev.tfinger.x;
                ty = ev.tfinger.y;
                pr = ev.tfinger.pressure;

                if(touches[1].active) //check if at least 2 fingers are activated
                {
                    log_this(10, "UP: fi=%d, x = %f, y = %f, pr = %f, ti = %d\n",fi, tx, ty,pr,ti);
                    if(register_touch_up(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y))
                    {
                        log_this(10, "OK, on the inside\n");
                        get_box_from_touches(touches, currentBBOX, newBBOX);
                        reset_touch_que(touches);
                        matrixFromBBOX(newBBOX, theMatrix);
                        log_this(10,"ok, go get data");
                        get_data(window, newBBOX, theMatrix);
                        copyNew2CurrentBBOX(newBBOX, currentBBOX);
                    }
                }
                else
                {
                    //	  mouse_down = 0;
                    mouse_up_x = tx * CURR_WIDTH;
                    mouse_up_y = ty * CURR_HEIGHT;
                    mouse_down_x = touches[0].x1 * CURR_WIDTH;
                    mouse_down_y = touches[0].y1 * CURR_HEIGHT;
                    matrixFromDeltaMouse(currentBBOX,newBBOX,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y, theMatrix);
                    reset_touch_que(touches);

                    //render_data(window, newBBOX, theMatrix);
                    get_data(window, newBBOX, theMatrix);
                    copyNew2CurrentBBOX(newBBOX, currentBBOX);

                }

                break;

            case SDL_FINGERMOTION:
                log_this(10,"SDL_FINGERMOTION");
                n_events = 	SDL_PeepEvents(tmp_ev,3,SDL_PEEKEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT);

                if(touches[1].active) //check if at least 2 fingers are activated
                {
                    log_this(10, "m2");

                    register_motion(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
                    log_this(10, "register_motion");
                    get_box_from_touches(touches, currentBBOX, newBBOX);
                    log_this(10, "get_box_from_touches");
                    //reset_touch_que(touches);
                    matrixFromBBOX(newBBOX, theMatrix);
                    log_this(10, "matrixFromBBOX");
                    if(n_events<2)
                        render_data(window, theMatrix);


                }
                else
                {
                    log_this(10, "m1");
                    //~ android_log_print(ANDROID_LOG_INFO, APPNAME, "m1");
                    ti = ev.tfinger.touchId;
                    fi = ev.tfinger.fingerId;
                    tx = ev.tfinger.x;
                    ty = ev.tfinger.y;
                    pr = ev.tfinger.pressure;


                    //	  mouse_down = 0;
                    mouse_up_x = tx * CURR_WIDTH;
                    mouse_up_y = ty * CURR_HEIGHT;
                    mouse_down_x = touches[0].x1 * CURR_WIDTH;
                    mouse_down_y = touches[0].y1 * CURR_HEIGHT;
                    matrixFromDeltaMouse(currentBBOX,newBBOX,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y, theMatrix);

                    if(n_events<2)
                        render_data(window, theMatrix);
                    //         copyNew2CurrentBBOX(newBBOX, currentBBOX);
                }

                break;





            case SDL_WINDOWEVENT:
                if (ev.window.event  == SDL_WINDOWEVENT_RESIZED)
                {
                    windowResize(ev.window.data1,ev.window.data2,currentBBOX, newBBOX);
                    copyNew2CurrentBBOX(newBBOX, currentBBOX);

                    glViewport(0,0,CURR_WIDTH, CURR_HEIGHT);

                }
                break;

            case SDL_QUIT:
                free(touches);
                return;
            }
        }
        //      render_data(window,currentBBOX,theMatrix);
    }
    free(touches);
    return ;
}

void free_resources(SDL_Window* window,SDL_GLContext context) {
    int t;
    log_this(10, "Entering free_resources\n");

    LAYER_RUNTIME theLayer;
        glDeleteProgram(std_program);
        glDeleteProgram(txt_program);
        glDeleteProgram(lw_program);
        
        destroy_layer_runtime(layerRuntime,nLayers);

    free(global_styles);
    sqlite3_close_v2(projectDB);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow( window );
    window = NULL;
    //Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char **argv)
{

    log_this(10, "OK, let's start \n");
    text_scale=2;
    char projectfile[500];
    char *the_file=NULL, *dir=NULL;
    
    while(--argc>0)
    {
        argv++;
        if(!strcmp(*argv,"-f") || !strcmp(*argv,"--projfile"))
        {
            argc--;
            if(argc > 0)
                the_file=*++argv;
            else
            {
                log_this(110, "Too few arguments \n");
                return 1;
            }
            continue;
        }

        if(!strcmp(*argv,"-d") || !strcmp(*argv,"--directory"))
        {
            argc--;
              if(argc > 0)
                dir=*++argv;
            else
            {
                log_this(110, "Too few arguments \n");
                return 1;
            }
            
            continue;
        }
    }
    
    if (!(the_file && dir))
    {
                log_this(110, "Too few arguments \n");
                return 1;
            }
            
            
            log_this(110, "file = %s and dir = %s \n",the_file, dir );
//snprintf(projectfile, 500, "%s", "hedmark.sqlite");
//snprintf(projectfile, 500, "%s%s",dir, "/gsd_proj2.sqlite");
//snprintf(projectfile, 500, "%s%s",dir, "/varmland_proj.sqlite");
//snprintf(projectfile, 500, "%s%s",dir, "/norden_proj.sqlite");
  
//  snprintf(projectfile, 500, "%s%s",dir, "/demo.sqlite");
    
            
            
snprintf(projectfile, 500, "%s",the_file);
    log_this(10, "project file = %s\n", projectfile);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    glDisable (GL_DEPTH_TEST);;

    
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
       SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,0);
    

    SDL_Rect r;
    if (SDL_GetDisplayBounds(0, &r) != 0) {
        SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
        return 1;
    }


#ifndef __ANDROID__
    SDL_Window* window = SDL_CreateWindow("TileLess",
                                          0, 0, r.w, r.h,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
#else

    SDL_Window* window = SDL_CreateWindow("TileLess",
                                          0, 0, r.w, r.h,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);

#endif
    CURR_WIDTH = r.w;
    CURR_HEIGHT = r.h;

//log_this(10, "width =  %d and height = %d\n",r.w, r.h);
    if (window == NULL) {
        log_this(1, "Error: can't create window:  : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* Open db-connection*/
    sqlite3_initialize();

    int rc = sqlite3_open_v2(projectfile, &projectDB,SQLITE_OPEN_READONLY, NULL );

    if (rc != SQLITE_OK) {
        log_this(1, "Cannot open database: %s\n", sqlite3_errmsg(projectDB));
        sqlite3_close(projectDB);
        return 1;
    }

    /*load the db into memory*/
    loadOrSaveDb(projectDB, projectfile,0);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        log_this(1, "Error: SDL_GL_CreateContext : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_GL_SetSwapInterval(1);
#ifndef __ANDROID__
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        fprintf(stderr, "Error: glewInit : %s", glewGetErrorString(glew_status) );
        return EXIT_FAILURE;
    }
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0");
        return EXIT_FAILURE;
    }
#endif




    if (init_resources(dir))
        return EXIT_FAILURE;
//if (init_text_resources())
    //      return EXIT_FAILURE;




    if (init_text_resources(dir))
    {
        log_this(1,"Problems in init_text_resources");
        return EXIT_FAILURE;

    }



    mainLoop(window);

    sqlite3_close(projectDB);
    free_resources(window, context);
    return EXIT_SUCCESS;
}
