@echo off
REM e.g. call this program with arguments: "MinGW Makefiles"
REM  E:\wxWidgets\3.1.0\include E:\wxWidgets\MSW-3.1.0_gcc510TDM_x64_Dev
REM  E:/compilers/TDM-GCC-64/bin/gcc.exe E:/compilers/TDM-GCC-64/bin/g++.exe

set NUMBER_OF_SHELL_ARGS=0
for %%x in (%*) do Set /A NUMBER_OF_SHELL_ARGS+=1
echo All batch file arguments to this script (%0):
for %%x in (%*) do ECHO %%x 
echo number of batch file arguments: %NUMBER_OF_SHELL_ARGS%

REM change if necessary
set NUM_ARGS_NEEDED=4
set MINIMAL_NUM_ARGS_NEEDED=3

REM IDEA: pass as args: <NAME=VALUE>, e.g. COMMON_SOURCECODE_ROOT_PATH=../common_sourcecode
if %NUMBER_OF_SHELL_ARGS% LSS %MINIMAL_NUM_ARGS_NEEDED% (
  echo error: at least %MINIMAL_NUM_ARGS_NEEDED% command line arguments are needed:
  echo "CMake Build System Generator" "wxWidgets include dir"
  echo "wxWidgets setup.h dir" "STLSOFT include dir"
  GOTO END
)

set BuildSystemGenerator=%1
echo BuildSystemGenerator: %BuildSystemGenerator%
set WXWIDGETS_INCLUDE_DIR=%2
REM Root path of wxWidgets libraries, i.e. ".dll" ".lib" (MSVC) and ".a"
REM  (e.g. MinGW) files top of "gcc/lib"
set WXWIDGETS_LIBS_ROOT_PATH=%3
echo WXWIDGETS_INCLUDE_DIR: %WXWIDGETS_INCLUDE_DIR%
REM echo WXWIDGETS_SETUP_H_PATH: %WXWIDGETS_SETUP_H_PATH%
REM set PLATFORM_STL_ROOT_PATH=%4
set CMakeBuildType=%4
REM echo PLATFORM_STL_ROOT_PATH: %PLATFORM_STL_ROOT_PATH%

REM Edit the following variables if needed!
REM If using a non-absolute path when setting the C Compiler causes this: http://public.kitware.com/Bug/view.php?id=13756
REM So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294 proposes
REM C_COMPILER=/usr/bin/gcc
set C_COMPILER=%5
echo C_COMPILER: %C_COMPILER%

REM CPP_COMPILER=/usr/bin/g++
set CPP_COMPILER=%6
echo CPP_COMPILER: %CPP_COMPILER%

REM path top of "xercesc", e.g. "xerces-c-3.2.2\src".
REM Not needed if e.g. in installed via "make install" in MinGW.
set ApacheXercesInclPath=%7

REM e.g. E:\Libs\WinRing0_1_3_1b\source\dll
set WinRing0InclPath=%8
set ApacheXercesLibPath=%9

REM if [ NUMBER_OF_SHELL_ARGS LSS $NUM_ARGS_NEEDED ]; then
REM   COMMON_SOURCECODE_ROOT_PATH=$4
REM  echo COMMON_SOURCECODE_ROOT_PATH: $COMMON_SOURCECODE_ROOT_PATH
REM fi

REM "-pg" option for "gprof" profiling

REM https://stackoverflow.com/questions/38864489/how-to-trace-cmakelists-txt

REM from http://cmake.3232098.n2.nabble.com/Can-t-find-wxWidgets-td7591015.html :
REM  -DwxWidgets_CONFIG_EXECUTABLE:FILEPATH=/home/mzx_bldr/mozaix_svn/engineering/c 
REM  > ommon_src/src/wxWidgets-3.0.2/buildgtk/wx-config
REM -DWITH_WXPATH

echo calling cmake
REM Building with MSYS Makefiles worked (except wxWidgets).
REM -Wdev --debug-output --trace
REM cmake -Wno-dev -G "MSYS Makefiles" ^
REM -DwxWidgets_ROOT_DIR:PATH="E:/wxWidgets/3.1.0" ^
REM -DPLATFORM_STL_ROOT_PATH:STRING=%PLATFORM_STL_ROOT_PATH% ^
REM -DCMAKE_SH="CMAKE_SH-NOTFOUND" ^
cmake -Wno-dev ^
-G %BuildSystemGenerator% ^
-DCMAKE_BUILD_TYPE=%CMakeBuildType% ^
-DCMAKE_C_COMPILER=%C_COMPILER% ^
-DCMAKE_CXX_COMPILER=%CPP_COMPILER% ^
-DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" ^
-DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2" ^
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON . ^
-DEXE_TYPE:STRING=wxGUI ^
-DWXWIDGETS_INCLUDE_DIR:STRING=%WXWIDGETS_INCLUDE_DIR% ^
-DWXWIDGETS_SETUP_H_PATH:STRING=%WXWIDGETS_LIBS_ROOT_PATH% ^
-DApacheXercesInclPath:STRING=%ApacheXercesInclPath% ^
-DWinRing0InclPath:STRING=%WinRing0InclPath% ^
-DApacheXercesLibPath=%ApacheXercesLibPath%
-DCOMMON_SOURCECODE_ROOT_PATH:STRING=%COMMON_SOURCECODE_ROOT_PATH%
echo "after calling cmake"
REM else
REM  echo "ERROR:at least" $MINIMAL_NUM_ARGS_NEEDED " args needed:"
REM  echo "specifiy options for this script:<WXWIDGETS_INCLUDE_DIR> <WXWIDGETS_SETUP_H_PATH> <PLATFORMSTL_ROOT_PATH> <COMMON_SOURCECODE_ROOT_PATH>"
REM  echo "e.g. for Ubuntu Linux 32 bit:"$0 /usr/include/wx-3.0 /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /usr/include/stlsoft-1.9.124/include ../common_sourcecode 
REM  echo "e.g. for 64 bit Linux Mint:"/usr/include/wx-3.0 /usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /usr/include/stlsoft-1.9.124/include ../common_sourcecode 
REM fi
:END