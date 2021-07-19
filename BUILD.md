this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# Building CPUinfoAndControl

## General

This affects all builds/targets.

* [CMake](https://cmake.org/download) for creating makefile / IDE project files
* C++ compiler (source code is able to build with "g++" 7.5.0)
* ["common_sourcecode"](https://www.github.com/st-gb/common_sourcecode) git
repository from Stefan Gebauer

  Place this into the same directory level as this repository for ease of use.:

  ```
           top-level directory
                   / \
  CPUinfoAndControl   common_sourcecode
  ```

###  Linux

- [Debian}(http://en.wikipedia.org/wiki/Debian)-/[APT](
http://en.wikipedia.org/wiki/APT_(software))-based

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

#### Linux

The "xerces" package (for [Apache Xerces](
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

### wxWidgets GUI

- [wxWidgets](http://www.wxwidgets.org/downloads) (tested with version 3)

  needed [libraries](http://docs.wxwidgets.org/3.0/page_libs.html):
  - [base](http://docs.wxwidgets.org/3.0/page_libs.html#page_libs_wxbase)
    (For mandatory classes)
  - [core](http://docs.wxwidgets.org/3.0/page_libs.html#page_libs_wxcore)
     ("Basic GUI classes")
  - [adv](http://docs.wxwidgets.org/3.0/page_libs.html#page_libs_wxadv)
    (For class  [wxTaskBarIcon](
    http://docs.wxwidgets.org/3.0/classwx_task_bar_icon.html) (etc.) )

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

  wxgtk2:for "wx-config", "wx/setup.h", gtk2u_adv, gtk2u_core .so files;
   "findWixwidgets.cmake", although not shown with command "pacman -Ql wxgtk2"
  wxgtk-common: header files, "base" .so files--obtained via command
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
