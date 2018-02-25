/**********************************************************************
 *
 * TilelessMap
 *
 * TilelessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TilelessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TilelessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/


#include "tilelessmap.h"
#include <string.h>

#ifdef __ANDROID__ 
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{

    char *projectfile=NULL, *dir=NULL;


    while(--argc>0)
    {
        argv++;
        if(!strcmp(*argv,"-f") || !strcmp(*argv,"--projfile"))
        {
            argc--;
            if(argc > 0)
                projectfile=*++argv;
     /*       else
            {
                log_this(110, "Too few arguments \n");
                return 1;
            }*/
            continue;
        }

        if(!strcmp(*argv,"-d") || !strcmp(*argv,"--directory"))
        {
            argc--;
            if(argc > 0)
                dir=*++argv;
            /*
            else
            {
                log_this(110, "Too few arguments \n");
                return 1;
            }
*/
            continue;
        }
    }
CTRL* controls = NULL;
     
        TLM_init_SDL();
    if (projectfile && strlen(projectfile) >0)
    {
        TLM_init_db(projectfile, dir);
        controls = TLM_init_controls(NATIVE_default);
     }
    TLM_start(controls);
     
     
    TLM_close();

    
    
    return EXIT_SUCCESS;
}
