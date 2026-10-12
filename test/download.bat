@echo off
set toolPath=%~dp0../cli/tool
set downloadTool=%toolPath%/sctool.exe
set rambin=%toolPath%/da/da.ram.bin
set binaryFile=%1
set port=COM21
set speed=2000000
set addr=%2
echo -------------------Start Download[%binaryFile%]-------------------
@REM %downloadTool% -p %port% -b %speed% -da %rambin% da_ver
@REM %downloadTool% -p %port% -b %speed% -da %rambin% hw_reset
%downloadTool% -p %port% -b %speed% -da %rambin% --after hw_reset flash_write %addr% %binaryFile%
@REM %downloadTool% -p %port% -b %speed% -da %rambin% hw_reset
echo -----------------Download Compelete[%binaryFile%]-----------------