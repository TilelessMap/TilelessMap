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


void gps_in(double latitude, double longitude, double acc)
{
    if(GPSEventType == ((Uint32)-1))
        GPSEventType = SDL_RegisterEvents(1);



    log_this(100, " let's reproj: lat = %lf, lon = %lf", latitude, longitude);

    int hemi = 0;
    if(longitude < 0)
        hemi = 1;

    GLfloat reproj_coord[2];
    reproj_coord[0] = latitude;
    reproj_coord[1] = longitude;
    reproject(reproj_coord, 0, curr_utm, hemi, curr_hemi);

    gps_point.x = reproj_coord[0];
    gps_point.y = reproj_coord[1];
    gps_point.s = acc;

    log_this(100, "lat = %lf, lon = %lf, x = %lf, y = %lf", latitude, longitude, gps_point.x, gps_point.y);



    if (GPSEventType != ((Uint32)-1))
    {
        SDL_Event event;
        SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
        event.type = GPSEventType;
        event.user.code = 1;
        event.user.data1 = 0;
        event.user.data2 = 0;
        SDL_PushEvent(&event);
    }




}
