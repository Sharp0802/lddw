#include "fw.h"
#include "ldd.h"
#include "log.h"

#define RES_MISSING_FILE_ARGS "lddw: missing file arguments\n"
#define RES_HELP_MSG "Try `lddw --help' for more information.\n"
#define RES_HELP_MANUAL                                                                                                \
    "Usage: lddw [OPTION]... FILE...\n"                                                                                \
    "\n"                                                                                                               \
    "Print shared library dependencies\n"                                                                              \
    "\n"                                                                                                               \
    "  -h, --help              print this help and exit\n"                                                             \
    "  -V, --version           print version information and exit\n"                                                   \
    "  -a, --all               print all dependencies including API-set"

#define RES_VERSION                                                                                                    \
    "lddw (Windows NT) 0.1\n"                                                                                          \
    "This is free software; see the source for copying conditions."

#define ARGBASE_MAX INT32_MAX

#if TRUE
char __mingw_module_is_dll = 0;

int mainCRTStartup(void)
#else
int main(void)
#endif
{
    int ret = 0;

    int argc = -1;
    int argbase = 1;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    struct LDD_ARGS args;
    memset(&args, 0, sizeof(args));

    if (argv == NULL)
    {
        _fputts(_T("lddw: Failed to parse command line\n"), stderr);
        ret = -1;
        goto FINALIZE;
    }
    else if (argc < 2)
    {
        _fputts(_T(RES_MISSING_FILE_ARGS), stderr);
        _fputts(_T(RES_HELP_MSG), stderr);
        ret = -1;
        argbase = ARGBASE_MAX;
    }
    else if (wcscmp(L"-h", argv[argbase]) == 0 || wcscmp(L"--help", argv[argbase]) == 0)
    {
        _putts(_T(RES_HELP_MANUAL));
        argbase = ARGBASE_MAX;
    }
    else if (wcscmp(L"-V", argv[argbase]) == 0 || wcscmp(L"--version", argv[argbase]) == 0)
    {
        _putts(_T(RES_VERSION));
        argbase = ARGBASE_MAX;
    }
    else if (wcscmp(L"-a", argv[argbase]) == 0 || wcscmp(L"--all", argv[argbase]) == 0)
    {
        args.bViewAll = true;
        argbase++;
    }

    for (int i = argbase; i < argc; ++i)
    {
        if (ldd(args, argv[i]) < 0)
        {
            _ctperr(argv[i], _T("Failed to load dependencies"));
            ret = -1;
        }
    }

FINALIZE:
    if (argv != NULL)
        LocalFree(argv);

    return ret;
}
