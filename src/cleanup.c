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


#include "theclient.h"
#include "interface/interface.h"
#include "info.h"
#include "matrix_handling.h"
#include "log.h"
#include "cleanup.h"

void free_resources(SDL_Window* window,SDL_GLContext context)
{
    log_this(10, "Entering free_resources\n");

    if(projectDB)
    {
        glDeleteProgram(std_program);
        glDeleteProgram(txt_program);
        glDeleteProgram(txt2_program);
        glDeleteProgram(lw_program);

        glDeleteProgram(gps_program);
        glDeleteProgram(sym_program);
        glDeleteProgram(raster_program);

        destroy_control(get_master_control());
    //   destroy_layer_runtime(layerRuntime,nLayers);
        destroy_layers(global_layers);
        destroy_layer_runtime(infoLayer,1);
        free(gps_circle);
        destroy_symbol_list(global_symbols);
        destroy_font(fnts);

        destroy_wc_txt(tmp_unicode_txt);

        destroy_txt_coords(txt_coords);


        FT_Done_FreeType(ft);
        sqlite3_close_v2(projectDB);
        sqlite3_shutdown();
    }
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow( window );
    window = NULL;
    SDL_Quit();
}
