# LDDW : LDD for Windows

A ldd(List Dynamic Dependencies) implementation for windows, written in C.

## SYNOPSIS

<pre>
lddw [<i>option</i>]... <i>file</i>...
</pre>

## DESCRIPTION

`lddw` prints the shard objects (shared libraries) required by each program or shared object specified on the command line.
An example of its use and output is the following:

```powershell
PS> lddw $(get-command rundll32).Source
        msvcrt.dll  =>  C:\WINDOWS\System32\msvcrt.dll
          ntdll.dll  =>  C:\WINDOWS\SYSTEM32\ntdll.dll
          KERNELBASE.dll  =>  C:\WINDOWS\System32\KERNELBASE.dll
            ntdll.dll  =>  C:\WINDOWS\SYSTEM32\ntdll.dll
        imagehlp.dll  =>  not found
        ntdll.dll  =>  C:\WINDOWS\SYSTEM32\ntdll.dll
```

In the usual case, `lddw` scan the import table section in PE header of file.
And if the import table is detected in PE header,
For each dependency that is in the import table,
`lddw` displays the location of the matching object and the (hexdecimal) address at which it is loaded.

### SECURITY

Unlinke original [`ldd(1)`](https://man7.org/linux/man-pages/man1/ldd.1.html),
`lddw` doesn't attempt to directly execute the program to obtain the dependency information.
Thus, You can use `lddw` on an untrusted executable.

## OPTIONS

-   `-h`, `--help`

Usage information

-   `-V`, `--version`

Print version information and exit

-   `-a`, `--all`

print all dependencies including API-set

## BUILDING

### Prerequisites

> Use `clang` and `lld` with MSYS2

-   C Compiler

`gcc` causes a link error when compile `lddw` because of its name mangling about Win32 APIs.
Thus, you need to use `clang` instead of `gcc`.

-   Linker

The standard dynamic linker (see [`ld`](https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_mono/ld.html)) causes a link error when compile `lddw`.
So, you need to use `lld` instead of the standard dynamic linker.

### Compiling

You can use build script file(see `project/build.ps1`) for powershell.
Below is how to use script file.

```powershell
Set-Location path/to/repository/
Set-Location project
./build.ps1 -Architecture x86-64 -Mode Release
```

If you cannot run powershell script directly, 
Instead of running powershell script, 
You can run `make` directly.

```sh
cd path/to/repository
cd project
mingw32-make clean
mingw32-make all ARCH=x86-64 MODE=Release
```
