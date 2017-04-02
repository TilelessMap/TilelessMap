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
    //   DEBUG_PRINT(("blob size;%d\n", buf_len));
    buf = malloc(buf_len);
    memcpy(buf, db_blob,buf_len);



    tb->start_pos = tb->read_pos=buf;
    tb->end_pos=buf+buf_len;
    //printf("allocate buffer at %p\n",tb->start_pos);
    return 0;


}



void *twkb_fromSQLiteBBOX(void *theL)
{
    DEBUG_PRINT(("Entering twkb_fromSQLiteBBOX\n"));
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

    float rotation, anchor;
    int size;
//DEBUG_PRINT(("sqlite_error? %d\n",sqlite3_config(SQLITE_CONFIG_SERIALIZED )));


    prepared_statement = theLayer->preparedStatement;
    ext = theLayer->BBOX;
    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);


    int err = sqlite3_errcode(projectDB);
    if(err)
        fprintf(stderr,"sqlite problem, %d\n",err);

    sqlite3_bind_double(prepared_statement, 1,(float) ext[2]); //maxX
    sqlite3_bind_double(prepared_statement, 2,(float) ext[0]); //minX
    sqlite3_bind_double(prepared_statement, 3,(float) ext[3]); //maxY
    sqlite3_bind_double(prepared_statement, 4,(float) ext[1]); //minY


    DEBUG_PRINT(("1 = %f, 2 = %f, 3 = %f, 4 = %f\n", ext[2],ext[0],ext[3],ext[1]));
    
    err = sqlite3_errcode(projectDB);
    if(err)
        fprintf(stderr,"sqlite problem 2, %d\n",err);

    
    while (sqlite3_step(prepared_statement)==SQLITE_ROW)
    {
        ts.id = sqlite3_column_int(prepared_statement, 2);
        ts.styleID = sqlite3_column_int(prepared_statement, 3);
        if(get_blob(&tb,prepared_statement,0))
        {
            fprintf(stderr, "Failed to select data\n");

            sqlite3_close(projectDB);
            return NULL;
        }
        ts.tb=&tb;

        while (ts.tb->read_pos<ts.tb->end_pos)
        {
            decode_twkb(&ts, theLayer->res_buf);
        }
//printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
        free(tb.start_pos);
        if(theLayer->geometryType == POLYGONTYPE)
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
                decode_element_array(&ts, theLayer->tri_index);
            }
//printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
            free(tb.start_pos);



        }
        if(theLayer->show_text)
	{
	  const char *txt = (const char*) sqlite3_column_text(prepared_statement, 4);

	  size = sqlite3_column_int(prepared_statement, 5);
	  rotation = sqlite3_column_double(prepared_statement, 6);
	  anchor = sqlite3_column_double(prepared_statement, 7);  
	 
	  text_write(txt,0, size, rotation,anchor, theLayer->text);
	}

    }
    sqlite3_clear_bindings(prepared_statement);
    sqlite3_reset(prepared_statement);


    pthread_exit(NULL);
}
