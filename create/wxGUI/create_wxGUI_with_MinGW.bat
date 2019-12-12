@echo off
echo "this batch file:" %0
set numMakeCPUcores=%9
REM from https://stackoverflow.com/questions/17063947/get-current-batchfile-directory
set batchFileDir=%~dp0
echo %batchFileDir%
REM https://cmake.org/cmake/help/v3.1/generator/MinGW%20Makefiles.html
REM "Generates makefiles for use with mingw32-make under a Windows command prompt.
REM Use this generator under a Windows command prompt with MinGW in the PATH 
REM and using mingw32-make as the build tool. The generated makefiles use
REM cmd.exe as the shell to launch build rules. They are not compatible with
REM MSYS or a unix shell."
REM "%*" : all arguments from this script
cmd /C %batchFileDir%create_wxGUI.bat "MinGW Makefiles" %*
REM https://stackoverflow.com/questions/48092812/warning-virtual-void-wxwindowbasesetinitialbasesizeconst-wxsize-is-depre
REM many error messages may arise like:
REM "warning: 'virtual void wxWindowBase::SetInitialBaseSize(const wxSize&)' is deprecated"
@echo on
mingw32-make -j%numMakeCPUcores%