@echo off

set path=%path%;%W91_TOOLCHAIN_PATH%/toolchains/nds32le-elf-mculib-v5/bin;%W91_TOOLCHAIN_PATH%/cygwin/bin
set binaryFile=%~dp0output\telink_arduino_wifi_signed.bin
set demoObject=%~dp0output\demo\demo_*.o

if exist %binaryFile% (
    del /f /s /q %binaryFile%
)

if exist %demoObject% (
    del /f /s /q %demoObject%
)

mingw32-make
if exist %binaryFile% (
    %~dp0download.bat %binaryFile% 0x80080000
)
