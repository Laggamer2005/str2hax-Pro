#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

void log_error(const char *fmt, ...);
void log_error_file(const char *fmt, ...);

#endif // LOG_H
