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
    snprintf(sql, 1024, "select name from %s.sqlite_master where type in ('table','view') and name = '%s'", dbname, layername);

    rc = sqlite3_prepare_v2(projectDB, sql, -1, &prepared_sql, 0);

    if (rc != SQLITE_OK ) {
        log_this(110, "SQL error in %s\n",sql);
        return 0;
    }


    if(sqlite3_step(prepared_sql) ==  SQLITE_ROW)
    {
        sqlite3_finalize(prepared_sql);
        return 1;
    }
    else
    {

        sqlite3_finalize(prepared_sql);
        return 0;
    }

}
