::  ----------------------------------------------------------
set ENGINE_PATH=C:\Users\Jens\Google Drive\Brickade\PRO\Engine
::  ----------------------------------------------------------

@echo off
cls
echo ----------------------------------------------------------
set /p folder=Engine Folder: Brickade\PRO\Engine\
if "%folder%"=="" exit
echo ----------------------------------------------------------

rd ..\src\PuReEngine /q /s
rd ..\bin\Win32\modules /q /s
rd ..\bin\x64\modules /q /s
echo deleted old shit
echo ---------------------------------

xcopy "%ENGINE_PATH%\%folder%\bin" ..\src\PuReEngine\bin\ /q /y /e
xcopy "%ENGINE_PATH%\%folder%\include" ..\src\PuReEngine\include\ /q /y /e

xcopy "%ENGINE_PATH%\%folder%" ..\bin\Win32\modules\ /q /y
xcopy "%ENGINE_PATH%\%folder%" ..\bin\x64\modules\ /q /y
echo copied new stuff
echo ---------------------------------

::pause