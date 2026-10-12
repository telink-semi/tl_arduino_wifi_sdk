@echo off
setlocal EnableDelayedExpansion

set "SCRIPT_DIR=%~dp0"
set "libList=%SCRIPT_DIR%lib_list.txt"
set "sdkDir=%SCRIPT_DIR%..\sdk"

if not exist "%libList%" (
    echo Error: lib list not found: %libList%
    exit /b 1
)

for /f "eol=# tokens=1* delims=	" %%A in ("%libList%") do call :process "%%A" "%%B"

if defined fail (
    echo Error: some libraries failed to download
    exit /b 1
)
echo all libraries are up to date
exit /b 0

:process
set "url=%~1"
set "relPath=%~2"
if "!relPath!"=="" exit /b 0

set "expectHash=!url:~-64!"
set "dest=!sdkDir!\!relPath!"

rem skip download if the local file already exists and matches the expected hash
if exist "!dest!" (
    set "hash="
    for /f "skip=1 tokens=*" %%h in ('certutil -hashfile "!dest!" SHA256') do set "hash=!hash!%%h"
    set "hash=!hash: =!"
    if /i "!hash!"=="!expectHash!" (
        echo ok: !relPath!
        exit /b 0
    )
)

for %%d in ("!dest!") do set "destDir=%%~dpd"
if not exist "!destDir!" mkdir "!destDir!"

echo download: !url!
echo       -^> !dest!
curl -fL "!url!" -o "!dest!"
if errorlevel 1 (
    echo Error: failed to download !url!
    del /q /f "!dest!" 2>nul
    set "fail=1"
    exit /b 0
)

set "hash="
for /f "skip=1 tokens=*" %%h in ('certutil -hashfile "!dest!" SHA256') do set "hash=!hash!%%h"
set "hash=!hash: =!"
if /i not "!hash!"=="!expectHash!" (
    echo Error: sha256 mismatch for !relPath!
    del /q /f "!dest!" 2>nul
    set "fail=1"
)
exit /b 0