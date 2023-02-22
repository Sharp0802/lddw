#ifndef LDDW_LDD_H
#define LDDW_LDD_H

#include "dict.h"
#include "fw.h"
#include "log.h"

struct LDD_ARGS
{
    bool bFlatten;
    bool bViewAll;
};

int ldd(struct LDD_ARGS args, LPCWSTR fname);

#endif
