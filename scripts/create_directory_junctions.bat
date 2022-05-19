@echo off
cd ..
mklink /J %LOCALAPPDATA%\Steff\Idevilion\data .\data
mklink /J %LOCALAPPDATA%\Steff\Idevilion\data-server .\data-server
mklink /J %LOCALAPPDATA%\Steff\Idevilion\data-dev .\data-dev
pause