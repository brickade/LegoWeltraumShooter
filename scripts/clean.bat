@echo off
cls
echo *******************************************************************************
echo *                               Clean up                                      *
echo *******************************************************************************
echo .
rd  ..\src\ContentEditor\intermediate /Q /S
rd  ..\src\Onager\Onager\TEMP /Q /S
rd  ..\src\SpaceBrickArena\intermediate /Q /S
rd  ..\src\TheBrick\intermediate /Q /S
del ..\bin\Win32\*.ilk
del ..\bin\Win32\*.pdb
del ..\bin\x64\*.ilk
del ..\bin\x64\*.pdb
echo .
timeout /t 2