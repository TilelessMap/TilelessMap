
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
#include "interface/interface.h"
#include "info.h"
#include "matrix_handling.h"
#include "log.h"

void mainLoop(SDL_Window* window)
{
    log_this(100, "Entering mainLoop\n");

    GLfloat tx,ty,pr;
    int ti, fi;
    GLfloat tmp_box[4] = {0.0,0.0,0.0,0.0};
    //GLfloat newBBOX[4] = {0.0,0.0,0.0,0.0};
    int mouse_down = 0;
    int wheel_y;
    SDL_Event ev;
    GPSEventType = ((Uint32)-1);
    SDL_Event tmp_ev[10];
    int n_events;
    GLint px_x_clicked,px_y_clicked;

    FINGEREVENT *touches = init_touch_que();

    MATRIX map_matrix;
    MATRIX ref;
    reset_matrix(&map_matrix);
    reset_matrix(&ref);


    map_matrix.horizontal_enabled = 1;
    map_matrix.vertical_enabled = 1;
    map_matrix.zoom_enabled = 1;


    gps_point.x = 0;
    gps_point.y = 0;
    gps_point.s = 0;
    int map_modus_before=1;
    int mouse_down_x = 0, mouse_down_y = 0,mouse_up_x, mouse_up_y;

    incharge = NULL; //this means map is in charge when events occur.

//     initialBBOX(380000, 6660000, 300000, newBBOX);
//initialBBOX(230000, 6660000, 5000, newBBOX);
//  initialBBOX(325000, 6800000, 800000, newBBOX);
    //  initialBBOX(234000, 895000, 5000, newBBOX);


    //initialBBOX(init_x, init_y, init_box_width, newBBOX);
    initialBBOX(init_x, init_y, init_box_width, &map_matrix);
    initialBBOX(init_x, init_y, init_box_width, &ref); //just for saftey we init the values in ref too
    GLfloat theMatrix[16];

//    matrixFromBBOX(newBBOX, theMatrix);
    matrixFromBBOX(&map_matrix);


    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"opengl error 65 :%d\n", err);
    }

//    get_data(window, newBBOX, theMatrix);
    get_data(window, &map_matrix);

    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
        fprintf(stderr,"error on return: %d\n", err);
    }
    //  copyNew2CurrentBBOX(newBBOX, currentBBOX);

    while (1)
    {
        if (SDL_WaitEvent(&ev)) /* execution suspends here while waiting on an event */
        {

            if(ev.type == GPSEventType)
            {
                render_data(window, map_matrix.matrix);
            }
            else
            {
                switch (ev.type)
                {
#ifndef __ANDROID__
                case SDL_MOUSEBUTTONDOWN:
                    mouse_down = 1;
                    mouse_down_x = ev.button.x;
                    mouse_down_y = ev.button.y;
                    if(incharge)
                        copy2ref_box(incharge->matrix_handler,&ref);
                    else
                        copy2ref_box(&map_matrix,&ref);

                    break;
                case SDL_MOUSEBUTTONUP:
                    mouse_down = 0;
                    mouse_up_x = ev.button.x;
                    mouse_up_y = ev.button.y;
                    if(mouse_down_x == mouse_up_x && mouse_down_y == mouse_up_y)
                    {
                      //  map_modus_before = map_modus;
                        int any_hit = check_click(mouse_up_x, mouse_up_y);


                        if(! map_modus && !any_hit)
                        {
                            identify(&map_matrix, mouse_up_x,mouse_up_y,window);
                           //render_data(window, map_matrix.matrix);
                        }
                        render_data(window, map_matrix.matrix);
                        
                    }
                    else
                    {
                      /*  if(map_modus)
                        {*/
                            if(!incharge)
                            {
                                matrixFromDeltaMouse(&map_matrix,&ref, mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                get_data(window, &map_matrix);
                                //  copyNew2CurrentBBOX(newBBOX, currentBBOX);
                            }
                            else
                            {
                                matrixFromDeltaMouse(incharge->matrix_handler,&ref, mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                render_data(window, map_matrix.matrix);
                            }
                        //}

                    }
                    break;
                case SDL_MOUSEWHEEL:
                    wheel_y = ev.wheel.y;

                    SDL_GetMouseState(&px_x_clicked, &px_y_clicked);
                      /*  if(map_modus)
                        {*/
                        if(wheel_y > 0)
                            matrixFromBboxPointZoom(&map_matrix,&map_matrix,px_x_clicked, px_y_clicked, 0.5);
                        else
                            matrixFromBboxPointZoom(&map_matrix,&map_matrix,px_x_clicked, px_y_clicked, 2);

                        get_data(window, &map_matrix);

                        // copyNew2CurrentBBOX(newBBOX, currentBBOX);
                    //}
                    break;

                case SDL_MOUSEMOTION:
                      /*  if(map_modus)
                        {*/
                        if(mouse_down)
                        {
                            n_events = 	SDL_PeepEvents(tmp_ev,3,SDL_PEEKEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT);

                            if(n_events<2)
                            {
                                mouse_up_x = ev.motion.x;
                                mouse_up_y = ev.motion.y;
                                if(!incharge)
                                {
                                    matrixFromDeltaMouse(&map_matrix,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                    render_data(window, map_matrix.matrix);
                                }
                                else
                                {
                                    matrixFromDeltaMouse(incharge->matrix_handler,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                    render_data(window, map_matrix.matrix);
                                }

                                //         copyNew2CurrentBBOX(newBBOX, currentBBOX);
                                while ((err = glGetError()) != GL_NO_ERROR) {
                                    log_this(10, "Problem 2\n");
                                    fprintf(stderr,"opengl error aaa999: %d\n", err);
                                }

                            }

                        }

                    //}
                    /*  else
                     {
                              if(n_events<2)
                             {

                                 mouse_up_x = ev.motion.x;
                                 mouse_up_y = ev.motion.y;

                         identify(&map_matrix, mouse_up_x,mouse_up_y,window);
                             }
                     }*/

                    break;

#endif

                case SDL_FINGERDOWN:

                    ti = (int) ev.tfinger.touchId;
                    fi = (int) ev.tfinger.fingerId;
                    tx = ev.tfinger.x;
                    ty = ev.tfinger.y;
                    pr = ev.tfinger.pressure;
                    //DEBUG_PRINT(("DOWN: fi=%d, x = %f, y = %f, pr = %f, ti = %d\n",fi, tx, ty,pr,ti));

                    register_touch_down(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
                    if(incharge)
                        copy2ref_box(incharge->matrix_handler,&ref);
                    else
                        copy2ref_box(&map_matrix,&ref);



                    break;
                case SDL_FINGERUP:
                    log_this(10,"SDL_FINGERUP");

                    ti = (int) ev.tfinger.touchId;
                    fi = (int) ev.tfinger.fingerId;
                    tx = ev.tfinger.x;
                    ty = ev.tfinger.y;
                    pr = ev.tfinger.pressure;



                    if(abs(mouse_down_x-mouse_up_x) < 10 && abs(mouse_down_y-mouse_up_y)<10)
                    {
                        map_modus_before = map_modus;
                        check_click((GLint) (tx * CURR_WIDTH), (GLint)(ty * CURR_HEIGHT));


                        render_data(window, map_matrix.matrix);

                        if(!map_modus && ! map_modus_before && !incharge)
                        {
                            identify(&map_matrix, (GLint) (tx * CURR_WIDTH), (GLint)(ty * CURR_HEIGHT),window);
                            render_data(window, map_matrix.matrix);
                        }
                        reset_touch_que(touches);
                    }
                    else
                    {
                        if(map_modus)
                        {
                            if(touches[1].active) //check if at least 2 fingers are activated
                            {
                                if(register_touch_up(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y))
                                {
                                    if(!incharge)
                                    {
                                        get_box_from_touches(touches, &map_matrix, &ref);
                                        matrixFromBBOX(&map_matrix);
                                        get_data(window, &map_matrix);

                                    }
                                    else
                                    {

                                        get_box_from_touches(touches, incharge->matrix_handler,&ref);
                                        matrixFromBBOX(incharge->matrix_handler);
                                        render_data(window, map_matrix.matrix);
                                    }

                                    reset_touch_que(touches);
                                    //   copyNew2CurrentBBOX(newBBOX, currentBBOX);
                                }
                            }

                            else
                            {
                                //	  mouse_down = 0;
                                mouse_up_x = (GLint) (tx * CURR_WIDTH);
                                mouse_up_y = (GLint)(ty * CURR_HEIGHT);
                                mouse_down_x = (GLint)(touches[0].x1 * CURR_WIDTH);
                                mouse_down_y = (GLint)(touches[0].y1 * CURR_HEIGHT);
                                if(!incharge)
                                {
                                    matrixFromDeltaMouse(&map_matrix,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                    get_data(window, &map_matrix);
                                }
                                else
                                {
                                    matrixFromDeltaMouse(incharge->matrix_handler,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                    render_data(window, map_matrix.matrix);
                                }

                                reset_touch_que(touches);

                                //render_data(window, newBBOX, theMatrix);

                                //      copyNew2CurrentBBOX(newBBOX, currentBBOX);

                            }
                        }
                        else
                            reset_touch_que(touches);
                    }

                    break;

                case SDL_FINGERMOTION:
                    log_this(10,"SDL_FINGERMOTION");
                    n_events = 	SDL_PeepEvents(tmp_ev,3,SDL_PEEKEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT);
                    if(map_modus)
                    {
                        if(touches[1].active) //check if at least 2 fingers are activated
                        {

                            register_motion(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);

                            if(!incharge)
                            {
                                get_box_from_touches(touches, &map_matrix, &ref);
                                matrixFromBBOX(&map_matrix);
                            }
                            else
                            {
                                get_box_from_touches(touches, incharge->matrix_handler, &ref);
                                matrixFromBBOX(incharge->matrix_handler);
                            }


                            if(n_events<2)
                                render_data(window, map_matrix.matrix);


                        }
                        else
                        {
                            log_this(10, "m1");
                            //~ android_log_print(ANDROID_LOG_INFO, APPNAME, "m1");
                            ti = (int) ev.tfinger.touchId;
                            fi = (int) ev.tfinger.fingerId;
                            tx = ev.tfinger.x;
                            ty = ev.tfinger.y;
                            pr = ev.tfinger.pressure;


                            //	  mouse_down = 0;
                            mouse_up_x = (GLint) (tx * CURR_WIDTH);
                            mouse_up_y = (GLint)(ty * CURR_HEIGHT);
                            mouse_down_x = (GLint)(touches[0].x1 * CURR_WIDTH);
                            mouse_down_y = (GLint)(touches[0].y1 * CURR_HEIGHT);
                            if(incharge)
                            {
                                matrixFromDeltaMouse(incharge->matrix_handler,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                            }
                            else
                            {
                                matrixFromDeltaMouse(&map_matrix,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                            }

                            if(n_events<2)
                                render_data(window, map_matrix.matrix);
                            //         copyNew2CurrentBBOX(newBBOX, currentBBOX);
                        }
                    }
                    break;





                case SDL_WINDOWEVENT:
                    if (ev.window.event  == SDL_WINDOWEVENT_RESIZED)
                    {
                        windowResize(ev.window.data1,ev.window.data2,&map_matrix,&map_matrix);
//                        copyNew2CurrentBBOX(newBBOX, currentBBOX);

                        glViewport(0,0,CURR_WIDTH, CURR_HEIGHT);

                    }
                    break;

                case SDL_QUIT:
                    free(touches);
                    return;
                }
            }
        }
        //      render_data(window,currentBBOX,theMatrix);
    }
    free(touches);
    return ;
}

void free_resources(SDL_Window* window,SDL_GLContext context) {
    log_this(10, "Entering free_resources\n");

    glDeleteProgram(std_program);
    glDeleteProgram(txt_program);
    glDeleteProgram(lw_program);

    destroy_layer_runtime(layerRuntime,nLayers);
    destroy_layer_runtime(infoLayer,1);
    free(gps_circle);

    destroy_font(fonts[0]);
    
/*    free(font_normal[0]);
    free(font_normal[1]);
    free(font_normal[2]);
    free(font_bold[0]);
    free(font_bold[1]);
    free(font_bold[2]);*/


    destroy_wc_txt(tmp_unicode_txt);
    FT_Done_FreeType(ft);
    free(global_styles);
    sqlite3_close_v2(projectDB);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow( window );
    window = NULL;
    //Quit SDL subsystems
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
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


    glDisable (GL_DEPTH_TEST);


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

    int rc = sqlite3_open_v2(projectfile, &projectDB,SQLITE_OPEN_READWRITE, NULL );

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


    if (init_text_resources())
    {
        log_this(1,"Problems in init_text_resources");
        return EXIT_FAILURE;
    }



    if (init_resources(dir))
        return EXIT_FAILURE;
//if (init_text_resources())
    //      return EXIT_FAILURE;






    mainLoop(window);

    sqlite3_close(projectDB);
    free_resources(window, context);
    return EXIT_SUCCESS;
}
