#ifndef LDDS_FW_H
#define LDDS_FW_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include <errno.h>
#include <psapi.h>
#include <tchar.h>
#include <tlhelp32.h>

#include <unistd.h>

#undef ERROR

/* https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation */
#ifdef MAX_PATH
#undef MAX_PATH
#endif

#define MAX_PATH_A 260
#define MAX_PATH_W 32767

#ifdef UNICODE
#define MAX_PATH MAX_PATH_W
#else
#define MAX_PATH MAX_PATH_A
#endif

#endif
