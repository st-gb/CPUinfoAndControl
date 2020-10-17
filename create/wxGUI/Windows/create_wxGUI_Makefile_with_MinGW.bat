@echo off
REM from https://stackoverflow.com/questions/17063947/get-current-batchfile-directory
set batchFileDir=%~dp0
REM %batchFileDir%/create_wxGUI.bat "MinGW Makefiles" %1 %2 %3 %4 %5
REM "%*" : all arguments from this script
%batchFileDir%/create_wxGUI.bat "MinGW Makefiles" %*