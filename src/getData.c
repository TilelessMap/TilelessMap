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
#include "buffer_handling.h"




int get_data(SDL_Window* window,MATRIX *map_matrix)
{


#ifndef _WIN32
    gettimeofday(&tval_before, NULL);
#endif
    log_this(10, "Entering get_data\n");
    int i,t, rc;
    pthread_t threads[255];
    LAYER_RUNTIME *oneLayer;
    GLfloat meterPerPixel = (map_matrix->bbox[3]-map_matrix->bbox[1])/CURR_HEIGHT;
    uint8_t type;

    for (i=0; i<nLayers; i++)
    {
        oneLayer = layerRuntime + i;
        type = oneLayer->type;
        reset_buffers(oneLayer);
        //   reset_buffer(oneLayer->res_buf);
        /*    if(oneLayer->geometryType == POLYGONTYPE)
                element_reset_buffer(oneLayer->tri_index);
        */
        if(type & 32)
            text_reset_buffer(oneLayer->text);

        if(oneLayer->visible && oneLayer->minScale<=meterPerPixel && oneLayer->maxScale>meterPerPixel)
        {
            //  log_this(10, "decode nr %d\n", i);
            oneLayer->BBOX = map_matrix->bbox;
            rc = pthread_create(&threads[i], NULL, twkb_fromSQLiteBBOX_thread, (void *) oneLayer);
            //  twkb_fromSQLiteBBOX((void *) oneLayer);
        }
    }

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    total_points=0;
    n_points=0;
    n_lines=0;
    n_polys=0;
    n_tri=0;
    n_words=0;
    n_letters=0;


    for(t=0; t<nLayers; t++)
//     for(t=0; t<0; t++)

    {


        oneLayer = layerRuntime + t;
        type = oneLayer->type;
        if(oneLayer->visible && oneLayer->minScale<=meterPerPixel && oneLayer->maxScale>meterPerPixel)
        {
            rc = pthread_join(threads[t], NULL);
            //  rc = 0;
            if (rc) {
                printf("ERROR; return code from pthread_join() is %d\n", rc);
                exit(-1);
            }
            if(type & 224)
                loadPoint( oneLayer, map_matrix->matrix);

            if(type & 24)
                loadLine( oneLayer, map_matrix->matrix);
            if(type & 6)
                loadPolygon( oneLayer, map_matrix->matrix);

        }

    }

    
    renderGPS(map_matrix->matrix);
    log_this(100, "---------------------let's rendr controls------------------------------\n");
    render_controls(controls, NULL );
    
    log_this(100, "--------back from render controls---------\n");
  //  render_simple_rect(5,75,300,225);
/*    print_txt(10,200,200,0,0,255,1, "n lines %d ", n_lines);
    print_txt(10,180,200,0,0,255,1, "n polygons %d ", n_polys);
    print_txt(10,160,200,0,0,255,1, "n triangels %d ", n_tri);
    print_txt(10,140,200,0,0,255,1, "n words %d ", n_words);
    print_txt(10,120,200,0,0,255,1, "n letters %d ", n_letters);
    print_txt(10,100,200,0,0,255,1, "Total points %d ", total_points);*/

GLfloat point_coord[2] = {10, 200};
GLfloat color[4] = {200,0,0,255};



print_txt(point_coord,NULL, color,1,0,300, "Normal text ");
printf("x = %f, y = %f\n", point_coord[0], point_coord[1]);
print_txt(point_coord,NULL, color,1,1,300, "bold text");
#ifndef _WIN32
    gettimeofday(&tval_after, NULL);

    timersub(&tval_after, &tval_before, &tval_result);
 //   print_txt(10,80,200,0,0,255,1, "time used in seconds: %ld.%06ld", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
#endif

    total_points=0;

//render_txt(window);
    SDL_GL_SwapWindow(window);

//render(window,res_buf);
    return 0;
}




