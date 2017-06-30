#include "theclient.h"
#include "buffer_handling.h"
#include "info.h"

int identify(GLfloat *currentBBOX, int x, int y,SDL_Window* window,GLfloat *theMatrix)
{
    log_this(10,"info, x=%d, y=%d\n",x,y);
    GLfloat w_x, w_y;
    int i, z, r, n_vals_acc, n_vals, poly_start_index , pa_start_index, n_of_pa,n_polys, n_points,next_pa ;
    int inside,n_dims, nrings,n_elements_acc,n_elements;
    px2m(currentBBOX,x,y,&w_x,&w_y);
    int64_t id; 
    GLfloat meterPerPixel = (currentBBOX[2]-currentBBOX[0])/CURR_WIDTH;
   printf("w_x = %f, w_y = %f\n", w_x, w_y);
    
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
        printf("layer name %s\n",theLayer->name);
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
            
    
            n_dims = infoLayer->n_dims;
            
      /*    for (r=0;r<poly->pa_start_indexes->used; r++)
        {
            
         printf("r = %d, pa_start = %d, id = %ld\n", r, poly-> );   
        }*/
        next_pa = 0;
        r = 0; 
        
int poly_n = 0;
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
uint pa;
inside = 0;
        for (pa=0;pa<poly->pa_start_indexes->used; pa++)
        {
        printf("poly_number = %d and id = %ld\n", poly_n, infoLayer->twkb_id->list[pa]);
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
        printf("next_polystart = %d, next_pa_start = %d\n", next_polystart, next_pa_start);
         if(next_polystart == next_pa_start)
        {            
                if(inside)
                {
                    log_this(100,"ok, poly for rendering");
                     add2gluint_list(renderpoly->polygon_start_indexes, renderpoly->vertex_array->used); //register start of new polygon to render
                     addbatch2glfloat_list(renderpoly->vertex_array, next_polystart - curr_poly_start, poly->vertex_array->list + curr_poly_start); //memcpy all vertexes in polygon
                     
                     
                     
                    n_elements = *(poly->element_start_indexes->list + poly_n) - n_elements_acc;
                    
                     addbatch2glushort_list(renderpoly->element_array, n_elements, poly->element_array->list + n_elements_acc); //memcpy all vertexes in polygon
                     add2gluint_list(renderpoly->element_start_indexes, renderpoly->element_array->used); //register start of new polygon to render
                    
                    
                    
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
    
    setzero2gluint_list(renderpoly->area_style_id, renderpoly->polygon_start_indexes->used);
    
    
    render_info(window,theMatrix);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}
































