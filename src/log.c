/**********************************************************************
 *
 * TileLessMap
 *
 * TileLessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TileLessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "log.h"
void log_this(int message_log_level, const char *log_txt, ... )
{


    if(message_log_level < LOGLEVEL)
        return;

    char log_txt_tot[2048];


    va_list args;
    va_start (args, log_txt);
    vsnprintf (log_txt_tot,2048,log_txt, args);
    va_end (args);

#ifdef __ANDROID__

    __android_log_print(ANDROID_LOG_INFO, APPNAME,"%s\n",log_txt_tot);
#else

    printf("%s\n",log_txt_tot);

#endif

    return;
}
