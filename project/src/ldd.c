#include "ldd.h"

#define OpenFileR(f) CreateFile(f, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)

int __findlib(LPCSTR fname, LPSTR fpath, DWORD size);
int __impl_static_ldd(struct LDD_ARGS args, LPCVOID fdat, LPCTSTR fname, int depth);
int __impl_ldd(struct LDD_ARGS args, LPCTSTR fname, int depth);
DWORD __rva2offset(DWORD rva, PIMAGE_SECTION_HEADER secH, PIMAGE_NT_HEADERS ntH);

int ldd(const struct LDD_ARGS args, const LPCTSTR fname)
{
    return __impl_ldd(args, fname, 0);
}

int __impl_ldd(const struct LDD_ARGS args, const LPCTSTR fname, int depth)
{
    TCHAR fpath[MAX_PATH];
    HANDLE fp;
    HANDLE fmap;
    HANDLE fbase;
    int ret = 0;

    if (GetFullPathName(fname, MAX_PATH, fpath, NULL) == 0)
    {
        _ctperr(fname, _T("Failed to get full path"));
        return -1;
    }

    ret = 0;
    fp = OpenFileR(fpath);
    fmap = CreateFileMapping(fp, NULL, PAGE_READONLY, 0, 0, NULL);
    if (fmap == NULL)
    {
        _ctperr(fname, _T("Failed to create file mapping"));
        _ctperrle(fname);
        ret = -1;
        goto FINALIZE;
    }
    fbase = MapViewOfFile(fmap, FILE_MAP_READ, 0, 0, 0);
    if (fbase == NULL)
    {
        _ctperr(fname, _T("Failed to view file mapping"));
        _ctperrle(fname);
        ret = -1;
        goto FINALIZE;
    }

    ret = __impl_static_ldd(args, fbase, fname, depth);

FINALIZE:
    UnmapViewOfFile(fbase);
    CloseHandle(fmap);
    CloseHandle(fp);
    return ret;
}

int __impl_static_ldd(struct LDD_ARGS args, const LPCVOID fdat, const LPCTSTR fname, int depth)
{
    PIMAGE_DOS_HEADER dosH = (PIMAGE_DOS_HEADER)fdat;
    PIMAGE_NT_HEADERS ntH = (PIMAGE_NT_HEADERS)((ULONG_PTR)fdat + dosH->e_lfanew);
    PIMAGE_SECTION_HEADER secH = IMAGE_FIRST_SECTION(ntH);
    DWORD impRVA;
    PIMAGE_IMPORT_DESCRIPTOR impDesc;

    if (dosH->e_magic != IMAGE_DOS_SIGNATURE)
    {
        _ctperr(fname, _T("DOS signature (MZ) not matched"));
        return -1;
    }
    if (ntH->Signature != IMAGE_NT_SIGNATURE)
    {
        _ctperr(fname, _T("PE signature (PE) not matched"));
        return -1;
    }
    if (ntH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
    {
        // _ctperr(fname, _T("Import table not detected"));
        return -1;
    }

    impRVA = ntH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    impDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)fdat + __rva2offset(impRVA, secH, ntH));
    while (impDesc->Name != 0)
    {
        LPCSTR name = (LPCSTR)((ULONG_PTR)fdat + __rva2offset(impDesc->Name, secH, ntH));
        if ((strncmp("ext-", name, 4) == 0 || strncmp("api-", name, 4) == 0) && !args.bViewAll)
        {
            goto CONTINUE; // do not call continue. it causes infinite loops
        }

        CHAR fpath[MAX_PATH_A];
        for (int i = 0; i < depth; ++i)
            fputs("  ", stdout);
        if (__findlib(name, fpath, MAX_PATH_A) != 0)
        {
            HMODULE module = GetModuleHandleA(name);
            if (module != NULL)
            {
                printf("        %s  =>  not found (0x%08X)\n", name, fpath, module);
            }
            else
            {
                printf("        %s  =>  not found (not loaded)\n", name, fpath);
            }
        }
        else
        {
            HMODULE module = GetModuleHandleA(name);
            if (module != NULL)
            {
                printf("        %s  =>  %s (0x%08X)\n", name, fpath, module);
            }
            else
            {
                printf("        %s  =>  %s (not loaded)\n", name, fpath);
            }
            TCHAR tfpath[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, fpath, MAX_PATH_A, tfpath, MAX_PATH);
            __impl_ldd(args, tfpath, depth + 1);
        }

    CONTINUE:
        impDesc++;
    }

    return 0;
}

int __findlib(const LPCSTR fname, const LPSTR fpath, DWORD size)
{
    HMODULE lib = LoadLibraryExA(fname, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (lib == NULL)
    {
        // cperr(fname, "Failed to load module");
        // cperrle(fname);
        return -1;
    }
    DWORD ret = GetModuleFileNameA(lib, fpath, size);
    FreeLibrary(lib);
    if (ret == 0)
    {
        // cperr(fname, "Failed to get module file name");
        // cperrle(fname);
        return -1;
    }
    return 0;
}

DWORD __rva2offset(DWORD rva, const PIMAGE_SECTION_HEADER secH, const PIMAGE_NT_HEADERS ntH)
{
    PIMAGE_SECTION_HEADER cpySecH = secH;
    if (rva == 0)
        return rva;
    for (size_t i = 0; i < ntH->FileHeader.NumberOfSections; ++i)
    {
        if (rva >= cpySecH->VirtualAddress && rva < cpySecH->VirtualAddress + cpySecH->Misc.VirtualSize)
            break;
        cpySecH++;
    }
    return rva - cpySecH->VirtualAddress + cpySecH->PointerToRawData;
}
