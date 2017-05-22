
#ifdef __ANDROID__
#include <android/log.h>
#endif

#include "global.h"
#include <stdarg.h>
#define LOGLEVEL 90

void log_this(int log_level, const char *log_txt, ... );
