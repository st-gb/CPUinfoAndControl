@echo off
REM from https://stackoverflow.com/questions/17063947/get-current-batchfile-directory
set batchFileDir=%~dp0
echo "this file's dir:" %batchFileDir%
%batchFileDir%create_wxGUI_with_MinGW.bat ^
E:/wxWidgets/3.1.0/include ^
E:/wxWidgets/MSW-3.1.0_gcc510TDM_x64_Dev ^
Release ^
E:/compilers/TDM-GCC-64/bin/gcc.exe ^
E:/compilers/TDM-GCC-64/bin/g++.exe ^
E:/Libs\Apache_Xerces/xerces-c-3.2.2/src ^
E:/Libs ^
E:/Libs\Apache_Xerces/xerces-c-3.2.2/src/.libs/GCC5.1TDM64bit ^
4