#include "ldd.h"

#define OpenFileR(f) CreateFileW(f, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)

int __findlib(LPCWSTR fname, LPWSTR fpath, DWORD size);
int __impl_static_ldd(struct LDD_ARGS args, LPCVOID fdat, LPCWSTR fname, int depth);
int __impl_ldd(struct LDD_ARGS args, LPCWSTR fname, int depth);
DWORD __rva2offset(DWORD rva, PIMAGE_SECTION_HEADER secH, PIMAGE_NT_HEADERS ntH);

int ldd(const struct LDD_ARGS args, const LPCWSTR fname)
{
    return __impl_ldd(args, fname, 0);
}

int __impl_ldd(const struct LDD_ARGS args, const LPCWSTR fname, int depth)
{
    LPWSTR fpath = malloc(MAX_PATH_W * sizeof(WCHAR));
    fpath[0] = 0;
    if (GetFullPathNameW(fname, MAX_PATH_W, fpath, NULL) == 0)
    {
        _ctperr(fname, _T("Failed to get full path"));
        return -1;
    }

    int ret = 0;
    HANDLE fp = OpenFileR(fpath);
    free(fpath);
    if (fp == NULL)
    {
        _ctperr(fname, _T("Failed to open file"));
        _ctperrle(fname);
        ret = -1;
        goto FINALIZE_FP;
    }
    HANDLE fmap = CreateFileMappingW(fp, NULL, PAGE_READONLY, 0, 0, NULL);
    if (fmap == NULL)
    {
        _ctperr(fname, _T("Failed to create file mapping"));
        _ctperrle(fname);
        ret = -1;
        goto FINALIZE_FMAP;
    }
    HANDLE fbase = MapViewOfFile(fmap, FILE_MAP_READ, 0, 0, 0);
    if (fbase == NULL)
    {
        _ctperr(fname, _T("Failed to view file mapping"));
        _ctperrle(fname);
        ret = -1;
        goto FINALIZE_FBASE;
    }

    ret = __impl_static_ldd(args, fbase, fname, depth);

FINALIZE_FBASE:
    if (fbase)
        UnmapViewOfFile(fbase);
FINALIZE_FMAP:
    if (fmap)
        CloseHandle(fmap);
FINALIZE_FP:
    if (fp)
        CloseHandle(fp);
    return ret;
}

int __impl_static_ldd(struct LDD_ARGS args, const LPCVOID fdat, const LPCWSTR fname, int depth)
{
    PIMAGE_DOS_HEADER dosH = (PIMAGE_DOS_HEADER)fdat;
    PIMAGE_NT_HEADERS ntH = (PIMAGE_NT_HEADERS)((ULONG_PTR)fdat + dosH->e_lfanew);
    PIMAGE_SECTION_HEADER secH = IMAGE_FIRST_SECTION(ntH);
    DWORD impRVA;
    PIMAGE_IMPORT_DESCRIPTOR impDesc;

    // filter non-DOS binary
    if (dosH->e_magic != IMAGE_DOS_SIGNATURE)
    {
        if (depth == 0)
            cwperr(fname, _T("DOS signature (MZ) not matched"));
        return -1;
    }
    // filter non-NT binary
    if (ntH->Signature != IMAGE_NT_SIGNATURE)
    {
        if (depth == 0)
            cwperr(fname, _T("PE signature (PE) not matched"));
        return -1;
    }

    // filter independent PE file
    if (ntH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
    {
        if (depth == 0)
            cwperr(fname, _T("Dependency not found"));
        return -1;
    }

    impRVA = ntH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    impDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)fdat + __rva2offset(impRVA, secH, ntH));
    for (; impDesc->Name != 0; ++impDesc)
    {
        // convert name from UTF-8 to UTF-16
        WCHAR namew[MAX_PATH_W];
        namew[0] = 0;
        {
            LPCSTR namea = (LPCSTR)((ULONG_PTR)fdat + __rva2offset(impDesc->Name, secH, ntH));

            // determine if library is Windows API sets
            if ((strncmp("ext-", namea, 4) == 0 || strncmp("api-", namea, 4) == 0) && !args.bViewAll)
                continue;

            MultiByteToWideChar(CP_UTF8, 0, namea, strlen(namea) + 1, namew, MAX_PATH_W);
        }

        // print indents
        for (int i = 0; i < depth + 4; ++i)
            pmsg("  ");

        // print library name
        pmsgw(namew);
        pmsg("  =>  ");

        // print library path
        LPWSTR fpath = malloc(MAX_PATH_W * sizeof(WCHAR));
        fpath[0] = 0;

        int found = __findlib(namew, fpath, MAX_PATH_W);
        pmsgw((found == 0) ? fpath : L"not found");

        // print library address
        pmsg(" (");
        HMODULE module = GetModuleHandleW(namew);
        if (module != NULL)
        {
            pmsg("0x");
            pld64x((size_t)module);
        }
        else
        {
            pmsg("not loaded");
        }
        pmsg(")\n");

        // recurse listing
        if (found == 0)
        {
            __impl_ldd(args, fpath, depth + 1);
        }

        free(fpath);
    }

    return 0;
}

int __findlib(const LPCWSTR fname, const LPWSTR fpath, DWORD size)
{
    HMODULE lib = LoadLibraryExW(fname, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (lib == NULL)
    {
        cwperr(fname, L"Failed to load module");
        cwperrle(fname);
        return -1;
    }

    LPWSTR buf = malloc(MAX_PATH_W * sizeof(WCHAR));
    buf[0] = 0;
    DWORD ret = GetMappedFileNameW(GetCurrentProcess(), lib, buf, MAX_PATH_W);
    if (ret == 0)
    {
        cwperr(fname, L"Failed to get module file name");
        cwperrle(fname);
        FreeLibrary(lib);
        free(buf);
        return -1;
    }

    LPWSTR tmp = malloc(512 * sizeof(WCHAR));
    tmp[0] = 0;
    if (GetLogicalDriveStringsW(511, tmp) == 0)
    {
        cwperr(fname, L"Failed to get logical drive name");
        cwperrle(fname);
        FreeLibrary(lib);
        free(buf);
        free(tmp);
        return -1;
    }

    WCHAR driv[3];
    PWCHAR p = tmp;
    BOOL found = 0;

    driv[0] = L' ';
    driv[1] = L':';
    driv[2] = 0;

    do
    {
        WCHAR name[MAX_PATH_W];
        name[0] = 0;

        driv[0] = p[0];
        if (!QueryDosDeviceW(driv, name, MAX_PATH_W))
        {
            continue;
        }

        size_t namelen = wcslen(name);
        if (namelen < MAX_PATH_W)
        {
            found = wcsnicmp(buf, name, namelen) == 0 && *(buf + namelen) == L'\\';
            if (found)
            {
                WCHAR tmpfile[MAX_PATH_W];
                tmpfile[0] = 0;
                wcscat_s(tmpfile, MAX_PATH_W, driv);
                wcscat_s(tmpfile, MAX_PATH_W, (PWCHAR)(buf + namelen));
                wcscpy_s(buf, MAX_PATH_W, tmpfile);
            }
        }

        while (*p++)
            ;
    } while (!found && *p);

    wcscpy_s(fpath, size, buf);
    FreeLibrary(lib);
    free(buf);
    free(tmp);
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
