#include "symbols.h"
#include "mem.h"
#include "buffer_handling.h"
#include <math.h>
#include "mem.h"

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif


int init_symbols()
{

        GLfloat* points, first_len, second_len, rotation;
        int n_dirs;
        char *symbolid;
        
        /*Create a circle*/
        symbolid = "circle";
        n_dirs = 16;
        first_len = 1;
        second_len = 1;
        rotation = 0;
        
        points = create_symbol(n_dirs,first_len, second_len, rotation);
        addsymbol(symbolid, (n_dirs + 2) * 2, points);   
        return 0;
    
}

GLfloat* create_circle(int npoints)
{
    GLfloat *res = st_malloc((npoints + 2) * 2 * sizeof(GLfloat));

    double rad;
    int i, res_pos = 0;

    //add center of point at 0,0
    res[res_pos++] = 0;
    res[res_pos++] = 0;
    for (i = 0; i<npoints/2; i++)
    {
        rad = i * M_PI/(npoints/2);
        res[res_pos++] = sin(rad);
        res[res_pos++] = cos(rad);
    }
    for (i = npoints/2; i>0; i--)
    {
        rad = i * M_PI/(npoints/2);
        res[res_pos++] = -sin(rad);
        res[res_pos++] = cos(rad);
    }
    //add first point again
    res[res_pos++] = sin(0);
    res[res_pos++] = cos(0);

    return  res;

}



GLfloat* create_symbol(int npoints, float even, float odd,float rotation)
{
    GLfloat *res = st_malloc((npoints + 2) * 2 * sizeof(GLfloat));

    double rad;
    int i, res_pos = 0;
    float e=odd;
    float rot_rad = rotation * 2 * M_PI/360;
    //add center of point at 0,0
    res[res_pos++] = 0;
    res[res_pos++] = 0;    
  for (i = 0; i<npoints; i++)
    {
        rad = rot_rad + i * 2* M_PI/npoints;
        res[res_pos++] = sin(rad)*e;
        res[res_pos++] = cos(rad)*e;
        if(e==odd)
            e = even;
        else
            e=odd;

    }
    //add first point again
    rad = rot_rad + 2 *M_PI;
    res[res_pos++] = sin(rad)*e;
    res[res_pos++] = cos(rad)*e;

    return  res;

}



