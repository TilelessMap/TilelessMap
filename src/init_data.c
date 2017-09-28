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


#include "buffer_handling.h"
#include "interface/interface.h"
#include "read_sld.h"
#include "symbols.h"
#include "mem.h"
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
		if(dir)
	        snprintf(sqlAttachDb,sizeof(sql), "ATTACH '%s//%s' AS %s;", dir, dbsource, dbname);
		else
			snprintf(sqlAttachDb, sizeof(sql), "ATTACH '%s' AS %s;", dbsource, dbname);
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
 * Load Symbols
 * ***************************************************/
#ifdef symininit
static int load_symbols()
{

    if (!(check_layer((const unsigned char *) "main", (const unsigned char *) "symbols")))
        return 0;
    int rc, i;
    sqlite3_stmt *preparedCountSymbol;
    sqlite3_stmt * preparedSymbolsLoading;
    GLfloat z;


    /********************************************************************************
      Put all the symbools in an array
    */


    char *sqlSymbols = "SELECT "
                       "symbol_id, n_dirs, length_second, rotation  "
                       "from symbols order by symbol_id;";

    rc = sqlite3_prepare_v2(projectDB, sqlSymbols, -1, &preparedSymbolsLoading, 0);

    if (rc != SQLITE_OK ) {
        log_this(1, "SQL error in %s\n",sqlSymbols );
        sqlite3_close(projectDB);
        return 1;
    }
    init_symbols();
    while(sqlite3_step(preparedSymbolsLoading)==SQLITE_ROW)
    {

        int symbolid = sqlite3_column_int(preparedSymbolsLoading, 0);

        int n_dirs = sqlite3_column_int(preparedSymbolsLoading, 1);
        float length_second = sqlite3_column_double(preparedSymbolsLoading, 2);
        float rotation= sqlite3_column_double(preparedSymbolsLoading, 3);

        GLfloat* points = create_symbol(n_dirs,1, length_second, rotation);
        addsymbol(symbolid, (n_dirs + 2) * 2, points);
    }

    sqlite3_finalize(preparedSymbolsLoading);



    return 0;
}
#endif
static int load_layers(TEXT *missing_db)
{


    /********************************************************************************
     Get information about all the layers in the project
     */
    int rc, i, show_text, line_width;

    sqlite3_stmt *preparedLayerLoading;
    sqlite3_stmt *preparedLayer;
    sqlite3_stmt *prepared_geo_col;

    LAYER_RUNTIME *oneLayer;

    char styleselect[128];
    char textselect[256];
    char sql[2048];
    uint8_t type = 0;
    char sqlSel2[15];
    /*  if(!(check_column((const unsigned char *) "main",(const unsigned char *) "layers",(const unsigned char *) "override_type")))
      {

          rc = sqlite3_exec(projectDB,"alter table layers add column override_type integer;",NULL, NULL, &err_msg);
          if (rc != 0)
          {
              log_this(90, "failed to add missing column in layers table: %s\n", err_msg);

              sqlite3_free(err_msg);
          }




      }*/
    char sqlLayerLoading[2048];
    char *sqlLayerLoading1 = "SELECT "
                             /*fields for attaching the database*/
                             "d.name dbname, "   // 0
                             /*fields for creating the prepared statement to get data later*/
                             "l.name layername,"  // 1
                             /*fields to inform comming processes about how and when to render*/
                             " l.defaultVisible,"  // 2
                             "l.minScale,"  // 3
                             "l.maxScale,"  // 4
                             "type,"  // 5
                             "showText,"  // 6
                             "linewidth,"  // 7
                             "l.layerID, "  // 8
                             "title,"  // 9
                             "sld";  // 10

    char *sqlLayerLoading2 = " FROM layers l "
                             "INNER JOIN dbs d on l.source = d.name ";


    int info_rel = check_column((const unsigned char *) "main",(const unsigned char *) "layers",(const unsigned char *) "info_rel");
    if(info_rel)
        strcpy(sqlSel2,", info_rel");
    else
        sqlSel2[0] = '\0';

    if(missing_db->used)
        snprintf(sqlLayerLoading, 2048, "%s %s %s where d.name not in (%s) order by l.orderby ;",sqlLayerLoading1, sqlSel2, sqlLayerLoading2, get_txt(missing_db));
    else
        snprintf(sqlLayerLoading, 2048, "%s %s %s order by l.orderby ;",sqlLayerLoading1, sqlSel2, sqlLayerLoading2);


    log_this(100, "Get Layer sql : %s\n",sqlLayerLoading);
    
    rc = sqlite3_prepare_v2(projectDB, sqlLayerLoading, -1, &preparedLayerLoading, 0);
    log_this(100, "prepared, ready");
    if (rc != SQLITE_OK ) {
        log_this(110, "SQL error in %s\n",sqlLayerLoading);
        sqlite3_close(projectDB);
        return 1;
    }


    /********************************************************************************
     Time to iterate all layers in the project and add data about them in struct layerRuntime
    */

    log_this(100, "ok, init layers");
    global_layers = init_layers(count_layers());
    
    log_this(100, "ok, layers initialized\n");
    log_this(100, "ok, layers initialized %d layers\n",global_layers->nlayers);
    
    i=0;
    while(1)
    {
        log_this(100, "1\n");
        int res =  sqlite3_step(preparedLayerLoading) ;
        log_this(100, "2 res = %d\n",res);

        if(res !=  SQLITE_ROW)
        {
            break;
        }

        oneLayer=global_layers->layers + i;
        
        log_this(100, "3\n");
        //   sqlite3_step(preparedLayerLoading);
        log_this(100,"get layer ----------------------------------------------------\n");
//       oneLayer->close_ring = 0;
        const unsigned char * dbname = sqlite3_column_text(preparedLayerLoading, 0);
        const unsigned char *layername = sqlite3_column_text(preparedLayerLoading,1);
        oneLayer->visible = sqlite3_column_int(preparedLayerLoading, 2);
        oneLayer->minScale = sqlite3_column_double(preparedLayerLoading, 3);
        oneLayer->maxScale = sqlite3_column_double(preparedLayerLoading, 4);
        const unsigned char *stylefield =  sqlite3_column_text(preparedLayerLoading, 5);
        show_text =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 6);
        line_width =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 7);
        int layerid =  (uint8_t) sqlite3_column_int(preparedLayerLoading, 8);


        log_this(100, "3\n");
        /*
                const unsigned char *size_fld = sqlite3_column_text(preparedLayerLoading,9);
                const unsigned char *rotation_fld = sqlite3_column_text(preparedLayerLoading, 10);
                const unsigned char *anchor_fld =  sqlite3_column_text(preparedLayerLoading, 11);
                const unsigned char *txt_fld =  sqlite3_column_text(preparedLayerLoading, 12);*/
        const unsigned char *title =  sqlite3_column_text(preparedLayerLoading, 9);
        const unsigned char *sld =  sqlite3_column_text(preparedLayerLoading, 10);


        oneLayer->name = malloc(2 * strlen((char*) layername)+1);
        strcpy(oneLayer->name,(char*) layername);
        oneLayer->db = st_malloc(2 * strlen((char*) dbname)+1);
        strcpy(oneLayer->db,(char*) dbname);

        log_this(100, "prepare layer %s\n",oneLayer->name);
        
        oneLayer->title = malloc(2 * strlen((char*) title)+1);
        strcpy(oneLayer->title,(char*) title);
        if (check_layer(dbname, layername))
        {
            if(!stylefield)
                continue;

            if(!strcmp((const char*) stylefield,  "raster"))
            {
                snprintf(sql, 2048, "SELECT geometry_fld,data_fld, id_fld,spatial_idx,  utm_zone, hemisphere, tilewidth, tileheight from %s.raster_columns where layer_name='%s';", dbname, layername);

                log_this(100, "Get info from raster_columns : %s\n",sql);
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
                i++;
                oneLayer->geometryType = RASTER;
                oneLayer->type = 16;

                init_buffers(oneLayer);
                const unsigned char *geometry_fld = sqlite3_column_text(prepared_geo_col, 0);
                const unsigned char *data_fld = sqlite3_column_text(prepared_geo_col, 1);
                const unsigned char *idfield = sqlite3_column_text(prepared_geo_col, 2);
                const unsigned char *geometryindex = sqlite3_column_text(prepared_geo_col, 3);
                oneLayer->utm_zone =  (uint8_t) sqlite3_column_int(prepared_geo_col, 4);
                oneLayer->hemisphere =  (uint8_t) sqlite3_column_int(prepared_geo_col, 5);
                oneLayer->rast->tilewidth =   sqlite3_column_int(prepared_geo_col, 6);
                oneLayer->rast->tileheight =   sqlite3_column_int(prepared_geo_col, 7);


                snprintf(sql, 2048, "select %s, %s,%s, %s,0, x,y from %s.%s o inner join %s.%s i on o.%s = i.id where  i.minX<? and i.maxX>? and i.minY<? and i.maxY >? order by x, y;",
                         geometry_fld,data_fld,idfield, idfield, dbname, layername, dbname, geometryindex, idfield);

                rc = sqlite3_prepare_v2(projectDB, sql, -1,&preparedLayer, 0);
                log_this(100, "sql %s\n",sql );
                if (rc != SQLITE_OK ) {
                    log_this(100, "SQL error in %s\n",sql );
                    sqlite3_close(projectDB);
                    return 1;
                }
                oneLayer->n_dims = 2;
                oneLayer->preparedStatement = preparedLayer;



            }
            else
            {
                char *text_field = NULL;

                char *sld_style_field = NULL;
                if(sld && strlen((const char*)sld) >0)
                {
                    size_t sld_len = strlen((const char*) sld);
                    char *sld_copy = st_malloc(sld_len+1);
                    strcpy(sld_copy, (const char*) sld);


                    sld_style_field = load_sld(oneLayer,sld_copy, &text_field);
                    free(sld_copy);
                    //Get the basic layer info from geometry columns table in data db
                }
                if(check_column(dbname,(const unsigned char*) "geometry_columns",(const unsigned char*) "idx_id_fld"))
                    snprintf(sql, 2048, "SELECT geometry_type, geometry_fld, idx_id_fld,id_fld, spatial_idx_fld, tri_idx_fld, utm_zone, hemisphere, n_dims from %s.geometry_columns where layer_name='%s';", dbname, layername);
                else
                    snprintf(sql, 2048, "SELECT geometry_type, geometry_fld, id_fld,id_fld, spatial_idx_fld, tri_idx_fld, utm_zone, hemisphere, n_dims from %s.geometry_columns where layer_name='%s';", dbname, layername);



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

                i++;

                oneLayer->geometryType =  (uint8_t) sqlite3_column_int(prepared_geo_col, 0);
                type = 0;

                if(oneLayer->geometryType == POINTTYPE)
                {
                    type = type | 128;
                    if (show_text)
                        type = type | 32;
                }
                else if(oneLayer->geometryType == LINETYPE)
                {
                    if(line_width)
                        type = type | 8;
                    else
                        type = type | 16;
                }
                else if(oneLayer->geometryType == POLYGONTYPE)
                {
                    type = type | 4;
                    if(line_width)
                        type = type | 8;

                }
                oneLayer->type = type;

                init_buffers(oneLayer);

                const unsigned char *geometryfield = sqlite3_column_text(prepared_geo_col, 1);
                const unsigned char *idx_idfield = sqlite3_column_text(prepared_geo_col, 2);
                const unsigned char *unique_idfield = sqlite3_column_text(prepared_geo_col, 3);
                const unsigned char *geometryindex = sqlite3_column_text(prepared_geo_col, 4);
                const unsigned char *tri_index_field = sqlite3_column_text(prepared_geo_col, 5);
                oneLayer->utm_zone =  (uint8_t) sqlite3_column_int(prepared_geo_col, 6);
                oneLayer->hemisphere =  (uint8_t) sqlite3_column_int(prepared_geo_col, 7);
                oneLayer->n_dims = (uint8_t) sqlite3_column_int(prepared_geo_col, 8);



                //TODO free this



                if(info_rel)
                {
                    const unsigned char *info_relation = sqlite3_column_text(preparedLayerLoading, 14);
                    if(info_relation)
                    {
                        oneLayer->info_rel = malloc(2 * strlen((char*) info_relation)+1);
                        strcpy(oneLayer->info_rel,(char*) info_relation);
                    }
                    else
                        oneLayer->info_rel = NULL;

                }
                else
                    oneLayer->info_rel = NULL;
                //printf("name = %s\n", oneLayer->name);
                oneLayer->layer_id =  (uint8_t) layerid;



                char tri_idx_fld[32];
                if(oneLayer->type & 4)
                {
                    snprintf(tri_idx_fld, sizeof(tri_idx_fld), "%s", tri_index_field);
                }
                else
                {
                    snprintf(tri_idx_fld, sizeof(tri_idx_fld), "%s","'a'" );
                }

                if(sld_style_field && check_column(dbname,layername,(const unsigned char *) sld_style_field))
                {

                    snprintf(styleselect,sizeof(styleselect), ", %s",  sld_style_field  );


                }
                else
                {
                    snprintf(styleselect, sizeof(styleselect), ", %s","-1" );
                    oneLayer->style_key_type = INT_TYPE;
                }


                /*  snprintf(sql,sizeof(sql),"select e.%s, %s,e.%s,e.%s %s from %s.%s e inner join %s.%s ei on e.%s = ei.id where  ei.minX<? and ei.maxX>? and ei.minY<? and ei.maxY >?",
                          geometryfield,
                          tri_idx_fld,
                          idx_idfield,
                          unique_idfield,
                          styleselect,
                          dbname,
                          layername,
                          dbname,
                          geometryindex,
                          idx_idfield);*/


                if(show_text)
                {
                    if(text_field)
                        snprintf(textselect, sizeof(textselect), ",e.%s",text_field);
                    else
                        snprintf(textselect, sizeof(textselect), ",%s","'text_field is missing'");

                }
                else
                {
                    textselect[0] = '\0';
                }

                snprintf(sql,sizeof(sql),"select e.%s, %s,e.%s,e.%s %s %s from %s.%s e inner join %s.%s ei on e.%s = ei.id where  ei.minX<? and ei.maxX>? and ei.minY<? and ei.maxY >?",
                         geometryfield,
                         tri_idx_fld,
                         idx_idfield,
                         unique_idfield,
                         styleselect,
                         textselect,
                         dbname,
                         layername,
                         dbname,
                         geometryindex,
                         idx_idfield);
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





                if (oneLayer->type & 32)
                    oneLayer->text =  init_text_buf();

                if(text_field)
                    free(text_field);
                if(sld_style_field)
                    free(sld_style_field);

            }



            /*       oneLayer->res_buf =  init_res_buf();

                if (oneLayer->type & 4)
                    oneLayer->tri_index =  init_element_buf();
            */


            sqlite3_finalize(prepared_geo_col);


        }
        else
        {
            log_this(90, "Cannot use layer %s",layername);
            continue;
        }

        printf("layer loaded\n");
    }
    log_this(100,"layers prepared\n");
    global_layers->nlayers = i;
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
static int init_gps()
{
    gps_npoints = 10;
    gps_circle = create_circle(gps_npoints);
    //  gps_circle = create_symbol(4, 4,4, 0.5);
    loadGPS(gps_circle);
    return 0;
}

static int init_info_Layer()
{
    infoLayer = init_layer_runtime(1);

    /*We init all buffers */
    infoLayer->type = 255-32;
    init_buffers(infoLayer);


    infoRenderLayer = init_layer_runtime(1);

    /*We init all buffers */
    infoRenderLayer->type = 255-32;
    init_buffers(infoRenderLayer);

    infoRenderLayer->visible = 0;
    return 0;

}

int init_resources(char *dir)
{
    log_this(10, "Entering %s\n",__func__);
    map_modus = 1;
    TEXT *missing_db = init_txt(1024);
    curr_utm = 0;
    curr_hemi = 0;
    //char stylewhere[128];

    info_box_color[0] = 1;
    info_box_color[1] = 1;
    info_box_color[2] = 1;
    info_box_color[3] = 0.9;
    build_program();

    attach_db(dir, missing_db);

//   load_styles();
    add_system_default_style();
    init_symbols();
    loadSymbols();
    if(load_layers(missing_db))
        log_this(100, "There is a problem loading layers");
    destroy_txt(missing_db);
    init_gps();
    init_info_Layer();
    tmp_unicode_txt = init_wc_txt(256);
    return 0;
}



