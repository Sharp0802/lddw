#include "log.h"

#define _L(t) L##t
#define L(t) _L(t)
#define FORMAT_FLAGS (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER)

#define __typ_str LPCSTR
#define __loc_fmt FormatMessageA
#define __loc_puts(m) fputs(m, stderr)
#define __loc_prtx(i) fprintf(stderr, "0x%08lX", i)
#define __loc_prte __impl_cperre
#define __exp_prte cperre
#define __str(t) t

void __impl_cperre(const __typ_str fname, const __typ_str msg, DWORD err)
{
    __loc_puts(__str(APP_NAME));
    __loc_puts(__str(": "));
    __loc_puts(fname);
    __loc_puts(__str(": "));
    __loc_prtx(err);
    __loc_puts(__str(": "));
    __loc_puts(msg);
    if (msg[strlen(msg) - 1] != '\n')
        __loc_puts(__str("\n"));
}

void cperr(const __typ_str fname, const __typ_str msg)
{
    __loc_puts(__str(APP_NAME));
    __loc_puts(__str(": "));
    __loc_puts(fname);
    __loc_puts(__str(": "));
    __loc_puts(msg);
    __loc_puts(__str("\n"));
}

void cperre(const __typ_str fname, DWORD err)
{
    __typ_str buf;
    DWORD ret = __loc_fmt(FORMAT_FLAGS, NULL, err, 0, (void *)&buf, 0, NULL);
    if (ret == 0)
    {
        __loc_prte(fname, __str("Unknown error"), err);
        __loc_prte(fname, __str("Failed to format error message"), GetLastError());
    }
    else
    {
        __loc_prte(fname, buf, err);
    }
    LocalFree((HLOCAL)buf);
}

void cperrle(const __typ_str fname)
{
    __exp_prte(fname, GetLastError());
}

#undef __typ_str
#undef __loc_fmt
#undef __loc_puts
#undef __loc_prtx
#undef __loc_prte
#undef __exp_prte
#undef __str

#define __typ_str LPCWSTR
#define __loc_fmt FormatMessageW
#define __loc_puts(m) fputws(m, stderr)
#define __loc_prtx(i) fwprintf(stderr, L"0x%08lX", i)
#define __loc_prte __impl_cwperre
#define __exp_prte cwperre
#define __str(t) L(t)

void __impl_cwperre(const __typ_str fname, const __typ_str msg, DWORD err)
{
    __loc_puts(__str(APP_NAME));
    __loc_puts(__str(": "));
    __loc_puts(fname);
    __loc_puts(__str(": "));
    __loc_prtx(err);
    __loc_puts(__str(": "));
    __loc_puts(msg);
    if (msg[wcslen(msg) - 1] != L'\n')
        __loc_puts(__str("\n"));
}

void cwperr(const __typ_str fname, const __typ_str msg)
{
    __loc_puts(__str(APP_NAME));
    __loc_puts(__str(": "));
    __loc_puts(fname);
    __loc_puts(__str(": "));
    __loc_puts(msg);
    __loc_puts(__str("\n"));
}

void cwperre(const __typ_str fname, DWORD err)
{
    __typ_str buf;
    DWORD ret = __loc_fmt(FORMAT_FLAGS, NULL, err, 0, (void *)&buf, 0, NULL);
    if (ret == 0)
    {
        __loc_prte(fname, __str("Unknown error"), err);
        __loc_prte(fname, __str("Failed to format error message"), GetLastError());
    }
    else
    {
        __loc_prte(fname, buf, err);
    }
    LocalFree((HLOCAL)buf);
}

void cwperrle(const __typ_str fname)
{
    __exp_prte(fname, GetLastError());
}

#undef __typ_str
#undef __loc_fmt
#undef __loc_puts
#undef __loc_prtx
#undef __loc_prte
#undef __exp_prte
#undef __str
