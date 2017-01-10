/**********************************************************************
 *
 * NoTile
 *
 * NoTile is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * NoTile is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NoTile.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016 Nicklas Avén
 *
 **********************************************************************/



#include "theclient.h"



int init_resources()
{
    DEBUG_PRINT(("Entering init_resources\n"));
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
    //char stylewhere[128];


    GLuint vs, fs, program;
    sqlite3_stmt *preparedLayerLoading;
    sqlite3_stmt *preparedCountLayers;
    sqlite3_stmt *preparedDb2Attach;
    sqlite3_stmt *preparedLayer;
    sqlite3_stmt *preparedCountStyle;
    sqlite3_stmt * preparedStylesLoading;

        
     /********************************************************************************
      Get information about all the layers in the project
      */
    char *sqlLayerLoading = "SELECT "
                            /*fields for attaching the database*/
                            "d.name dbname, d.source filename, "
                            /*fields used to find if program is already compiled*/
                            "p.programID, "
                            /*fields for compilation if not already done*/
                            " vs.source vsSource, fs.source fsSource, "
                            /*fields for creating the prepared statement to get data later*/
                            "l.geometryField,l.triIndexField, l.idField, l.name layername, l.geometryindex, "
                            /*fields to inform comming processes about how and when to render*/
                            " l.defaultVisible, l.minScale, l.maxScale, geometryType,styleField,showText, l.layerID, "
                            "tc.size_fld, rotation_fld, anchor_fld, txt_fld,pt.programID , vt.source vtSource, ft.source ftSource "
                            " FROM layers l "
                            "INNER JOIN dbs d on l.source = d.name "
                            "INNER JOIN programs p on l.program = p.programID "
                            "INNER JOIN shaders vs on p.vs = vs.name "
                            "INNER JOIN shaders fs on p.fs = fs.name "
			    "LEFT JOIN text_conf tc on l.layerID=tc.layerID "
			    "LEFT JOIN programs pt on tc.program=pt.programID "
			    "LEFT JOIN shaders vt on pt.vs=vt.name "
			    "LEFT JOIN shaders ft on pt.fs=ft.name "			    
			    "order by l.orderby ;";
			    
			    
			    
			    
    DEBUG_PRINT(("Get Layer sql : %s\n",sqlLayerLoading));
    rc = sqlite3_prepare_v2(projectDB, sqlLayerLoading, -1, &preparedLayerLoading, 0);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error in %s\n",sqlLayerLoading );
        sqlite3_close(projectDB);
        return 1;
    }
    
    
     /********************************************************************************
      Count the layers in the project and get maximum styleID in the project
      */
    char *sqlCountStyles = "SELECT COUNT(*), MAX(styleID)   "
                           "FROM styles s ; ";

    rc = sqlite3_prepare_v2(projectDB, sqlCountStyles, -1, &preparedCountStyle, 0);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error in %s\n",sqlCountStyles );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_step(preparedCountStyle);
    int nStyles = sqlite3_column_int(preparedCountStyle, 0);
    int maxStyleID = sqlite3_column_int(preparedCountStyle, 1);
    sqlite3_finalize(preparedCountStyle);
    
    
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
        fprintf(stderr, "SQL error in %s\n",sqlStyles );
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
        global_styles[styleID].lineWidth =  sqlite3_column_int(preparedStylesLoading, 8);
    }

    sqlite3_finalize(preparedStylesLoading);
    
    
    /********************************************************************************
      Attach all databases with data for the project
    */

    char *sqlDb2Attach = " select distinct d.source, d.name from dbs d inner join layers l on d.name=l.source;";

    rc = sqlite3_prepare_v2(projectDB, sqlDb2Attach, -1, &preparedDb2Attach, 0);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error in %s\n",sqlDb2Attach );
        sqlite3_close(projectDB);
        return 1;
    }

    while(sqlite3_step(preparedDb2Attach)==SQLITE_ROW)
    {

        const unsigned char *dbsource = sqlite3_column_text(preparedDb2Attach, 0);
        const unsigned char * dbname= sqlite3_column_text(preparedDb2Attach, 1);




        char sqlAttachDb[30];
        snprintf(sqlAttachDb,sizeof(sql), "%s%s%s%s%s",
                 "ATTACH '",dbsource,"' AS ", dbname,";");
        DEBUG_PRINT(("attachsql = %s\n",sqlAttachDb ));
        sqlite3_exec(projectDB,sqlAttachDb,NULL, NULL, &err_msg );

        DEBUG_PRINT(("errcode = %s\n",err_msg ));
        /* vs_source = sqlite3_column_text(preparedLayerLoading, 3);
         fs_source = sqlite3_column_text(preparedLayerLoading, 4);*/
    }
    sqlite3_finalize(preparedDb2Attach);
    
    /********************************************************************************
     Count how many layers we are dealing with
    */
    char *sqlCountLayers = "SELECT COUNT(*)   "
                           "FROM layers l ; ";

    
    rc = sqlite3_prepare_v2(projectDB, sqlCountLayers, -1, &preparedCountLayers, 0);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error in %s\n",sqlCountLayers );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_step(preparedCountLayers);
    nLayers = sqlite3_column_int(preparedCountLayers, 0);

    sqlite3_finalize(preparedCountLayers);
    

    /********************************************************************************
     Time to iterate all layers in the project and add data about them in struct layerRuntime
    */

    layerRuntime = init_layer_runtime(nLayers);
    

    for (i =0; i<nLayers; i++)
    {
        oneLayer=layerRuntime + i;
        sqlite3_step(preparedLayerLoading);

        const unsigned char * vs_source = sqlite3_column_text(preparedLayerLoading, 3);
        const unsigned char *  fs_source = sqlite3_column_text(preparedLayerLoading, 4);

	
	program = create_program(vs_source, fs_source, &vs, &fs);
	
        attribute_coord2d = glGetAttribLocation(program, "coord2d");
        if (attribute_coord2d == -1) {
            fprintf(stderr, "Could not bind attribute : %s\n", "coord2d");
            return 0;
        }

        uniform_theMatrix = glGetUniformLocation(program, "theMatrix");
        if (uniform_theMatrix == -1) {
            fprintf(stderr, "Could not bind uniform : %s\n", "theMatrix");
            return 0;
        }

        uniform_color = glGetUniformLocation(program, "color");
        if (uniform_color == -1) {
            fprintf(stderr, "Could not bind uniform : %s\n", "color");
            return 0;
        }

        oneLayer->program = program;
        oneLayer->attribute_coord2d = attribute_coord2d;
        oneLayer->uniform_theMatrix = uniform_theMatrix;
        oneLayer->uniform_color = uniform_color;

	reset_shaders(vs, fs, program);
	
	
        const unsigned char * dbname = sqlite3_column_text(preparedLayerLoading, 0);
        const unsigned char *geometryfield = sqlite3_column_text(preparedLayerLoading, 5);


        const unsigned char *tri_index_field = sqlite3_column_text(preparedLayerLoading, 6);


        const unsigned char *idfield = sqlite3_column_text(preparedLayerLoading, 7);
        const unsigned char *layername = sqlite3_column_text(preparedLayerLoading,8);
        const unsigned char *geometryindex = sqlite3_column_text(preparedLayerLoading, 9);
        const unsigned char *stylefield =  sqlite3_column_text(preparedLayerLoading, 14);
        int layerid =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 16);

        oneLayer->visible = sqlite3_column_int(preparedLayerLoading, 10);
        oneLayer->minScale = sqlite3_column_int(preparedLayerLoading, 11);
        oneLayer->maxScale = sqlite3_column_int(preparedLayerLoading, 12);
        oneLayer->geometryType =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 13);
	
        oneLayer->show_text =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 15);
	
        const unsigned char *size_fld = sqlite3_column_text(preparedLayerLoading,17);
        const unsigned char *rotation_fld = sqlite3_column_text(preparedLayerLoading, 18);
        const unsigned char *anchor_fld =  sqlite3_column_text(preparedLayerLoading, 19);
        const unsigned char *txt_fld =  sqlite3_column_text(preparedLayerLoading, 20);
//	oneLayer->has_text=0;

	
	if(oneLayer->show_text)
	{
	    const unsigned char *vt_source = sqlite3_column_text(preparedLayerLoading, 22);
	    const unsigned char *ft_source = sqlite3_column_text(preparedLayerLoading, 23);

	
	program = create_program(vt_source, ft_source, &vs, &fs);
	
        uniform_coord2d = glGetUniformLocation(program, "coord2d");
        if (uniform_coord2d == -1) {
            fprintf(stderr, "Could not bind uniform : %s\n", "coord2d");
            return 0;
        }

        box4d = glGetAttribLocation(program, "box");
        if (box4d == -1) {
            fprintf(stderr, "Could not bind attribute : %s\n", "box");
            return 0;
        }

        uniform_theMatrix = glGetUniformLocation(program, "theMatrix");
        if (uniform_theMatrix == -1) {
            fprintf(stderr, "Could not bind uniform : %s\n", "theMatrix");
            return 0;
        }

        uniform_color = glGetUniformLocation(program, "color");
        if (uniform_color == -1) {
            fprintf(stderr, "Could not bind uniform : %s\n", "color");
            return 0;
        }

        oneLayer->txt_program = program;
        oneLayer->txt_coord2d = uniform_coord2d;
        oneLayer->txt_theMatrix = uniform_theMatrix;
        oneLayer->txt_color = uniform_color;
      oneLayer->txt_box = box4d;
	reset_shaders(vs, fs, program);
	
	  
	  
	}
	
	
	
	
	
	
        char tri_idx_fld[32];
        if(oneLayer->geometryType == POLYGONTYPE)
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

        rc = sqlite3_prepare_v2(projectDB, sql, -1,&preparedLayer, 0);

        if (rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error in %s\n",sql );
            sqlite3_close(projectDB);
            return 1;
        }
        oneLayer->preparedStatement = preparedLayer;
        DEBUG_PRINT(("put prepared statement from sql : %s, into : %p\n",sql,oneLayer->preparedStatement ));

        oneLayer->res_buf =  init_res_buf();
printf("lager %d = %p and resbuf = %p\n" ,i,  oneLayer, oneLayer->res_buf);
        if (oneLayer->geometryType == POLYGONTYPE)
            oneLayer->tri_index =  init_element_buf();

	if (oneLayer->show_text)
	  oneLayer->text =  init_text_buf();

    }

      
    

    sqlite3_finalize(preparedLayerLoading);
    return 0;
}

void mainLoop(SDL_Window* window) {

    DEBUG_PRINT(("Entering ,mainLoop\n"));

    DEBUG_PRINT(("SwapInterval is %d\n", SDL_GL_GetSwapInterval()));
GLfloat dDist, tx,ty,pr;
int ti;
    GLfloat currentBBOX[4] = {0.0,0.0,0.0,0.0};
    GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    int mouse_down;
    int wheel_y;
    SDL_Event ev;
    GLint px_x_clicked,px_y_clicked;

    FINGEREVENT *touches = init_touch_que();
    

    GLfloat mouse_down_x = 0, mouse_down_y = 0,mouse_up_x, mouse_up_y;


    initialBBOX(380000, 6644000, 1000, newBBOX);


    GLfloat theMatrix[16];

    matrixFromBBOX(newBBOX, theMatrix);

    get_data(window, newBBOX, theMatrix);

    copyNew2CurrentBBOX(newBBOX, currentBBOX);

    while (1)
    {

        if (SDL_WaitEvent(&ev)) /* execution suspends here while waiting on an event */
        {

            switch (ev.type)
            {
            case SDL_QUIT:
	      free(touches);
                return;
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
		//render_data(window,currentBBOX,theMatrix);
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
        /*    case SDL_MULTIGESTURE:
            dDist = ev.mgesture.dDist;
            tx = ev.mgesture.x;
            ty = ev.mgesture.y;
            DEBUG_PRINT(("dDist=%f, x = %f, y = %f\n",dDist, tx, ty));*/
            case SDL_FINGERDOWN:        
                register_touch_down(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
                break;
            case SDL_FINGERUP:	      
                if(register_touch_up(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y))
                {
                    get_box_from_touches(touches, currentBBOX, newBBOX);                
                    reset_touch_que(touches);
                    matrixFromBBOX(newBBOX, theMatrix);
                    get_data(window, newBBOX, theMatrix);
                    copyNew2CurrentBBOX(newBBOX, currentBBOX);
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
            }
        }
        //      render_data(window,currentBBOX,theMatrix);
    }
	      free(touches);
    return ;
}

void free_resources(SDL_Window* window,SDL_GLContext context) {
    int t;
    DEBUG_PRINT(("Entering free_resources\n"));

    LAYER_RUNTIME theLayer;
    for (t=0; t<nLayers; t++)
    {
        theLayer = layerRuntime[t];
        glDeleteProgram(theLayer.program);
        destroy_buffer(theLayer.res_buf);
        if (theLayer.geometryType == POLYGONTYPE)
        {
            element_destroy_buffer(theLayer.tri_index);
        }

        sqlite3_finalize(theLayer.preparedStatement);

    }
    free(layerRuntime);
    free(global_styles);
    sqlite3_close_v2(projectDB);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow( window );
    window = NULL;
    //Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char *argv[])
{
  
  text_scale=2;
  char *projectfile;
  if(argc > 1)
    projectfile = argv[argc-1];
  else
    projectfile = "norge_proj.sqlite";
DEBUG_PRINT(("projectfile = %s\n",projectfile));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
/*
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,0);
*/
    SDL_Window* window = SDL_CreateWindow("noTile",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          INIT_WIDTH,INIT_HEIGHT,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    CURR_WIDTH = INIT_WIDTH;
    CURR_HEIGHT = INIT_HEIGHT;
    if (window == NULL) {
        fprintf(stderr, "Error: can't create window:  : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* Open db-connection*/
    sqlite3_initialize();

    int rc = sqlite3_open_v2(projectfile, &projectDB,SQLITE_OPEN_READONLY, NULL );

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n",
                sqlite3_errmsg(projectDB));
        sqlite3_close(projectDB);
        return 1;
    }

    /*load the db into memory*/
    loadOrSaveDb(projectDB, projectfile,0);
    
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        fprintf(stderr, "Error: SDL_GL_CreateContext : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_GL_SetSwapInterval(1);
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        fprintf(stderr, "Error: glewInit : %s", glewGetErrorString(glew_status) );
        return EXIT_FAILURE;
    }
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0");
        return EXIT_FAILURE;
    }

    if (init_resources())
        return EXIT_FAILURE;
//if (init_text_resources())
  //      return EXIT_FAILURE;

        if (init_text_resources())
        return EXIT_FAILURE;

    mainLoop(window);

    sqlite3_close(projectDB);
    free_resources(window, context);
    return EXIT_SUCCESS;
}
