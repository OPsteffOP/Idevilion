@echo off
cd ..
del /S /Q /F *.sln
del /S /Q /F *.vcxproj
del /S /Q /F *.androidproj
del /S /Q /F *.vcxproj.filters
del /S /Q /F *.androidproj.user
FOR /d /r . %%d IN (.vs) DO @IF EXIST "%%d" rd /s /q "%%d"
pause