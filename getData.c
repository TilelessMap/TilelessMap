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




int get_data(SDL_Window* window,GLfloat *bbox,GLfloat *theMatrix)
{


    DEBUG_PRINT(("Entering get_data\n"));
    
    int i,t, rc;
    pthread_t threads[nLayers];
    LAYER_RUNTIME *oneLayer;
    GLfloat meterPerPixel = (bbox[2]-bbox[0])/CURR_WIDTH;
//	glClearColor(1, 1, 1, 1);
//	glClear(GL_COLOR_BUFFER_BIT);

	/* Enable blending, necessary for our alpha texture */


    for (i=0; i<nLayers; i++)
    {
        oneLayer = layerRuntime + i;

        reset_buffer(oneLayer->res_buf);
        if(oneLayer->geometryType == POLYGONTYPE)
            element_reset_buffer(oneLayer->tri_index);

        if(oneLayer->visible && oneLayer->minScale<=meterPerPixel && oneLayer->maxScale>meterPerPixel)
        {

            oneLayer->BBOX = bbox;
            rc = pthread_create(&threads[i], NULL, twkb_fromSQLiteBBOX, (void *) oneLayer);
            //twkb_fromSQLiteBBOX("veger_ea2.sqlite","veger","id",(float*) bbox, res_buf);

        }
    }
    
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for(t=0; t<nLayers; t++) 
    {
        oneLayer = layerRuntime + t;
        if(oneLayer->visible && oneLayer->minScale<=meterPerPixel && oneLayer->maxScale>meterPerPixel)
        {
            DEBUG_PRINT(("Waiting for thread %d to finish\n",t));
            rc = pthread_join(threads[t], NULL);
            if (rc) {
                printf("ERROR; return code from pthread_join() is %d\n", rc);
                exit(-1);
            }
            switch(oneLayer->geometryType)
            {
            case POINTTYPE :
                loadPoint( oneLayer, theMatrix);
                break;
            case LINETYPE :
                loadLine( oneLayer, theMatrix);
                break;
            case POLYGONTYPE :
                loadPolygon( oneLayer, theMatrix);
                break;
            }
        }
    }
    
render_txt(window);
    SDL_GL_SwapWindow(window);
//render(window,res_buf);
    return 0;
}




