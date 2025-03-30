// logging.h
#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <time.h>

#define DEBUG 1

void log_event(const char *message) {
    FILE *log_file = fopen("DEIChain_log.txt", "a");
    if (!log_file) {
        perror("fopen");
        return;
    }
    time_t now = time(NULL);
    fprintf(log_file, "[%ld] %s\n", now, message);
    fclose(log_file);
    printf("[%ld] %s\n", now, message);
}

#if DEBUG
#define debug_log(msg) log_event(msg)
#else
#define debug_log(msg)
#endif

#endif // LOGGING_H
