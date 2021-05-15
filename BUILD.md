this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# Building CPUinfoAndControl

## General

This affects all builds/targets.

### Invocation

1. Change to _repository_root_" directory where 
[CMakeLists.txt](https://cmake.org/cmake/help/latest/guide/tutorial/index.html#a-basic-starting-point-step-1)
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

The "libxerces" package (for Apache Xerces) is needed.

- Debian-/APT-based

  Command line to install it:

  ```
  apt-get install libxerces
  ```
  
- Arch Linux

  Command line to install it (as root/via sudo):

  ```
  pacman -S xerces-c
  ```

### wxWidgets GUI

#### Linux

- command line to install [wxWidgets](https://www.wxwidgets.org/) runtime
  libraries:

  - Debian Linux-/APT-based:

    ```
    apt-get install libwxbase3.0-0v5
    apt-get install libwxgtk3.0-0v5
    ```

    ([...]gtk[...] is "adv" library for Linux/GTK)

  - Arch Linux

    Command line to install it (tested with ARMv7 32 bit):

    ```
    pacman -S wxgtk-common
    pacman -S wxgtk2
    ```

    wxgtk2: for findWixwidgets.cmake, although not shown with "pacman -Ql wxgtk2" 
    wxgtk-common: header files, wxbase .so files

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

[Stefan Gebauer, M.Sc. Comp. Sc.](https://github.com/st-gb)

## License

Copyright by Stefan Gebauer, no commercial use without grant
