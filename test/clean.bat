@echo off
set buildDir=%~dp0output
if exist %buildDir% (
    rd /s /q %buildDir%
)

