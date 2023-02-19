#ifndef LDDW_LOG_H
#define LDDW_LOG_H

#include "fw.h"

#define APP_NAME "lddw"

void cperr(LPCSTR fname, LPCSTR msg);
void cperre(LPCSTR fname, DWORD err);
void cperrle(LPCSTR fname);

void cwperr(LPCWSTR fname, LPCWSTR msg);
void cwperre(LPCWSTR fname, DWORD err);
void cwperrle(LPCWSTR fname);

#ifdef UNICODE
#define _ctperr cwperr
#define _ctperre cwperre
#define _ctperrle cwperrle
#else
#define _ctperr cperr
#define _ctperre cperre
#define _ctperrle cperrle
#endif

#endif
