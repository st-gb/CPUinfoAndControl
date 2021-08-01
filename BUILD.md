this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# Building CPUinfoAndControl

## General

This affects all builds/targets ([wxWidgets](http://wxwidgets.org/about/) [GUI](
http://en.wikipedia.org/wiki/Graphical_user_interface),
[Microsoft Windows service](http://en.wikipedia.org/wiki/Windows_service) &
Linux [daemon](http://en.wikipedia.org/wiki/Daemon_\(computing\)),
[dynamic libraries](http://en.wikipedia.org/wiki/Dynamic_loading#In_C/C++)).

In order to build you will need:
* [CMake](https://cmake.org/download) for easily creating [Makefile](
  http://en.wikipedia.org/wiki/Make_\(software\)#Makefile) / [IDE](
  http://en.wikipedia.org/wiki/Integrated_development_environment) project files
* [C++ (and C) compiler and linker](
  http://en.wikipedia.org/wiki/List_of_compilers#C++_compilers)
* "[common_sourcecode](http://www.github.com/st-gb/common_sourcecode)" git
repository from Stefan Gebauer

  Place this into the same directory level as this repository for ease of use.:

  ```
           top-level directory
                   / \
  CPUinfoAndControl   common_sourcecode
  ```

###  Linux

- [Debian](http://en.wikipedia.org/wiki/Debian)-/[APT](
http://en.wikipedia.org/wiki/APT_\(software\))-based

  command line to install C++ compiler and CMake (as root):

  ```
  apt-get install g++
  apt-get install cmake
  ```

- [Arch Linux](http://en.wikipedia.org/wiki/Arch_Linux)-/[pacman](
https://en.wikipedia.org/wiki/Arch_Linux#Pacman)-based

  command line to install C++ compiler and CMake (as root):

  ```
  pacman -S gcc
  pacman -S cmake
  ```

### [Compilers](http://en.wikipedia.org/wiki/Compiler)

The [wxWidgets](https://en.wikipedia.org/wiki/WxWidgets)
[_G_raphical _U_ser _I_nterface](
http://en.wikipedia.org/wiki/Graphical_user_interface) (and [service](
http://en.wikipedia.org/wiki/Windows_service) ?) have been successfully built
with [MinGW](http://en.wikipedia.org/wiki/MinGW)/ 64 bit [TDM-GCC](
http://en.wikipedia.org/wiki/TDM-GCC) version 5.1.0 compiler suite )
under 64 bit [Microsoft Windows 7](http://en.wikipedia.org/wiki/Windows_7)
and 64 bit "[g++](http://en.wikipedia.org/wiki/GNU_Compiler_Collection)" 7.5.0
under 64 bit [Linux Mint](http://en.wikipedia.org/wiki/Linux_Mint) 19 (with
"[Cinnamon](http://en.wikipedia.org/wiki/Cinnamon_\(desktop_environment\))"
desktop).\
So these are the compilers of choice and you should stick to these "compiler and
version" pairs/tuples if you encounter build problems.\
Building with other compilers may not work, especially if they are too far away
from the [GCC](http://en.wikipedia.org/wiki/GNU_Compiler_Collection) standard.

**Use the [same compiler type and compiler version](
http://en.wikipedia.org/wiki/Name_mangling#Real-world_effects_of_C++_name_mangling)
(->same "[name mangling](
http://en.wikipedia.org/wiki/Name_mangling#How_different_compilers_mangle_the_same_functions)")
for _each_ _directly_/[_statically_](
http://en.wikipedia.org/wiki/Static_library), that means: probably also
[dynamic library import files](
http://en.wikipedia.org/wiki/Dynamic-link_library#Import_libraries) linked
software component (Apache Xerces library, wxWidgets library, GUI etc.) to
enable linking them _successfully_ together.**

### [Microsoft Windows](http://en.wikipedia.org/wiki/Microsoft_Windows)

#### WinRing0 for service (and GUI)

As this application uses the [CPUID](http://en.wikipedia.org/wiki/CPUID)
instruction for determining the [CPU manufacturer](
http://en.wikipedia.org/wiki/CPUID#EAX=0:_Highest_Function_Parameter_and_Manufacturer_ID),
[family, model and stepping](
http://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits) in
order to load the appropriate "CPU info & control" [dynamic library](
http://en.wikipedia.org/wiki/Dynamic_loading#In_C/C++), often uses
the "[rdmsr](http://en.wikipedia.org/wiki/Model-specific_register#Using_MSRs)" /
"[wrmsr](http://en.wikipedia.org/wiki/Model-specific_register#Using_MSRs)\"
(MSR=[_M_odel-_S_pecific _R_egister](
http://en.wikipedia.org/wiki/Model-specific_register)) and "[rdtsc](
http://en.wikipedia.org/wiki/Time_Stamp_Counter)" instructions to set/get the
current voltage/[voltage ID](
http://en.wikipedia.org/wiki/Voltage_regulator_module#Voltage_identification)
and [CPU core multiplier](
http://en.wikipedia.org/wiki/CPU_multiplier)/[CPU core clock rate/frequency](
http://en.wikipedia.org/wiki/Clock_rate) and may need access to the [PCI
configuration space](http://en.wikipedia.org/wiki/PCI_configuration_space) this
application relies on the [WinRing0](
http://openlibsys.org/manual/WhatIsWinRing0.html) [software library](
http://en.wikipedia.org/wiki/Library_\(computing\)) for elevated/[privileged](
http://en.wikipedia.org/wiki/Privilege_\(computing\))/direct ("[Ring 0](
http://en.wikipedia.org/wiki/Protection_ring)") access to the hardware you need
to download it.

This software works (is tested) with [WinRing0](
http://openlibsys.org/manual/WhatIsWinRing0.html) **version 1.3.1b** ("file
version" and "product version" from [Windows Explorer](
http://en.wikipedia.org/wiki/File_Explorer)'s "Details" tab: 1.3.1.19).\
Later versions of WinRing0 may not work because the write access (to [CPU]
[registers](http://en.wikipedia.org/wiki/Processor_register)/to
[_M_odel-_S_pecific _R_egister](
http://en.wikipedia.org/wiki/Model-specific_register) etc.) was removed there
according to the author's memories.\
As there is no more official [release](
http://en.wikipedia.org/wiki/Software_release_life_cycle) of this
[software library](http://en.wikipedia.org/wiki/Library_\(computing\)) you have
find one on your own.

#### [wxWidgets](http://en.wikipedia.org/wiki/WxWidgets) 3.x for GUI

For building you need the wxWidgets header files, for example from the
[wxWidgets source code, here version 3.1.5](
http://github.com/wxWidgets/wxWidgets/archive/refs/tags/v3.1.5.zip) and the
corresponding [static](http://en.wikipedia.org/wiki/Static_library) or
[dynamic](http://en.wikipedia.org/wiki/Dynamic_loading#In_C/C++) libraries to
link to.\
You can get pre-built libraries for compilers:
- [Microsoft Visual C++](http://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B)
- [MinGW](http://en.wikipedia.org/wiki/MinGW)
- [TDM-GCC](http://en.wikipedia.org/wiki/TDM-GCC)
- (etc.)

See [here regarding version 3.15](
http://github.com/wxWidgets/wxWidgets/releases/tag/v3.1.5).\
If these do not exist for the compiler version of your choice then you can build
them manually from the wxWidgets source code.

#### [Apache Xerces](http://xerces.apache.org/xerces-c/) 3.x for GUI and service

- if building with [Cygwin](http://en.wikipedia.org/wiki/Cygwin) then download
 [Cygwin binaries as proposed here](
 http://xerces.apache.org/xerces-c/install-3.html#Cygwin) (paragraph
 "*Binary distribution*")
- if building with [Microsoft Visual Studio](
 http://en.wikipedia.org/wiki/Microsoft_Visual_Studio) then you may download
 [pre-built Microsoft Visual Studio files](
 http://archive.apache.org/dist/xerces/c/3/binaries/) for building the GUI/
 service easier/faster
- Otherwise Build the [static](http://en.wikipedia.org/wiki/Static_library)
 and/or [dynamic](http://en.wikipedia.org/wiki/Dynamic_loading#In_C/C++)
 library [manually](http://xerces.apache.org/xerces-c/build-3.html)

### Invocation

1. Change to _repository_root_ directory where [CMakeLists.txt](
https://cmake.org/cmake/help/latest/guide/tutorial/index.html#a-basic-starting-point-step-1)
resides.

2. either

    * call [CMake](https://cmake.org) directly:

      command line:

      ```
      cmake -G _build_system_generator_ _[...]_
      ```

      For available build system generators see output of command line:

      ```
      cmake -G
      ```

    * call the build scripts in _repository_root_/create" directory

#### Specify build type

Pass "-DCMAKE\_BUILD\_TYPE=_type_" to [CMake](https://cmake.org) command line.

The
[build type](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)
can be 1 of

* Debug
* Release
* RelWithDebInfo
* MinSizeRel

## Build Examples

### Linux GUI and [daemon](http://en.wikipedia.org/wiki/Daemon_\(computing\))

The "xerces" package for [Apache Xerces](
http://en.wikipedia.org/wiki/Apache_Xerces) is needed.

- Debian-/APT-based

  Command line to install it (as root/via sudo):

  ```
  apt-get install libxerces
  apt-get install xerces-c
  ```

- Arch Linux-/pacman-based

  Command line to install it (as root/via sudo):

  ```
  pacman -S xerces-c
  ```

  (package "xerces-c" contains class header files, dynamic libraries and
  examples--obtained via command "pacman -Ql xerces-c")

### [wxWidgets](http://www.wxwidgets.org/downloads) GUI

- tested with wxWidgets version 3.x

- needed [libraries](http://docs.wxwidgets.org/3.0/page_libs.html):
  - [base](http://docs.wxwidgets.org/3.0/page_libs.html#page_libs_wxbase)
    (For mandatory classes)
  - [core](http://docs.wxwidgets.org/3.0/page_libs.html#page_libs_wxcore)
     ("Basic GUI classes")
  - [adv](http://docs.wxwidgets.org/3.0/page_libs.html#page_libs_wxadv)
    (For class  [wxTaskBarIcon](
    http://docs.wxwidgets.org/3.0/classwx_task_bar_icon.html) (etc.) )

#### [Microsoft Windows](http://en.wikipedia.org/wiki/Microsoft_Windows)

##### TDM-GCC, [MinGW](http://en.wikipedia.org/wiki/MinGW)

Call [_repository root file system directory_
\\create\\wxGUI\\Windows\\create_wxGUI_with_MinGW.bat](
http://github.com/st-gb/CPUinfoAndControl/blob/master/create/wxGUI/Windows/create_wxGUI_with_MinGW.bat)
from _repository root file system directory_ folder.\
You may need to pass file system paths with path separator as slashes ("/")
rather than backslashes ("\\") because the underlying build scripts are
[sh](http://en.wikipedia.org/wiki/Unix_shell#Bourne_shell)/[bash](
http://en.wikipedia.org/wiki/Bash_\(Unix_shell\))
[shell scripts](http://en.wikipedia.org/wiki/Shell_script).

This .bat file in turn/internally calls the more general file
[_repository root file system directory_
\\create\\wxGUI\\Windows\\create_wxGUI.bat](
http://github.com/st-gb/CPUinfoAndControl/blob/master/create/wxGUI/Windows/create_wxGUI.bat)
. You may view this file to see its parameters, but keep in mind that the
parameters there are shifted by 1 command line argument because the 1st
parameter is given by the [direct batch file](
http://github.com/st-gb/CPUinfoAndControl/blob/master/create/wxGUI/Windows/create_wxGUI_with_MinGW.bat).

An [example to call the [...]MinGW.bat file](
http://github.com/st-gb/CPUinfoAndControl/blob/master/create/wxGUI/Windows/build_with_64bitMinGW_TDM_720QM.bat).
For comfort you may adapt this file so you don't need to write every parameter
on the command line.

Therefore pass as [command line arguments](
http://en.wikipedia.org/wiki/Command-line_interface#Arguments) to the
[.bat](https://en.wikipedia.org/wiki/Batch_file) [file mentioned above](
http://github.com/st-gb/CPUinfoAndControl/blob/master/create/wxGUI/Windows/create_wxGUI_with_MinGW.bat)
(in this order):
1. "include" folder from [wxWidgets source code, here version 3.1.5](
 http://github.com/wxWidgets/wxWidgets/archive/refs/tags/v3.1.5.zip)\
 , for example "E:/wxWidgets/3.1.0/include"
2. wxWidgets binary directory where the file system directory "lib\\gcc_dll"
 containing the [static libraries](http://en.wikipedia.org/wiki/Static_library)
 ("\*.a" for MinGW) or [dynamic library import files](
 http://en.wikipedia.org/wiki/Dynamic-link_library#Import_libraries) ("\*.dll.a"
 for MinGW ?) resides\
 , for example "E:/wxWidgets/MSW-3.1.0_gcc510TDM_x64_Dev"
3. [CMake build type](Specify-build-type)\
 , for example "Release" or "Debug"
4. file system path to the [C compiler](
 http://en.wikipedia.org/wiki/List_of_compilers#C_compilers) [executable](
 https://en.wikipedia.org/wiki/Executable)\
 , for example "E:/compilers/TDM-GCC-64/bin/gcc.exe"
5. file system path to the [C**++** compiler](
 http://en.wikipedia.org/wiki/List_of_compilers#C++_compilers) [executable](
 https://en.wikipedia.org/wiki/Executable)\
 , for example "E:/compilers/TDM-GCC-64/bin/g++.exe"
6. file system path to the [Apache Xerces version 3 C++ source code files](
 http://downloads.apache.org/xerces/c/3/sources/)' "src" folder directly top of
 file system directory "xercesc", that means
 _>>Apache Xerces root<< file system directory_/src\
 Not needed e.g. if installed via "make install" in [MinGW](
 http://en.wikipedia.org/wiki/MinGW).\
 , for example "E:/Libs\Apache_Xerces/xerces-c-3.2.2/src"
7. file system path top of different possible WinRing0 versions with folder
 name "WinRing0_1_3_1b" etc.
 , for example "E:/Libs" when there exists a subdirectory
 "E:\Libs\WinRing0_1_3_1b" with "release" and "source" file system folders
 inside
8. file system path to the Apache Xerces [static](
 http://en.wikipedia.org/wiki/Static_library)/[dynamic](
 http://en.wikipedia.org/wiki/Dynamic_loading#In_C/C++) library\
 , for example "E:/Libs/Apache_Xerces/xerces-c-3.2.2/src/.libs/GCC5.1TDM64bit"
 when there is a "libxerces-c.a" (for [MinGW](
 http://en.wikipedia.org/wiki/MinGW)/[TDM-GCC](
 http://en.wikipedia.org/wiki/TDM-GCC)) inside this file system
 directory
9. number of concurrent/parallel CPU cores for compiling\
 , for example "4"

#### Linux

- Debian Linux-/APT-based:

  command line to install [wxWidgets](https://www.wxwidgets.org/) runtime
  libraries (wxWidgets version 3, as root):

  ```
  apt-get install libwxbase3.0-0v5
  apt-get install libwxgtk3.0-0v5
  ```

  ([...]gtk[...] is "adv" library for Linux/GTK)

- Arch Linux-/"pacman"-based:

  Command line to install it (as root, tested with ARMv7 32 bit):

  ```
  pacman -S wxgtk-common
  pacman -S wxgtk2
  ```

  **wxgtk2**: for "wx-config", "wx/setup.h", gtk2u_adv, gtk2u_core .so files;
   "findWixwidgets.cmake", although not shown with command "pacman -Ql wxgtk2"\
  **wxgtk-common**: header files, "base" .so files--obtained via command
   "pacman -Ql wxgtk-common"

- command line to build (assuming working directory is _repository_root_):

  ```
  ../create/wxGUI/Linux/create_wxGUI_Linux_debug.sh "Unix Makefiles"
  make -j `nproc`
  ```

### Makefiles

if the CMake build system generator created a Makefile:

  command line (expects file "Makefile" in current working directory and builds
  there):

  ```
  make -j _number of max. concurrent jobs_
  ```

  argument right of "-j" should be <= _number of CPU cores_

## Authors

[Stefan Gebauer, M. Sc. Comp. Sc. (TU Berlin)](https://github.com/st-gb)

## License

Copyright by Stefan Gebauer, no commercial use without grant
