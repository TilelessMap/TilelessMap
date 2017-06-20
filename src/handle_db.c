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


/**

straight from  the sqlite docs
http://www.sqlite.org/backup.html
*/
#include "theclient.h"
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave)
{
    int rc;                   /* Function return code */
    sqlite3 *pFile;           /* Database connection opened on zFilename */
    sqlite3_backup *pBackup;  /* Backup object used to copy data */
    sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
    sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */

    /* Open the database file identified by zFilename. Exit early if this fails
    ** for any reason. */
    rc = sqlite3_open(zFilename, &pFile);
    if( rc==SQLITE_OK ) {


        pFrom = (isSave ? pInMemory : pFile);
        pTo   = (isSave ? pFile     : pInMemory);

        pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
        if( pBackup ) {
            (void)sqlite3_backup_step(pBackup, -1);
            (void)sqlite3_backup_finish(pBackup);
        }
        rc = sqlite3_errcode(pTo);
    }

    /* Close the database connection opened on database file zFilename
    ** and return the result of this function. */
    (void)sqlite3_close(pFile);
    return rc;
}


LAYER_RUNTIME* init_layer_runtime(int n)
{
    LAYER_RUNTIME *lr, *theLayer;
    int i;
    lr = malloc(n * sizeof(LAYER_RUNTIME));
    
    for (i = 0;i<n; i++)
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
        if (theLayer->geometryType == POLYGONTYPE)
        {
            element_destroy_buffer(theLayer->tri_index);
        }
        if(theLayer->show_text)
            text_destroy_buffer(theLayer->text);

        free(theLayer->name);
        sqlite3_finalize(theLayer->preparedStatement);


    }
    free(lr);

    return;
}


