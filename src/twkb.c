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


static int get_blob(TWKB_BUF *tb,sqlite3_stmt *res, int icol)
{


    /*twkb-buffer*/
    uint8_t *buf;
    size_t buf_len;
    const sqlite3_blob *db_blob;

    db_blob = sqlite3_column_blob(res, icol);

    buf_len = sqlite3_column_bytes(res, icol);
    //   log_this(10, "blob size;%d\n", buf_len);
    buf = malloc(buf_len);
    memcpy(buf, db_blob,buf_len);



    tb->start_pos = tb->read_pos=buf;
    tb->end_pos=buf+buf_len;
    //printf("allocate buffer at %p\n",tb->start_pos);

    return 0;


}

void *twkb_fromSQLiteBBOX_thread(void *theL)
{
    twkb_fromSQLiteBBOX(theL);

    pthread_exit(NULL);
    return NULL;
}

void *twkb_fromSQLiteBBOX(void *theL)
{
    log_this(10, "Entering twkb_fromSQLiteBBOX, prepared = %p\n", ((LAYER_RUNTIME*) theL)->preparedStatement);
    /*twkb structures*/
    TWKB_HEADER_INFO thi;
    TWKB_PARSE_STATE ts;
    TWKB_BUF tb;
    sqlite3_stmt *prepared_statement;

    GLfloat *ext;
    BBOX bbox;
    ts.thi = &thi;
    ts.thi->bbox=&bbox;
    LAYER_RUNTIME *theLayer = (LAYER_RUNTIME *) theL;

    ts.theLayer = (LAYER_RUNTIME *) theL;

    GLfloat rotation;
    GLint anchor;
    int size;
//log_this(10, "sqlite_error? %d\n",sqlite3_config(SQLITE_CONFIG_SERIALIZED ));


    prepared_statement = theLayer->preparedStatement;
    ext = theLayer->BBOX;
    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);


    int err = sqlite3_errcode(projectDB);
    if(err)
        log_this(1,"sqlite problem, %d\n",err);

    if((theLayer->utm_zone != curr_utm) || (theLayer->hemisphere != curr_hemi))
    {
        GLfloat reproj_coord[2];
        GLfloat maxx;
        GLfloat maxy;
        GLfloat minx;
        GLfloat miny;




        reproj_coord[0] = ext[0];
        reproj_coord[1] = ext[1];

        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);
        minx = reproj_coord[0];
        miny = reproj_coord[1];

        reproj_coord[0] = ext[0];
        reproj_coord[1] = ext[3];
        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);

        if(minx>reproj_coord[0])
            minx = reproj_coord[0];

        maxy = reproj_coord[1];


        reproj_coord[0] = ext[2];
        reproj_coord[1] = ext[3];
        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);

        if(maxy<reproj_coord[1])
            maxy = reproj_coord[1];

        maxx = reproj_coord[0];

        reproj_coord[0] = ext[2];
        reproj_coord[1] = ext[1];
        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);

        if(maxx<reproj_coord[0])
            maxx = reproj_coord[0];

        if(miny > reproj_coord[1])
            miny = reproj_coord[1];


        sqlite3_bind_double(prepared_statement, 1,(float) maxx); //minX
        sqlite3_bind_double(prepared_statement, 2,(float) minx); //minY
        sqlite3_bind_double(prepared_statement, 3,(float) maxy); //maxX
        sqlite3_bind_double(prepared_statement, 4,(float) miny); //maxY


    }
    else
    {


        sqlite3_bind_double(prepared_statement, 1,(float) ext[2]); //maxX
        sqlite3_bind_double(prepared_statement, 2,(float) ext[0]); //minX
        sqlite3_bind_double(prepared_statement, 3,(float) ext[3]); //maxY
        sqlite3_bind_double(prepared_statement, 4,(float) ext[1]); //minY
        log_this(10, "1 = %f, 2 = %f, 3 = %f, 4 = %f\n", ext[2],ext[0],ext[3],ext[1]);

    }



    err = sqlite3_errcode(projectDB);
    if(err)
        log_this(1,"sqlite problem 2, %d\n",err);




    while (sqlite3_step(prepared_statement)==SQLITE_ROW)
    {
        ts.id = sqlite3_column_int(prepared_statement, 2);
        printf("id fra db = %ld\n",ts.id);
        ts.styleID = sqlite3_column_int(prepared_statement, 3);
        if(get_blob(&tb,prepared_statement,0))
        {
            log_this(1,"Failed to select data\n");

            sqlite3_close(projectDB);
            return NULL;
        }
        ts.tb=&tb;
        ts.utm_zone = theLayer->utm_zone;
        ts.hemisphere = theLayer->hemisphere;
        while (ts.tb->read_pos<ts.tb->end_pos)
        {
            decode_twkb(&ts);//, theLayer->res_buf);
        }
//printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
        free(tb.start_pos);
        if(theLayer->type & 4)
        {
            if(get_blob(&tb,prepared_statement,1))
            {
                fprintf(stderr, "Failed to select data\n");

                sqlite3_close(projectDB);
                return NULL;
            }
            ts.tb=&tb;

            while (ts.tb->read_pos<ts.tb->end_pos)
            {
                decode_element_array(&ts);
            }
//printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
            free(tb.start_pos);



        }
        if(theLayer->type & 32)
        {
            const char *txt = (const char*) sqlite3_column_text(prepared_statement, 4);

            size = sqlite3_column_int(prepared_statement, 5);
            rotation = (GLfloat) sqlite3_column_double(prepared_statement, 6);
            anchor = (GLint) sqlite3_column_double(prepared_statement, 7);

            text_write(txt,0, (GLshort) size, rotation,anchor, theLayer->text);
        }

    }
    sqlite3_clear_bindings(prepared_statement);
    sqlite3_reset(prepared_statement);

    return NULL;
}





