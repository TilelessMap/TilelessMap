#include "theclient.h"
#include "buffer_handling.h"

int identify(GLfloat *currentBBOX, int x, int y)
{
    GLfloat w_x, w_y;
    int i;
    px2m(currentBBOX,x,y,&w_x,&w_y);
    
    printf("w_x = %f, w_y = %f\n", w_x, w_y);
    
    GLfloat box[4];
    
    box[0] = w_x;
    box[1] = w_y;
    box[2] = w_x;
    box[3] = w_y;
    
    
    LAYER_RUNTIME *theLayer;
    
    for (i = 0;i<nLayers;i++)
    {
        //reset all used buffers in our infoLayer
        reset_buffers(infoLayer);
        
        theLayer = layerRuntime + i;        
        
        //init more buffers if needed
        //Manipulate type, to not get unnessecary info
        if(theLayer->type & 30)
            infoLayer->type = 16; //set type to simple line
        else if(theLayer->type & 192)
            infoLayer->type = 128; //set type to simple point
        init_buffers(infoLayer);
        
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
        
        
        if(theLayer->type & 30)
        printf("n_geometries %zu\n", infoLayer->lines->line_start_indexes->used);
       
        
        
        
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}
