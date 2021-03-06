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
#include "utils.h"


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
    for (i=0; i<MAX_ZOOM_FINGERS; i++)
    {
        touches[i].active = 0;
    }
    return 0;
}

int register_touch_down(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y)
{
    int i = 0;
    FINGEREVENT *fie;
    for (i=0; i<MAX_ZOOM_FINGERS; i++)
    {
        if(i<MAX_ZOOM_FINGERS)
        {
            if(!touches[i].active)
            {
                break;
            }
        }
        else
        {
            i = MAX_ZOOM_FINGERS;
            break;
        }

    }




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
    for (i=0; i<MAX_ZOOM_FINGERS; i++)
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

int register_motion(FINGEREVENT *touches, int64_t fingerid, GLfloat x, GLfloat y)
{
    int i = 0;
    FINGEREVENT *fie;
    for (i=0; i<MAX_ZOOM_FINGERS; i++)
    {
        fie = &(touches[i]);
        if (fie->fingerid == fingerid )
        {
            fie->x2 = x;
            fie->y2 = y;
        }
    }

    return 0;
}



int get_box_from_touches(FINGEREVENT *touches,MATRIX *matrix_hndl,MATRIX *ref)
{
    GLfloat w_x1,w_y1,w_x2,w_y2;
    GLfloat w_dist, px_dist;
    GLint p1x1, p1x2, p1y1, p1y2;
    GLint p2x1, p2x2, p2y1, p2y2;

    GLint deltax_px, deltay_px;
    GLfloat deltax_w, deltay_w, ratio;

    //x at first points
    p1x1 = (GLint) (touches[0].x1 * CURR_WIDTH);
    p2x1 = (GLint)(touches[1].x1 * CURR_WIDTH);

    //y at first points
    p1y1 = (GLint)(touches[0].y1 * CURR_HEIGHT);
    p2y1 = (GLint)(touches[1].y1 * CURR_HEIGHT);

    //x at second points
    if(matrix_hndl->horizontal_enabled)
    {
        p1x2 = (GLint)(touches[0].x2 * CURR_WIDTH);
        p2x2 = (GLint)(touches[1].x2 * CURR_WIDTH);
    }
    else
    {
        p1x2 = p1x1;
        p2x2 = p2x1;
    }

    //y at second points
    if(matrix_hndl->vertical_enabled)
    {
        p1y2 = (GLint)(touches[0].y2 * CURR_HEIGHT);
        p2y2 = (GLint)(touches[1].y2 * CURR_HEIGHT);
    }
    else
    {
        p1y2 = p1y1;
        p2y2 = p2y1;
    }
    px2m(ref->bbox,p1x1,p1y1,&w_x1,&w_y1);
    px2m(ref->bbox,p2x1,p2y1,&w_x2,&w_y2);

    deltax_w = w_x2 - w_x1;
    deltay_w = w_y2 - w_y1;
    w_dist = sqrtf(deltax_w * deltax_w + deltay_w * deltay_w);
    deltax_px = p2x2 - p1x2;
    deltay_px = p2y2 - p1y2;
    px_dist = sqrtf((float) (1.0 * deltax_px * deltax_px + deltay_px * deltay_px));

    if(matrix_hndl->zoom_enabled)
        ratio = w_dist/px_dist;
    else
        ratio = 1;

    matrix_hndl->bbox[0] = w_x1 - ratio * p1x2;
    matrix_hndl->bbox[1] = w_y1 - ratio * (CURR_HEIGHT - p1y2);
    matrix_hndl->bbox[2] = matrix_hndl->bbox[0] + ratio * CURR_WIDTH;
    matrix_hndl->bbox[3] = matrix_hndl->bbox[1] + ratio * CURR_HEIGHT;
    return 0;
}



