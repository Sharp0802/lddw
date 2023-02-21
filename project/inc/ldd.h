#ifndef LDDW_LDD_H
#define LDDW_LDD_H

#include "fw.h"
#include "log.h"

struct LDD_ARGS
{
    bool bViewAll;
};

int ldd(struct LDD_ARGS args, LPCWSTR fname);

#endif
