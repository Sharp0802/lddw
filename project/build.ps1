param(
    [string]$Architecture,
    [string]$Mode
)

$Path = Get-Location
Set-Location $(Split-Path -Parent $MyInvocation.MyCommand.Definition)

if (($Mode -ne "Debug") -and ($Mode -ne "Release")) { 
    Write-Host "E: Unknown build mode" 
    Exit 1
}

mingw32-make.exe clean
mingw32-make.exe all MODE=$Mode ARCH=$Architecture

Set-Location $Path
