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
        if(sqlite3_column_int(prepared_sql, 0)==1) //should be 2 rows, 1 for the layer and 1 for geometry_columns
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
    int rc;
    sqlite3_stmt *prepared_sql;
    snprintf(sql, 1024, "select * from %s.sqlite_master where type in ('table','view') and name = '%s' and sql like '%%`%s`%%'", dbname, layername, col_name);

    rc = sqlite3_prepare_v2(projectDB, sql, -1, &prepared_sql, 0);

    if (rc != SQLITE_OK ) {
        log_this(90, "failed run sql: %s, error_code: %d\n",sql,  rc);
        return 0;
    }


    if(sqlite3_step(prepared_sql) ==  SQLITE_ROW)
    {
        sqlite3_finalize(prepared_sql);
        return 1;
    }

    sqlite3_finalize(prepared_sql);
    return 0;


}



LAYER_RUNTIME* init_layer_runtime(int n)
{
    LAYER_RUNTIME *lr, *theLayer;
    int i;
    lr = malloc(n * sizeof(LAYER_RUNTIME));

    for (i = 0; i<n; i++)
    {
        theLayer = lr+i;
        theLayer->name = NULL;
        theLayer->visible = 0;
        theLayer->preparedStatement = NULL;
        /*Buffers*/
        theLayer->vbo = 0;
        theLayer->ebo = 0;
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
        theLayer->res_buf = NULL;
        theLayer->tri_index = NULL;
        theLayer->text = NULL;

        theLayer->show_text = 0;
        theLayer->line_width = 0;
        theLayer->layer_id = 0;
        theLayer->render_area = 0; //This is a way to render only border of polygon. No triangels will be loadded
        theLayer->utm_zone = 0;
        theLayer->hemisphere = 0; //1 is southern hemisphere and 0 is northern
        theLayer->close_ring = 0;
    }
    return lr;
}

void destroy_layer_runtime(LAYER_RUNTIME *lr, int n)
{
    int i;
    LAYER_RUNTIME *theLayer;
    for (i=0; i<n; i++)
    {
        theLayer = lr+i;

        destroy_buffer(theLayer->res_buf);
        if (theLayer->type & 4)
        {
            element_destroy_buffer(theLayer->tri_index);
        }
        if(theLayer->type & 32)
            text_destroy_buffer(theLayer->text);

        free(theLayer->name);
        sqlite3_finalize(theLayer->preparedStatement);


    }
    free(lr);

    return;
}
