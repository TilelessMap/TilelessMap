
#include <stdio.h>
#include <stdarg.h>
#include "log.h"
void log_this(int log_level, const char *log_txt, ... )
{


    if(log_level <= LOGLEVEL)
        return;

    char log_txt_tot[1024];


    va_list args;
    va_start (args, log_txt);
    vsnprintf (log_txt_tot,1024,log_txt, args);
    va_end (args);

#ifdef __ANDROID__

    __android_log_print(ANDROID_LOG_INFO, APPNAME,"%s\n",log_txt_tot);
#else

    printf("%s\n",log_txt_tot);

#endif

    return;
}
