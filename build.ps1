$CC = ("clang", "gcc") -eq $args[0]
if ($CC.Count -eq 0) {
    write-host "failed to get C compiler"
    exit
}

write-host "=== LLDW"
set-location project
./build.ps1 $CC

write-host "=== FINALIZE"
set-location ..
copy-item -force -recurse -path project/bin -destination .
