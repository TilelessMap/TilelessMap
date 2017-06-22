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

#ifdef __ANDROID__
#include <jni.h>
#endif

void initialBBOX(GLfloat x, GLfloat y, GLfloat width, GLfloat *newBBOX)
{
    GLfloat ratio = (GLfloat) CURR_HEIGHT/ (GLfloat) CURR_WIDTH;
    GLfloat height = width * ratio;
    newBBOX[0] = x-width/2;
    newBBOX[1] = y-height/2;
    newBBOX[2] = x+width/2;
    newBBOX[3] = y+height/2;
}

void copyNew2CurrentBBOX(GLfloat *newBBOX,GLfloat *currentBBOX)
{
    currentBBOX[0] = newBBOX[0];
    currentBBOX[1] = newBBOX[1];
    currentBBOX[2] = newBBOX[2];
    currentBBOX[3] = newBBOX[3];
    return;
}

void windowResize(int newWidth,int newHeight,GLfloat *currentBBOX, GLfloat *newBBOX)
{
    GLfloat meterPerPixel = (currentBBOX[2] - currentBBOX[0]) / (GLfloat) CURR_WIDTH;
    GLfloat oldCenterX = currentBBOX[0] + (currentBBOX[2] - currentBBOX[0])/2;
    GLfloat oldCenterY = currentBBOX[1] + (currentBBOX[3] - currentBBOX[1])/2;

    newBBOX[0] = oldCenterX - meterPerPixel * newWidth / 2;
    newBBOX[1] = oldCenterY - meterPerPixel * newHeight / 2;
    newBBOX[2] = oldCenterX + meterPerPixel * newWidth / 2;
    newBBOX[3] = oldCenterY + meterPerPixel * newHeight / 2;
    CURR_WIDTH = newWidth;
    CURR_HEIGHT = newHeight;

    return;
}



int multiply_matrices(GLfloat *matrix1,GLfloat *matrix2, GLfloat *theMatrix)
{
    int m1, m2, i;
    GLfloat  val=0; //m1o, m2o,

    memset(theMatrix, 0,sizeof(GLfloat)*16);
    for (i = 0; i<4; i++)
    {
        for (m2 = 0; m2<4; m2++)
        {
            val = 0;
            for (m1 = 0; m1<4; m1++)
            {
                val +=  matrix1[m2 + m1*4] * matrix2[m1 + 4 * i];
            }
            theMatrix[m2 + 4 * i] = val;
        }
    }
    return 0;
}


int px2m(GLfloat *bbox,GLint px_x,GLint px_y,GLfloat *w_x,GLfloat *w_y)
{

    log_this(10, "Entering get_bbox\n");

    GLfloat width = bbox[2]-bbox[0];
    GLfloat height = bbox[3]-bbox[1];
    *w_x = bbox[0] + px_x * (width/CURR_WIDTH);
    *w_y = bbox[3] - px_y * (height/CURR_HEIGHT);

    return 0;
}


int calc_translate(GLfloat w_x,GLfloat w_y, GLfloat *transl)
{


    log_this(10, "Entering calc_translate\n");

    memset(transl, 0,sizeof(float)*16);

    transl[0] = 1;
    transl[5] = 1;
    transl[10] = 1;
    transl[12] = -w_x;
    transl[13] = -w_y;
    transl[15] = 1;

    return 0;
}

GLfloat* create_circle(int npoints)
{
    GLfloat *res = malloc((npoints + 2) * 2 * sizeof(GLfloat));

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
int calc_scale(GLfloat *bbox, GLfloat zoom, GLfloat *scale)
{

    log_this(10, "Entering calc_scale\n");

    memset(scale, 0,sizeof(float)*16);

    GLfloat width = bbox[2]-bbox[0];
    GLfloat height = bbox[3]-bbox[1];

    scale[0] = (GLfloat) 2.0/width/zoom;
    scale[5] = (GLfloat) 2.0/height/zoom;
    scale[10] = (GLfloat) 1; //we set z-scaling 10 0.01 so we can give z-values from 0 to 100
    scale[15] = 1;
    return 0;
}



#ifdef __ANDROID__
JNIEXPORT void JNICALL Java_no_jordogskog_app_gps_onNativeGPS(
    JNIEnv* env, jclass jcls,
    double latitude, double longitude, double acc)
{
    gps_in(latitude, longitude, acc);

}
#endif


