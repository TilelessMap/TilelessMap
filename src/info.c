#include "theclient.h"
#include "buffer_handling.h"
#include "info.h"

int identify(GLfloat *currentBBOX, int x, int y,SDL_Window* window,GLfloat *theMatrix)
{
    log_this(100,"info, x=%d, y=%d\n",x,y);
    GLfloat w_x, w_y;
    int i, z, r, n_vals_acc, n_vals, poly_start_index , pa_start_index, n_of_pa,n_polys, n_points,next_pa ;
    int inside,n_dims, nrings,n_elements_acc,n_elements;
    px2m(currentBBOX,x,y,&w_x,&w_y);
    
    GLfloat meterPerPixel = (currentBBOX[2]-currentBBOX[0])/CURR_WIDTH;
//    printf("w_x = %f, w_y = %f\n", w_x, w_y);
    
    GLfloat box[4];
    GLfloat point[2];
    
    box[0] = point[0] = w_x;
    box[1] = point[1] = w_y;
    box[2] = w_x;
    box[3] = w_y;
    
    
    
    LAYER_RUNTIME *theLayer;
                 POLYGON_LIST *poly = infoLayer->polygons;
            POLYGON_LIST *renderpoly = infoRenderLayer->polygons;
            
        reset_buffers(infoRenderLayer);
    for (i = 0;i<nLayers;i++)
    {
        theLayer = layerRuntime + i;  
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
        
        //borrow prepared statement
        infoLayer->preparedStatement = theLayer->preparedStatement;
        
        //If we in the future will handle 3D, we are prepared
        infoLayer->n_dims = theLayer->n_dims;
        
        //get the right UTM-zone and hemisphere
        infoLayer->utm_zone = theLayer->utm_zone;
        infoLayer->hemisphere = theLayer->hemisphere;
        

        
        //add the "box, well it is just a point, but it will do
        infoLayer->BBOX = box;
        
        twkb_fromSQLiteBBOX(infoLayer);
        
       
        if(theLayer->type & 6)
        {
            
            poly_start_index = 0;
            pa_start_index = 0;
            n_of_pa = 0;
            n_dims = infoLayer->n_dims;
            
            

             n_polys = poly->polygon_start_indexes->used;
        r = 0;    
        pa_start_index = 0;
       int n_rings, next_poly;
       if(n_polys)
            n_points = (poly->pa_start_indexes->list[0])/n_dims;
        for (z=0;z<n_polys; z++)
        {
            nrings = 0;
            n_elements = 0;
            n_elements_acc = 0;
            poly_start_index = poly->polygon_start_indexes->list[z];
            if(z<n_polys-1)
                next_poly = poly->polygon_start_indexes->list[z+1];
            else
                next_poly = poly->vertex_array->used;
            
               
            inside=0;
            if(wn_PnPoly( point,poly->vertex_array->list + poly_start_index, n_points , n_dims))
            {
                inside = 1;
                printf("inside outer ring");
                nrings++;
                
                    next_pa = *(poly->pa_start_indexes->list + r);
                r++;
                while(next_pa<next_poly)
                {
                    
                        pa_start_index = next_pa;
                    next_pa = *(poly->pa_start_indexes->list + r);
                    r++;
                    n_points = (next_pa - pa_start_index)/n_dims;
                    
                    if(wn_PnPoly( point,poly->vertex_array->list + pa_start_index,n_points , n_dims))
                    {
                        inside = 0;   
                        //Ok, we are inside a hole, let's fast forward through the rest of the holes
                        while(next_pa<next_poly)
                        {
                                next_pa = *(poly->pa_start_indexes->list + r);
                                r++;
                        }    
                        
                        
                    }
                    
                }
                
                if(inside)
                {
                     add2gluint_list(renderpoly->polygon_start_indexes, renderpoly->vertex_array->used); //register start of new polygon to render
                     addbatch2glfloat_list(renderpoly->vertex_array, next_poly-poly_start_index, poly->vertex_array->list + pa_start_index); //memcpy all vertexes in polygon
                     
                     
                     
                    n_elements = *(poly->element_start_indexes->list + z) - n_elements_acc;
                    
                     addbatch2glushort_list(renderpoly->element_array, n_elements, poly->element_array->list + n_elements_acc); //memcpy all vertexes in polygon
                     add2gluint_list(renderpoly->element_start_indexes, renderpoly->element_array->used); //register start of new polygon to render
                    
                    n_elements_acc = *(poly->element_start_indexes->list + z);
                    
                }
                
                        
                }                
                }   
                
            }
    
    
        }
    
    
    }
    
    setzero2gluint_list(renderpoly->area_style_id, renderpoly->polygon_start_indexes->used);
    
    
    render_info(window,theMatrix);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}
































