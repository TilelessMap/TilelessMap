
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
#include "utils.h"

void mainLoop(SDL_Window* window,struct  CTRL *controls)
{
    log_this(100, "Entering mainLoop now\n");
//return;
    GLfloat tx,ty;
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
    int mouse_down_x = 0, mouse_down_y = 0,mouse_up_x, mouse_up_y;

    incharge = NULL; //this means map is in charge when events occur.

//     initialBBOX(380000, 6660000, 300000, newBBOX);
//initialBBOX(230000, 6660000, 5000, newBBOX);
//  initialBBOX(325000, 6800000, 800000, newBBOX);
    //  initialBBOX(234000, 895000, 5000, newBBOX);


    //initialBBOX(init_x, init_y, init_box_width, newBBOX);
    initialBBOX(init_x, init_y, init_box_width, &map_matrix);
    initialBBOX(init_x, init_y, init_box_width, &ref); //just for saftey we init the values in ref too

//    matrixFromBBOX(newBBOX, theMatrix);
    matrixFromBBOX(&map_matrix);


    while ((err = glGetError()) != GL_NO_ERROR) {
        log_this(10, "Problem 2\n");
       while ((err = glGetError()) != GL_NO_ERROR) {
	fprintf(stderr,"0 - opengl error:%d in func %s\n", err, __func__);
}
    }

//    get_data(window, newBBOX, theMatrix);
    get_data(window, &map_matrix, controls);

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
                render_data(window, map_matrix.matrix, controls);
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
                        int any_hit = check_click(controls, mouse_up_x, mouse_up_y);


                        if(! map_modus && !any_hit)
                        {
                            identify(&map_matrix, mouse_up_x,mouse_up_y);
                            //render_data(window, map_matrix.matrix);
                        }
                        render_data(window, map_matrix.matrix, controls);

                    }
                    else
                    {
                        /*  if(map_modus)
                          {*/
                        if(!incharge)
                        {
                            matrixFromDeltaMouse(&map_matrix,&ref, mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                            get_data(window, &map_matrix, controls);
                            //  copyNew2CurrentBBOX(newBBOX, currentBBOX);
                        }
                        else
                        {
                            matrixFromDeltaMouse(incharge->matrix_handler,&ref, mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                            render_data(window, map_matrix.matrix, controls);
                        }
                        //}

                    }
                    break;
                case SDL_MOUSEWHEEL:
                    wheel_y = ev.wheel.y;

                    SDL_GetMouseState(&px_x_clicked, &px_y_clicked);
                    /*  if(map_modus)
                      {*/
                    if(incharge)
                    {
                        
                        if(wheel_y > 0)
                            matrixFromBboxPointZoom(incharge->matrix_handler,incharge->matrix_handler,px_x_clicked, px_y_clicked, 0.5);
                        else
                            matrixFromBboxPointZoom(incharge->matrix_handler,incharge->matrix_handler,px_x_clicked, px_y_clicked, 2);

                    }
                    else
                    {
                        
                        if(wheel_y > 0)
                            matrixFromBboxPointZoom(&map_matrix,&map_matrix,px_x_clicked, px_y_clicked, 0.5);
                        else
                            matrixFromBboxPointZoom(&map_matrix,&map_matrix,px_x_clicked, px_y_clicked, 2);

                    }
                   
                    get_data(window, &map_matrix, controls);

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
                             //   render_data(window, map_matrix.matrix, controls);
                            }
                            else
                            {
                                matrixFromDeltaMouse(incharge->matrix_handler,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                            //    render_data(window, map_matrix.matrix, controls);
                            }
                                render_data(window, map_matrix.matrix, controls);
                     
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

                    tx = ev.tfinger.x;
                    ty = ev.tfinger.y;

                    register_touch_down(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
                    if(incharge)
                        copy2ref_box(incharge->matrix_handler,&ref);
                    else
                        copy2ref_box(&map_matrix,&ref);



                    break;
                case SDL_FINGERUP:
                    log_this(10,"SDL_FINGERUP");

                    tx = ev.tfinger.x;
                    ty = ev.tfinger.y;

                    int tolerance = (int) (10 * size_factor);

                    if(abs(mouse_down_x-mouse_up_x) < tolerance && abs(mouse_down_y-mouse_up_y)<tolerance && !touches[1].active)
                    {

                        int any_hit = check_click(controls, (GLint) (tx * CURR_WIDTH), (GLint)(ty * CURR_HEIGHT));
                        if(! map_modus && !any_hit)
                        {
                            identify(&map_matrix, (GLint) (tx * CURR_WIDTH), (GLint)(ty * CURR_HEIGHT));
                        }
                        render_data(window, map_matrix.matrix, controls);
                        reset_touch_que(touches);
                    }
                    else
                    {
                        /*  if(map_modus)
                          {*/
                        if(touches[1].active) //check if at least 2 fingers are activated
                        {
                            if(register_touch_up(touches, ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y))
                            {
                                if(!incharge)
                                {
                                    get_box_from_touches(touches, &map_matrix, &ref);
                                    matrixFromBBOX(&map_matrix);
                                    get_data(window, &map_matrix, controls);

                                }
                                else
                                {

                                    get_box_from_touches(touches, incharge->matrix_handler,&ref);
                                    matrixFromBBOX(incharge->matrix_handler);
                                    render_data(window, map_matrix.matrix, controls);
                                }

                                reset_touch_que(touches);
                                //   copyNew2CurrentBBOX(newBBOX, currentBBOX);
                            }
                        }

                        else// if(touches[0].active)
                        {
                            //	  mouse_down = 0;
                            mouse_up_x = (GLint) (tx * CURR_WIDTH);
                            mouse_up_y = (GLint)(ty * CURR_HEIGHT);
                            mouse_down_x = (GLint)(touches[0].x1 * CURR_WIDTH);
                            mouse_down_y = (GLint)(touches[0].y1 * CURR_HEIGHT);
                            if(!incharge)
                            {
                                matrixFromDeltaMouse(&map_matrix,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                get_data(window, &map_matrix, controls);
                            }
                            else
                            {
                                matrixFromDeltaMouse(incharge->matrix_handler,&ref,mouse_down_x,mouse_down_y,mouse_up_x,mouse_up_y);
                                render_data(window, map_matrix.matrix, controls);
                            }

                            reset_touch_que(touches);

                            //render_data(window, newBBOX, theMatrix);

                            //      copyNew2CurrentBBOX(newBBOX, currentBBOX);

                        }
                        /*}
                        else
                            reset_touch_que(touches);*/
                    }

                    break;

                case SDL_FINGERMOTION:
                    log_this(10,"SDL_FINGERMOTION");
                    n_events = 	SDL_PeepEvents(tmp_ev,3,SDL_PEEKEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT);
                    /* if(map_modus)
                     {*/
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
                            render_data(window, map_matrix.matrix, controls);


                    }
                    else
                    {
                        log_this(10, "m1");
                        tx = ev.tfinger.x;
                        ty = ev.tfinger.y;


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
                            render_data(window, map_matrix.matrix, controls);
                        //         copyNew2CurrentBBOX(newBBOX, currentBBOX);
                    }
                    //}
                    break;





                case SDL_WINDOWEVENT:
                    if (ev.window.event  == SDL_WINDOWEVENT_RESIZED)
                    {
                        windowResize(ev.window.data1,ev.window.data2,&map_matrix,&map_matrix);
//                        copyNew2CurrentBBOX(newBBOX, currentBBOX);

                        glViewport(0,0,CURR_WIDTH, CURR_HEIGHT);

                        check_screen_size();
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

