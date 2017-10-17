
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
            else
            {
                log_this(110, "Too few arguments \n");
                return 1;
            }
            continue;
        }

        if(!strcmp(*argv,"-d") || !strcmp(*argv,"--directory"))
        {
            argc--;
            if(argc > 0)
                dir=*++argv;
            else
            {
                log_this(110, "Too few arguments \n");
                return 1;
            }

            continue;
        }
    }

    if (!(projectfile))
    {
        log_this(110, "Too few arguments \n");
        return 1;
    }


    
    TLM_init(projectfile, dir);
    
    CTRL* controls = TLM_init_controls(NATIVE_default);
    /*
    short box[4] = {500,500,1200,700};
    float color[] = {0,0,0,150};
    short margins[2] = {20,20};
    
   // CTRL *button = add_button(controls, controls,box ,"Test jg ", button_test,"rundgång", color,80,margins,1);
    
    //init_matrix_handler(button, 1, 1,1);  
short box2[4] = {500, 500, 1000, 1300};
short margins2[2] = {2,2};
    
    CTRL *table = add_table(controls, controls, color, box2, margins2, table_test);
    CTRL *row, *cell;
    
    float color2[4] = {0,0,0,200};
    short col_widths[2] = {200, 240};
    row = add_row(table,color2, 2, col_widths );
    
    color[0]=color[1]=color[2]=color[3]=255;
    cell = add_cell(row, "test", color, color2, margins2, NULL,NULL, 20);
    cell = add_cell(row, "test mer", color, color2, margins2, NULL,NULL, 20);
    
    row = add_row(table,color2, 2, col_widths );
    
    cell = add_cell(row, "test igen", color, color2, margins2, NULL,NULL, 20);
    cell = add_cell(row, "test mer", color, color2, margins2, NULL,NULL, 20);
      
    
    init_matrix_handler(table, 1, 1,1);  */
    TLM_start(controls);
    
    TLM_close();

    
    
    return EXIT_SUCCESS;
}
