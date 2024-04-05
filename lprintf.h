#pragma once

#ifndef LPRINTF_H
#define LPRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern FILE *log_file;
extern uint32_t get_ms(void);

extern int lprintf(const char *format, ...);
extern int vlprintf(const char *format, va_list arg_ptr);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
