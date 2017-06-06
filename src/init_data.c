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



/********************************************************************************
  Attach all databases with data for the project
*/
static int attach_db(char *dir, TEXT *missing_db)
{

    int rc;
    char *err_msg;
    sqlite3_stmt *preparedDb2Attach;
    char sql[2048];
    char *sqlDb2Attach = " select distinct d.source, d.name from dbs d inner join layers l on d.name=l.source;";

    rc = sqlite3_prepare_v2(projectDB, sqlDb2Attach, -1, &preparedDb2Attach, 0);
    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlDb2Attach );
        sqlite3_close(projectDB);
        return 1;
    }

    while(sqlite3_step(preparedDb2Attach)==SQLITE_ROW)
    {

        const unsigned char *dbsource = sqlite3_column_text(preparedDb2Attach, 0);
        const unsigned char * dbname= sqlite3_column_text(preparedDb2Attach, 1);
        char sqlAttachDb[128];
        snprintf(sqlAttachDb,sizeof(sql), "ATTACH '%s//%s' AS %s;", dir, dbsource, dbname);
        log_this(10, "attachsql = %s\n",sqlAttachDb );
        rc = sqlite3_exec(projectDB,sqlAttachDb,NULL, NULL, &err_msg);
        if (rc != 0)
        {
            log_this(90, "failed to load db: %s. rc = %d errcode = %s, sql = %s\n",dbsource, rc,err_msg, sqlAttachDb);

            if(missing_db->used)
                add_txt(missing_db, ",");

            add_txt(missing_db, "'");
            add_txt(missing_db,(const char*) dbname);
            add_txt(missing_db, "'");

            sqlite3_free(err_msg);
        }
        /* vs_source = sqlite3_column_text(preparedLayerLoading, 3);
         fs_source = sqlite3_column_text(preparedLayerLoading, 4);*/
    }
    sqlite3_finalize(preparedDb2Attach);
    return 0;
}



/********************************************************************************
    Count how many layers we are dealing with
*/

static int count_layers()
{
    int n, rc;
    sqlite3_stmt *preparedCountLayers;
    char *sqlCountLayers = "SELECT COUNT(*) FROM layers l ; ";

    rc = sqlite3_prepare_v2(projectDB, sqlCountLayers, -1, &preparedCountLayers, 0);

    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlCountLayers );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_step(preparedCountLayers);
    n = sqlite3_column_int(preparedCountLayers, 0);
    sqlite3_finalize(preparedCountLayers);
    return n;
}


/******************************************************
 * Load Styles
 * ***************************************************/
static int load_styles()
{

    int rc, i;
    sqlite3_stmt *preparedCountStyle;
    sqlite3_stmt * preparedStylesLoading;
    GLfloat z;

    /********************************************************************************
     Count the layers in the project and get maximum styleID in the project
     */
    char *sqlCountStyles = "SELECT COUNT(*), MAX(styleID)   "
                           "FROM styles s ; ";

    rc = sqlite3_prepare_v2(projectDB, sqlCountStyles, -1, &preparedCountStyle, 0);

    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlCountStyles );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_step(preparedCountStyle);
    int nStyles = sqlite3_column_int(preparedCountStyle, 0);
    int maxStyleID = sqlite3_column_int(preparedCountStyle, 1);
    sqlite3_finalize(preparedCountStyle);



    /********************************************************************************
      Put all the styles in an array of style structures
    */
    length_global_styles = maxStyleID + 1;

    size_t thesize = length_global_styles * sizeof(STYLES_RUNTIME) + 1;
    global_styles = malloc(thesize); // + 1 is for making place for 1-based array
    memset(global_styles,0,thesize);

    char *sqlStyles = "SELECT "
                      /*fields for attaching the database*/
                      "styleID, color_r, color_g, color_b, color_a, out_r, out_g, out_b, line_w, line_w2, z, unit "
                      "from styles;";

    rc = sqlite3_prepare_v2(projectDB, sqlStyles, -1, &preparedStylesLoading, 0);

    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlStyles );
        sqlite3_close(projectDB);
        return 1;
    }

    for (i =0; i<nStyles; i++)
    {
        sqlite3_step(preparedStylesLoading);
        int styleID = sqlite3_column_int(preparedStylesLoading, 0);
        global_styles[styleID].styleID = styleID;
        global_styles[styleID].color[0] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 1)/255.0);
        global_styles[styleID].color[1] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 2)/255.0);
        global_styles[styleID].color[2] =  (GLfloat) (sqlite3_column_int(preparedStylesLoading, 3)/255.0);
        global_styles[styleID].color[3] =  (GLfloat) (sqlite3_column_int(preparedStylesLoading, 4)/255.0);
        global_styles[styleID].outlinecolor[0] =  (GLfloat) (sqlite3_column_int(preparedStylesLoading, 5)/255.0);
        global_styles[styleID].outlinecolor[1] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 6)/255.0);
        global_styles[styleID].outlinecolor[2] = (GLfloat) (sqlite3_column_int(preparedStylesLoading, 7)/255.0);
        global_styles[styleID].outlinecolor[3] = 1.0;
        global_styles[styleID].lineWidth =  (GLfloat) sqlite3_column_double(preparedStylesLoading, 8);
        global_styles[styleID].lineWidth2 =  (GLfloat) sqlite3_column_double(preparedStylesLoading, 9);
        z = (GLfloat) sqlite3_column_double(preparedStylesLoading, 10);
        if(z)
        {
            if(z>100)
                z = 100;
            if(z < 0)
                z = 0;
            global_styles[styleID].z = 1-z*0.01;
        }
        else
            global_styles[styleID].z = 0;
        global_styles[styleID].unit =  (GLfloat) sqlite3_column_int(preparedStylesLoading, 11);
    }

    sqlite3_finalize(preparedStylesLoading);
    return 0;
}

static int load_layers(TEXT *missing_db)
{


    /********************************************************************************
     Get information about all the layers in the project
     */
    int rc, i;

    sqlite3_stmt *preparedLayerLoading;
    sqlite3_stmt *preparedLayer;
    sqlite3_stmt *prepared_geo_col;

    LAYER_RUNTIME *oneLayer;

    char *styleselect;
    char stylejoin[128];
    char stylewhere[128];
    char textselect[128];
    uint8_t override_type;
    char sql[2048];


    char sqlLayerLoading[2048];
    char *sqlLayerLoading1 = "SELECT "
                             /*fields for attaching the database*/
                             "d.name dbname, "   // 0
                             /*fields for creating the prepared statement to get data later*/
                             "l.name layername,"  // 1
                             "l.override_type,"  // 2
                             /*fields to inform comming processes about how and when to render*/
                             " l.defaultVisible,"  // 3
                             "l.minScale,"  // 4
                             "l.maxScale,"  // 5
                             "styleField,"  // 6
                             "showText,"  // 7
                             "linewidth,"  // 8
                             "l.layerID, "  // 9
                             "tc.size_fld,"  // 10
                             "rotation_fld,"  // 11
                             "anchor_fld,"  // 12
                             "txt_fld"  // 13

                             " FROM layers l "
                             "INNER JOIN dbs d on l.source = d.name "
                             "LEFT JOIN text_conf tc on l.layerID=tc.layerID ";

    if(missing_db->used)
        snprintf(sqlLayerLoading, 2048, "%s where d.name not in (%s) order by l.orderby ;",sqlLayerLoading1, get_txt(missing_db));
    else
        snprintf(sqlLayerLoading, 2048, "%s order by l.orderby ;",sqlLayerLoading1);


    log_this(50, "Get Layer sql : %s\n",sqlLayerLoading);
    rc = sqlite3_prepare_v2(projectDB, sqlLayerLoading, -1, &preparedLayerLoading, 0);

    if (rc != SQLITE_OK ) {
        log_this(110, "SQL error in %s\n",sqlLayerLoading);
        sqlite3_close(projectDB);
        return 1;
    }


    /********************************************************************************
     Time to iterate all layers in the project and add data about them in struct layerRuntime
    */

    layerRuntime = init_layer_runtime(count_layers());
    //  for (i =0; i<nLayers; i++)
    i=0;
    while(sqlite3_step(preparedLayerLoading) ==  SQLITE_ROW)
    {
        oneLayer=layerRuntime + i;
        //   sqlite3_step(preparedLayerLoading);

        override_type = 0;
        oneLayer->close_ring = 0;
        const unsigned char * dbname = sqlite3_column_text(preparedLayerLoading, 0);
        const unsigned char *layername = sqlite3_column_text(preparedLayerLoading,1);
        override_type = sqlite3_column_int(preparedLayerLoading,2);
        oneLayer->visible = sqlite3_column_int(preparedLayerLoading, 3);
        oneLayer->minScale = sqlite3_column_double(preparedLayerLoading, 4);
        oneLayer->maxScale = sqlite3_column_double(preparedLayerLoading, 5);
        const unsigned char *stylefield =  sqlite3_column_text(preparedLayerLoading, 6);
        oneLayer->show_text =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 7);
        oneLayer->line_width =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 8);
        int layerid =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 9);



        const unsigned char *size_fld = sqlite3_column_text(preparedLayerLoading,10);
        const unsigned char *rotation_fld = sqlite3_column_text(preparedLayerLoading, 11);
        const unsigned char *anchor_fld =  sqlite3_column_text(preparedLayerLoading, 12);
        const unsigned char *txt_fld =  sqlite3_column_text(preparedLayerLoading, 13);

        
        if (check_layer(dbname, layername))
        {
            i++;
        }
        else
        {
            log_this(90, "Cannot use layer %s",layername);
            continue;
        }


        //Get the basic layer info from geometry columns table in data db
        snprintf(sql, 2048, "SELECT geometry_type, geometry_fld, id_fld, spatial_idx_fld, tri_idx_fld, utm_zone, hemisphere from %s.geometry_columns where layer_name='%s';", dbname, layername);

        log_this(100, "Get info from geometry_columns : %s\n",sql);
        rc = sqlite3_prepare_v2(projectDB, sql, -1, &prepared_geo_col, 0);

        if (rc != SQLITE_OK ) {
            log_this(110, "SQL error in %s\n",sql);
            sqlite3_close(projectDB);
            return 1;
        }
        if(!(sqlite3_step(prepared_geo_col) ==  SQLITE_ROW))
        {
            log_this(100, "Cannot use layer %s",layername);
            continue;
        }
        oneLayer->geometryType =  (uint8_t) sqlite3_column_int(prepared_geo_col, 0);
        if(override_type)
        {
            if(oneLayer->geometryType == 3)
                oneLayer->close_ring = 1;
            
            oneLayer->geometryType = override_type;
        }
        const unsigned char *geometryfield = sqlite3_column_text(prepared_geo_col, 1);
        const unsigned char *idfield = sqlite3_column_text(prepared_geo_col, 2);
        const unsigned char *geometryindex = sqlite3_column_text(prepared_geo_col, 3);
        const unsigned char *tri_index_field = sqlite3_column_text(prepared_geo_col, 4);
        oneLayer->utm_zone =  (uint8_t) sqlite3_column_int(prepared_geo_col, 5);
        oneLayer->hemisphere =  (uint8_t) sqlite3_column_int(prepared_geo_col, 6);



        //TODO free this
        oneLayer->name = malloc(strlen((char*) layername)+1);

        strcpy(oneLayer->name,(char*) layername);


        //printf("name = %s\n", oneLayer->name);
        oneLayer->layer_id =  (uint8_t) layerid;

        oneLayer->render_area =! NULL;


        char tri_idx_fld[32];
        if(oneLayer->geometryType == POLYGONTYPE && oneLayer->render_area)
        {
            snprintf(tri_idx_fld, sizeof(tri_idx_fld), "%s", tri_index_field);
        }
        else
        {
            snprintf(tri_idx_fld, sizeof(tri_idx_fld), "%s","'a'" );
        }

        if(stylefield)
        {
            styleselect = ", styleID ";
            if(strcmp((const char *)stylefield, "__everything__"))
            {
                snprintf(stylejoin,sizeof(stylejoin), "%s%s%s", " inner join styles s on e.", stylefield, "=s.value " );
            }
            else
            {
                snprintf(stylejoin,sizeof(stylejoin), "%s", " , styles s " );
            }
            snprintf(stylewhere,sizeof(stylewhere), "%s%d", " and s.layerID=", layerid);
        }
        else
        {
            styleselect = "\0";
            stylejoin[0] = '\0';
            stylewhere[0] =  '\0';
        }

        if(oneLayer->show_text)
        {
            snprintf(textselect, sizeof(textselect), ",e.%s, e.%s,e.%s,e.%s",txt_fld, size_fld, rotation_fld, anchor_fld);
        }
        else
        {
            textselect[0] = '\0';
        }

        snprintf(sql,sizeof(sql),"select e.%s, %s,e.%s %s %s from %s.%s e inner join %s.%s ei on e.%s = ei.id %s where  ei.minX<? and ei.maxX>? and ei.minY<? and ei.maxY >? %s",
                 geometryfield,
                 tri_idx_fld,
                 idfield,
                 styleselect,
                 textselect,
                 dbname,
                 layername,
                 dbname,
                 geometryindex,
                 idfield,
                 stylejoin,
                 stylewhere );

        /*

        snprintf(sql,sizeof(sql), "%s%s %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                 "select ",
                 geometryfield,
                 ", ",tri_idx_fld,
                 ", e.", idfield, styleselect, textselect,  " from ",
                 dbname, ".",layername,
                 " e inner join ",
                 dbname, ".",geometryindex,
                 "  ei on e.",
                 idfield,
                 "=ei.id ",
                 stylejoin,
                 "where ",
                 " ei.minX<? and ei.maxX>? and ei.minY<? and ei.maxY >? ",
                 stylewhere );

                 */
        rc = sqlite3_prepare_v2(projectDB, sql, -1,&preparedLayer, 0);
log_this(100, "sql %s\n",sql );
        if (rc != SQLITE_OK ) {
            log_this(100, "SQL error in %s\n",sql );
            sqlite3_close(projectDB);
            return 1;
        }
        oneLayer->preparedStatement = preparedLayer;
        oneLayer->res_buf =  init_res_buf();

        if (oneLayer->geometryType == POLYGONTYPE)
            oneLayer->tri_index =  init_element_buf();

        if (oneLayer->show_text)
            oneLayer->text =  init_text_buf();

        sqlite3_finalize(prepared_geo_col);
    }
    nLayers = i;
    sqlite3_finalize(preparedLayerLoading);


    /*get init_bbox*/
    if (check_layer((const unsigned char *) "main", (const unsigned char *)  "init_box"))
    {

        int rc;
        sqlite3_stmt *preparedinitBox;

        char *sql = "select x,y,box_width, utm_zone, hemisphere from init_box;";

        rc = sqlite3_prepare_v2(projectDB, sql, -1, &preparedinitBox, 0);


        if (rc != SQLITE_OK ) {
            log_this(100, "SQL error in %s\n",sql );
            sqlite3_close(projectDB);
            return 1;
        }
        sqlite3_step(preparedinitBox);
        init_x = sqlite3_column_double(preparedinitBox, 0);
        init_y = sqlite3_column_double(preparedinitBox, 1);
        init_box_width = sqlite3_column_double(preparedinitBox, 2);
        curr_utm = sqlite3_column_double(preparedinitBox, 3);
        curr_hemi = sqlite3_column_double(preparedinitBox, 4);
        sqlite3_finalize(preparedinitBox);
    }
    else
    {
        init_x = 325000;
        init_y = 6800000;
        init_box_width = 800000;

    }


    return 0;
}
int init_gps()
{
    gps_npoints = 32;
    GLfloat *gps_circle = create_circle(gps_npoints);
    loadGPS(gps_circle);
    return 0;
}



int init_resources(char *dir)
{
    log_this(10, "Entering init_resources\n");

    TEXT *missing_db = init_txt(1024);

    //char stylewhere[128];


    build_program();

    attach_db(dir, missing_db);

    load_styles();

    load_layers(missing_db);
    destroy_txt(missing_db);
    init_gps();
    return 0;
}



