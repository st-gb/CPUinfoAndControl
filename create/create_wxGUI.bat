@echo off
REM e.g. call this program with 2 arguments: /usr/include/wx-3.0 /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0

set NUMBER_OF_SHELL_ARGS=0
for %%x in (%*) do Set /A NUMBER_OF_SHELL_ARGS+=1
echo number of batch file arguments: %NUMBER_OF_SHELL_ARGS%

REM Edit the following variables if needed!
REM If using a non-absolute path when setting the C Compiler causes this: http://public.kitware.com/Bug/view.php?id=13756
REM So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294 proposes
REM C_COMPILER=/usr/bin/gcc
set C_COMPILER=C:/TDM-GCC-64/bin/gcc.exe
REM CPP_COMPILER=/usr/bin/g++
set CPP_COMPILER=C:/TDM-GCC-64/bin/g++.exe

REM change if necessary
set NUM_ARGS_NEEDED=4
set MINIMAL_NUM_ARGS_NEEDED=3

REM IDEA: pass as args: <NAME=VALUE>, e.g. COMMON_SOURCECODE_ROOT_PATH=../common_sourcecode
if %NUMBER_OF_SHELL_ARGS% LSS %MINIMAL_NUM_ARGS_NEEDED% (
  echo error: at least %MINIMAL_NUM_ARGS_NEEDED% command line arguments are needed:
  echo "wxWidgets include dir" "wxWidgets setup.h dir" "STLSOFT include dir"
  GOTO END
)

set WXWIDGETS_INCLUDE_DIR=%1
set WXWIDGETS_SETUP_H_PATH=%2
echo WXWIDGETS_INCLUDE_DIR: %WXWIDGETS_INCLUDE_DIR%
echo WXWIDGETS_SETUP_H_PATH: %WXWIDGETS_SETUP_H_PATH%
set PLATFORM_STL_ROOT_PATH=%3
echo PLATFORM_STL_ROOT_PATH: %PLATFORM_STL_ROOT_PATH%

REM if [ NUMBER_OF_SHELL_ARGS LSS $NUM_ARGS_NEEDED ]; then
REM   COMMON_SOURCECODE_ROOT_PATH=$4
REM  echo COMMON_SOURCECODE_ROOT_PATH: $COMMON_SOURCECODE_ROOT_PATH
REM fi

REM "-pg" option for "gprof" profiling

REM from http://cmake.3232098.n2.nabble.com/Can-t-find-wxWidgets-td7591015.html :
REM  -DwxWidgets_CONFIG_EXECUTABLE:FILEPATH=/home/mzx_bldr/mozaix_svn/engineering/c 
REM  > ommon_src/src/wxWidgets-3.0.2/buildgtk/wx-config
REM -DWITH_WXPATH
cmake -G "MSYS Makefiles" ^
-DCMAKE_BUILD_TYPE=Debug ^
-DCMAKE_C_COMPILER=%C_COMPILER% ^
-DwxWidgets_ROOT_DIR:PATH="C:\wxWidgets-2.8.10/" ^
-DCMAKE_CXX_COMPILER=%CPP_COMPILER% ^
-DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" ^
-DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2" ^
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON . ^
-DEXE_TYPE:STRING=wxGUI ^
-DWXWIDGETS_INCLUDE_DIR:STRING=%WXWIDGETS_INCLUDE_DIR% ^
-DWXWIDGETS_SETUP_H_PATH:STRING=%WXWIDGETS_SETUP_H_PATH% ^
-DPLATFORM_STL_ROOT_PATH:STRING=%PLATFORM_STL_ROOT_PATH% ^
-DCOMMON_SOURCECODE_ROOT_PATH:STRING=%COMMON_SOURCECODE_ROOT_PATH%
REM else
REM  echo "ERROR:at least" $MINIMAL_NUM_ARGS_NEEDED " args needed:"
REM  echo "specifiy options for this script:<WXWIDGETS_INCLUDE_DIR> <WXWIDGETS_SETUP_H_PATH> <PLATFORMSTL_ROOT_PATH> <COMMON_SOURCECODE_ROOT_PATH>"
REM  echo "e.g. for Ubuntu Linux 32 bit:"$0 /usr/include/wx-3.0 /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /usr/include/stlsoft-1.9.124/include ../common_sourcecode 
REM  echo "e.g. for 64 bit Linux Mint:"/usr/include/wx-3.0 /usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /usr/include/stlsoft-1.9.124/include ../common_sourcecode 
REM fi
:END