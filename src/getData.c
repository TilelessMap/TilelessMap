/**********************************************************************
 *
 * TileLessMap
 *
 * TileLessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TileLessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/


#include "theclient.h"
#include "interface/interface.h"
#include "buffer_handling.h"



int get_data(SDL_Window* window,MATRIX *map_matrix,struct CTRL *controls)
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

    for (i=0; i<global_layers->nlayers; i++)
    {

        oneLayer = global_layers->layers + i;
        //   if(oneLayer->geometryType >= RASTER)
        //     continue;
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

#if THREADING >0
            pthread_create(&threads[i], NULL, twkb_fromSQLiteBBOX_thread, (void *) oneLayer);
#else
            twkb_fromSQLiteBBOX((void *) oneLayer);
#endif
            
            
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


    for(t=0; t<global_layers->nlayers; t++)
//     for(t=0; t<0; t++)

    {


        oneLayer = global_layers->layers + t;


        type = oneLayer->type;
        if(oneLayer->visible && oneLayer->minScale<=meterPerPixel && oneLayer->maxScale>meterPerPixel)
        {
            
#if THREADING >0
            rc = pthread_join(threads[t], NULL);
#else
            rc = 0;
#endif
            if(oneLayer->geometryType >= RASTER)
                // loadRaster( oneLayer, map_matrix->matrix);
                loadandRenderRaster( oneLayer, map_matrix->matrix);
            //           continue;

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

            
    while ((err = glGetError()) != GL_NO_ERROR) {
fprintf(stderr,"0 - opengl error:%d in func %s layer %s\n", err, __func__,oneLayer->name);
}
        }

    }

    if(infoRenderLayer->visible)
        loadPolygon(infoRenderLayer, map_matrix->matrix);

    renderGPS(map_matrix->matrix);
    render_controls(controls, NULL);



#ifndef _WIN32
    gettimeofday(&tval_after, NULL);

   // timersub(&tval_after, &tval_before, &tval_result);
//   print_txt(10,80,200,0,0,255,1, "time used in seconds: %ld.%06ld", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
#endif

    total_points=0;

//render_txt(window);
    SDL_GL_SwapWindow(window);

//render(window,res_buf);
    return 0;
}




