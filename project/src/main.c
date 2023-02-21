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
    "  -a, --all               print all dependencies including API-set\n"

#define RES_VERSION                                                                                                    \
    "lddw (Windows NT) 0.1\n"                                                                                          \
    "This is free software; see the source for copying conditions.\n"

#define ARGBASE_MAX INT32_MAX

int mainCRTStartup(void)
{
    int ret = 0;

    int argc = -1;
    int argbase = 1;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    struct LDD_ARGS args;
    memset(&args, 0, sizeof(args));

    if (argv == NULL)
    {
        _pmsgt(_T("lddw: Failed to parse command line\n"));
        ret = -1;
        goto FINALIZE;
    }
    else if (argc < 2)
    {
        _pmsgt(_T(RES_MISSING_FILE_ARGS));
        _pmsgt(_T(RES_HELP_MSG));
        ret = -1;
        argbase = ARGBASE_MAX;
    }
    else if (wcscmp(L"-h", argv[argbase]) == 0 || wcscmp(L"--help", argv[argbase]) == 0)
    {
        _pmsgt(_T(RES_HELP_MANUAL));
        argbase = ARGBASE_MAX;
    }
    else if (wcscmp(L"-V", argv[argbase]) == 0 || wcscmp(L"--version", argv[argbase]) == 0)
    {
        _pmsgt(_T(RES_VERSION));
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
            cwperr(argv[i], L"Failed to load dependencies");
            ret = -1;
        }
    }

FINALIZE:
    if (argv)
        LocalFree(argv);

    return ret;
}
