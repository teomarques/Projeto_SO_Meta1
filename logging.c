// logging.c
#include "deichain.h"
#include <stdarg.h>

void log_message(FILE* log_file, const char* format, ...) {
    va_list args;
    time_t now = time(NULL);
    char time_str[30];
    
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    va_start(args, format);
    printf("[%s] ", time_str);
    vprintf(format, args);
    
    if (log_file) {
        fprintf(log_file, "[%s] ", time_str);
        vfprintf(log_file, format, args);
        fflush(log_file);
    }
    va_end(args);
}