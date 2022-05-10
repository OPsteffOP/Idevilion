@echo off
cd ..\tools
VSSolutionBuilder.exe "..\src\Tests\Tests.sfb" windows
pause