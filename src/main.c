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


#include "tilelessmap.h"
#include <string.h>

static int button_test(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
    log_this(100, "Yes, it works, getting text %s\n", (char*) val);
 
    if(incharge == ctrl)   
    {
    init_matrix_handler(ctrl, 0, 0,0);    
    incharge = NULL;
    }
    else
    {
    init_matrix_handler(ctrl, 1, 1,1);    
    incharge = ctrl;
    }
    
    return 0;
    
}
static int table_test(void *ctrl, void *val, tileless_event_func_in_func func_in_func)
{
    log_this(100, "Yes, it works, getting text %s\n", (char*) val);
 
    if(incharge == ctrl)   
    {
    init_matrix_handler(ctrl, 0, 0,0);    
    incharge = NULL;
    }
    else
    {
    init_matrix_handler(ctrl, 1, 1,1);    
    incharge = ctrl;
    }
    
    return 0;
    
}
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
