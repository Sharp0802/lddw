#ifndef LDDW_DICT_H
#define LDDW_DICT_H

#include "fw.h"
#include "log.h"

typedef void (*LPPRINT)(const void *const a);
typedef bool (*LPEQCMP)(const void *restrict const a, const void *restrict const b);

struct DICT
{
    int itemn; // actual number
    int itemc; // capacitor
    void **itemv;
    LPEQCMP equals;
};

struct DICT dict__new(const LPEQCMP lpfnEqCmp);
bool dict__add_force(struct DICT *dict, void *item, int size);
bool dict__add(struct DICT *dict, void *item, int size);
void dict__free(struct DICT *dict);
void dict__print(struct DICT *dict, LPPRINT lpfnPrint);

void __print_wcs(const void *const a);
bool __eqcmp_wcs(const void *restrict const a, const void *restrict const b);

#endif
