#include "dict.h"

struct DICT dict__new(const LPEQCMP lpfnEqCmp)
{
    struct DICT dict;
    memset(&dict, 0, sizeof(dict));
    dict.itemn = 0;
    dict.itemc = 0;
    dict.itemv = NULL;
    dict.equals = lpfnEqCmp;
    return dict;
}

bool dict__add_force(struct DICT *dict, void *item, int size)
{
    if (dict == NULL || dict->equals == NULL)
        return false;

    dict->itemn++;
    if (dict->itemn > dict->itemc)
    {
        if (dict->itemv == NULL)
        {
            dict->itemc = 1;
            dict->itemv = malloc(dict->itemc * sizeof(void *));
        }
        else
        {
            dict->itemc *= 2;
            dict->itemv = realloc(dict->itemv, dict->itemc * sizeof(void *));
        }
    }

    dict->itemv[dict->itemn - 1] = malloc(size);
    memcpy(dict->itemv[dict->itemn - 1], item, size);

    return true;
}

bool dict__add(struct DICT *dict, void *item, int size)
{
    for (int i = 0; i < dict->itemn; ++i)
    {
        if (dict->itemv[i] == item || dict->equals(dict->itemv[i], item))
            return false;
    }

    dict__add_force(dict, item, size);
    return true;
}

void dict__free(struct DICT *dict)
{
    for (int i = 0; i < dict->itemn; ++i)
        free(dict->itemv[i]);
    free(dict->itemv);

    dict->itemn = 0;
    dict->itemc = 0;
    dict->itemv = NULL;
}

void dict__print(struct DICT *dict, LPPRINT lpfnPrint)
{
    for (int i = 0; i < dict->itemn; ++i)
        lpfnPrint(dict->itemv[i]);
}

void __print_wcs(const void *const a)
{
    reqbr();
    pmsgw(a);
    pmsgw(L"\n");
}

bool __eqcmp_wcs(const void *restrict const a, const void *restrict const b)
{
    return wcscmp(a, b) == 0;
}
