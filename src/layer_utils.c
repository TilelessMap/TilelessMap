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
#include "buffer_handling.h"
#include "mem.h"
#include "cleanup.h"

int check_layer(const unsigned char *dbname, const unsigned char  *layername)
{

    char sql[1024];
    int rc;
    sqlite3_stmt *prepared_sql;
    snprintf(sql, 1024, "select count(*) from %s.sqlite_master where type in ('table','view') and name = '%s'", dbname, layername);

    rc = sqlite3_prepare_v2(projectDB, sql, -1, &prepared_sql, 0);

    if (rc != SQLITE_OK ) {
        log_this(110, "SQL error in %s\n",sql);
        return 0;
    }


    if(sqlite3_step(prepared_sql) ==  SQLITE_ROW)
    {
        //We don't check if layer actually is represented. That will be found without db-error when loading layer
        if(sqlite3_column_int(prepared_sql, 0)==1)
        {
            sqlite3_finalize(prepared_sql);
            return 1;
        }
    }

    log_this(110, "we cannot use %s from %s database\n",layername, dbname);
    sqlite3_finalize(prepared_sql);
    return 0;


}




int check_column(const unsigned char *dbname,const unsigned char * layername, const unsigned char  *col_name)
{
    char sql[1024];
    int rc, res;
    sqlite3_stmt *prepared_sql;
    snprintf(sql, 1024, "select sql from %s.sqlite_master where type in ('table','view') and name = '%s'", dbname, layername);
//   printf("sql = %s\n", sql);
    rc = sqlite3_prepare_v2(projectDB, sql, -1, &prepared_sql, 0);

    if (rc != SQLITE_OK ) {
        log_this(90, "failed run sql: %s, error_code: %d\n",sql,  rc);
        return 0;
    }


    if(sqlite3_step(prepared_sql) ==  SQLITE_ROW)
    {
        const char *w = (const char*) sqlite3_column_text(prepared_sql, 0);

        res = search_string(w,(const char*) col_name);
        sqlite3_finalize(prepared_sql);
        return res;
    }

    sqlite3_finalize(prepared_sql);
    return 0;


}


LAYERS* init_layers(int n)
{
    log_this(10, "entering init_layers\n");
    LAYERS *l = st_malloc(sizeof(LAYERS));
    l->layers = init_layer_runtime(n);
    l->max_nlayers = n;
    l->nlayers = n;
    return l;
    
}


LAYER_RUNTIME* init_layer_runtime(int n)
{
    log_this(10, "entering init_layer_runtime\n");
    LAYER_RUNTIME *lr, *theLayer;
    int i;
    lr = st_malloc(n * sizeof(LAYER_RUNTIME));
    
    for (i = 0; i<n; i++)
    {
        theLayer = lr+i;
        theLayer->name = NULL;
        theLayer->db = NULL;
        theLayer->title = NULL;
        theLayer->visible = 0;
        theLayer->info_active = 0;
        theLayer->preparedStatement = st_malloc(sizeof(PS_HOLDER));
        theLayer->preparedStatement->ps=NULL;
        theLayer->preparedStatement->usage=0;
        /*Buffers*/
        /*Values for shaders*/
        //theLayer->theMatrix[16];
        /*values for what and how to render*/
        theLayer->BBOX = NULL;
        theLayer->geometryType = 0;
        theLayer->type = 0; //8 on/off switches: point simple, point symbol, point text, line simple, line width, poly
        theLayer->n_dims = 0;;
        //   uint8_t has_text;
        theLayer->minScale = 0;
        theLayer->maxScale = 0;
        theLayer->points = NULL;
        theLayer->lines = NULL;
        theLayer->wide_lines = NULL;
        theLayer->polygons = NULL;
        //  theLayer->res_buf = NULL;
        theLayer->text = NULL;

//        theLayer->show_text = 0;
//        theLayer->line_width = 0;
        theLayer->layer_id = 0;
//        theLayer->render_area = 0; //This is a way to render only border of polygon. No triangels will be loadded
        theLayer->utm_zone = 0;
        theLayer->hemisphere = 0; //1 is southern hemisphere and 0 is northern
//        theLayer->close_ring = 0;
        theLayer->styles = NULL;
        theLayer->style_key_type = INT_TYPE;
    }
    return lr;
}


static int destroy_polygon_style(POLYGON_STYLE *s)
{
    destroy_glfloat_list(s->color);
    destroy_glfloat_list(s->z);
    destroy_glushort_list(s->units);
    st_free(s);
    return 0;
}

static int destroy_line_style(LINE_STYLE *s)
{
    destroy_glfloat_list(s->color);
    destroy_glfloat_list(s->z);
    destroy_glushort_list(s->units);
    destroy_glfloat_list(s->width);
    st_free(s);
    return 0;
}

static int destroy_point_style(POINT_STYLE *s)
{
    destroy_glfloat_list(s->color);
    destroy_glfloat_list(s->z);
    destroy_glfloat_list(s->size);
    destroy_glushort_list(s->units);
    destroy_uint8_list(s->symbol);
    st_free(s);
    return 0;
}

static int destroy_text_style(TEXT_STYLE *s)
{
    destroy_glfloat_list(s->color);
    destroy_glfloat_list(s->size);
    destroy_glfloat_list(s->z);
    destroy_pointer_list(s->a);
    st_free(s);
    return 0;
}

static int destroy_style(struct STYLES *s)
{
    if(s->key_type == STRING_TYPE && s->string_key)
        st_free(s->string_key);

    if(s->polygon_styles)
        destroy_polygon_style(s->polygon_styles);
    if(s->line_styles)
        destroy_line_style(s->line_styles);
    if(s->point_styles)
        destroy_point_style(s->point_styles);
    if(s->text_styles)
        destroy_text_style(s->text_styles);
    st_free(s);
    return 0;

}









static void delete_styles(LAYER_RUNTIME *l) {
    struct STYLES *current_style, *tmp;


    HASH_ITER(hh, l->styles, current_style, tmp)
    {
        HASH_DEL(l->styles,current_style);  /* delete; users advances to next */
        destroy_style(current_style);
        current_style=NULL;

    }



}

void destroy_layers(LAYERS *l)
{
 
    destroy_layer_runtime(l->layers, l->nlayers);
    l->max_nlayers = 0;
    l->layers = 0;
    free(l);
    l=NULL;
    return;
}


void destroy_layer_runtime(LAYER_RUNTIME *lr, int n)
{
    int i;
    LAYER_RUNTIME *theLayer;
    for (i=0; i<n; i++)
    {
        theLayer = lr+i;

        /*       destroy_buffer(theLayer->res_buf);
               if (theLayer->type & 4)
               {
                   element_destroy_buffer(theLayer->tri_index);
               }*/
        destroy_buffers(theLayer);
        if(theLayer->type & 32)
            text_destroy_buffer(theLayer->text);

        delete_styles(theLayer);
        st_free(theLayer->name);
        st_free(theLayer->db);
        st_free(theLayer->title);
        
        theLayer->preparedStatement->usage--;
        if(theLayer->preparedStatement->usage<=0)
        {
            sqlite3_finalize(theLayer->preparedStatement->ps);        
            theLayer->preparedStatement->ps = NULL;        
            st_free(theLayer->preparedStatement);
        }

    }
    free(lr);
    lr = NULL;
    return;
}
