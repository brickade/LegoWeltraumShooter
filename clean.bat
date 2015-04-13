@echo off
cls
Color 1F
echo *******************************************************************************
echo *                               Clean up                                      *
echo *******************************************************************************

echo Cleaning up ...
echo.
rd  src\LegoWeltraumShooter\LegoWeltraumShooter\intermediate /Q /S
del bin\Win32\*.ilk
del bin\Win32\*.pdb
del bin\x64\*.ilk
del bin\x64\*.pdb