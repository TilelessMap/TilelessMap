
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
