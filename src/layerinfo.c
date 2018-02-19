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
#include "theclient.h"
#include "buffer_handling.h"
#include "ext/sqlite/sqlite3.h"
#include "mem.h"


/*************** Get info about layers *******************/
TLM_LAYER_LIST *TLM_get_layerlist()
{
        int i;
        
        TLM_LAYER_LIST *tll = st_malloc(sizeof(TLM_LAYER_LIST));
        tll->layers = st_malloc(tll->nlayers * sizeof(TLM_LAYER_INFO));
        
        for(i=0;i<global_layers->nlayers;i++)
        {
            TLM_LAYER_INFO *tli = tll->layers + i;
            LAYER_RUNTIME *l = global_layers->layers + i; 
            
            tli->name = st_malloc(strlen(l->name)+1);
            strcpy(tli->name, l->name);
            
            tli->db_alias = st_malloc(strlen(l->db)+1);
            strcpy(tli->db_alias, l->db);
            
            tli->id = l->layer_id);
            
            tli->visible = l->visible;
            
            tli->info_active = l->info_active;
            
            tli->type = l->type;           
        }
        tll->nlayers = global_layers->nlayers;
}



int    TLM_destroy_layerlist(TLM_LAYER_LIST *tll)
{
        int i;
        
        for (i=0;i<tll->nlayers;i++)
        {
            TLM_LAYER_INFO *tli = tll->layers + i;
            st_free(tli->name);   
            st_free(tli->db_alias);            
        }
        st_free(tll->layers);
        st_free(tll);
        return 0;
}

TLM_FIELDS *TLM_get_info_fields;
TLM_FIELDS *TLM_get_fields;
