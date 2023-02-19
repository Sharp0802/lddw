set-location build
$pre_path = $env:PATH
$env:PATH = ($env:PATH.split(';') | where-object { $_ -like "*msys*" }) + ($env:PATH) -join ';'
cmake.exe CMakeLists.txt -G "MinGW Makefiles" -DCMAKE_C_COMPILER="$($args[0])" -DCMAKE_CXX_COMPILER="$($args[0])"
mingw32-make.exe
$env:PATH = $pre_path
set-location ..