#ifndef LDDW_LOG_H
#define LDDW_LOG_H

#include "fw.h"

#define APP_NAME "lddw"

void reqbr(void);

void pmsg(LPCSTR msg);
void pmsgw(LPCWSTR msg);

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
#define _pmsgt pmsgw
#else
#define _ctperr cperr
#define _ctperre cperre
#define _ctperrle cperrle
#define _pmsgt pmsg
#endif

void pu32x(unsigned int u32);
void pu64x(unsigned long long u64);

#ifdef _WIN64
#define pld64x pu64x
#else
#define pld64x pu32x
#endif

#endif
