@echo OFF
rem Basic compilation script
rem You could try -std=c99, but file size will greatly grow!
rem Never tested MSVC here.
if "%1" == "-dev" (
rem "C:\MinGW\bin\gcc"
gcc -shared -std=gnu99 -c *.c -s -O3 -DDEVELOPMENT
) else (
gcc -shared -std=gnu99 -c *.c -s -O3 -DRELEASE
)
gcc -shared -std=gnu99 -o lightfx.dll *.o -s -O3
echo "Compiled!"