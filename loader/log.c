#include "log.h"
#include "loader.h"
#include <stdio.h>
#include <stdarg.h>

// Wii/console logging: prints to console
void log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("[ERROR] ");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

// Desktop logging: prints to console and logs to file
void log_error_file(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("[ERROR] ");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
    FILE *f = fopen("error.log", "a");
    if (f) {
        va_start(ap, fmt);
        fprintf(f, "[ERROR] ");
        vfprintf(f, fmt, ap);
        fprintf(f, "\n");
        va_end(ap);
        fclose(f);
    }
}
