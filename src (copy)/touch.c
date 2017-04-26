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



FINGEREVENT* init_touch_que()
{

    FINGEREVENT *touches;
    touches = malloc(MAX_ZOOM_FINGERS * sizeof(FINGEREVENT));
    reset_touch_que(touches);
    return touches;
}

int reset_touch_que(FINGEREVENT *touches)
{
    int i;
    for (i=0;i<MAX_ZOOM_FINGERS;i++)
    {
        touches[i].active = 0;
    }   
    return 0;
}

int register_touch_down(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y)
{
        int i = 0;
        FINGEREVENT *fie;
        while (i<MAX_ZOOM_FINGERS && touches[i].active)
        {
            i++;
        };
        
        if(i<MAX_ZOOM_FINGERS)
        {
            fie = &(touches[i]);
            fie->active = 1;
            fie->fingerid = fingerid;
            fie->x1 = x;
            fie->y1 = y;
        }    
        return 0;
}

int register_touch_up(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y)
{
        int i = 0;
        FINGEREVENT *fie;
        int all_finnished = 1;
        for (i=0;i<MAX_ZOOM_FINGERS;i++)
        {
            
            fie = &(touches[i]);
            if (fie->fingerid == fingerid && (fie->x1 != x || fie->y1 != y))
            {
                if (fie->active == 1)
                {
                    fie->active = 2;  
                    fie->x2 = x;
                    fie->y2 = y;                    
                }      
                else
                    fie->active = 0;
            }
            
            if (fie->active != 2)
                all_finnished = 0;
        }           
        
        if(all_finnished)
            return 1;
        else
            return 0;
}



int get_box_from_touches(FINGEREVENT *touches,GLfloat *currentBBOX,GLfloat *newBBOX)
{
    GLfloat w_x1,w_y1,w_x2,w_y2;
    GLfloat w_dist, px_dist;
    double deltax, deltay, ratio;
    px2m(currentBBOX,touches[0].x1,touches[0].y1,&w_x1,&w_y1);
    px2m(currentBBOX,touches[1].x1,touches[1].y1,&w_x2,&w_y2);
    
    deltax = w_x2 - w_x1;
    deltay = w_y2 - w_y1;
    w_dist = sqrtf(deltax * deltax + deltay * deltay);
    
    deltax = touches[1].x2 - touches[1].x1;
    deltay = touches[1].y2 - touches[1].y1;
    px_dist = sqrtf(deltax * deltax + deltay * deltay);
    
    ratio = w_dist/px_dist;
    
    newBBOX[0] = w_x1 - ratio * touches[0].x2;
    newBBOX[1] = w_y1 - ratio * (CURR_HEIGHT - touches[0].y2);
    newBBOX[2] = newBBOX[0] + ratio * CURR_WIDTH;
    newBBOX[3] = newBBOX[1] + ratio * CURR_HEIGHT;
    return 0;
}



