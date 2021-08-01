this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# CPU core voltage and frequency setting application

A tool mainly to do [dynamic voltage and frequency scaling](
http://en.wikipedia.org/wiki/Dynamic_frequency_scaling) on its own via dynamic
libraries that are often specific to a CPU model (and maybe stepping).
Also other data like CPU core temperature or CPU core usage may be shown.
The settings (interval, which dynamic library for which CPU model etc.) can be
specified in configuration files.

![here the screenshot of wxWidgets GUI in Linux Mint should appear](
http://it-gebauer.de/CPUinfoAndCtl/CPUinfoAndCtl_i5-8250U_wxGUI_main_dialog_LinuxMint.png
"screenshot of wxWidgets GUI's main dialog with Intel i5-8250U in Linux Mint")

## Building

see [BUILD.md](BUILD.md)

## Execution

### [Microsoft Windows](http://en.wikipedia.org/wiki/Microsoft_Windows)

#### Execution Account/Rights

If you want or need (for the [service](
http://en.wikipedia.org/wiki/Windows_service)) to get the CPU data directly from
the hardware then you need to execute the application with [administrative
rights/as administrator](
http://en.wikipedia.org/wiki/Superuser#Microsoft_Windows).

#### Execution Environment

In order to not need to place the neccessary [WinRing0](
http://openlibsys.org/manual/WhatIsWinRing0.html) (and optionally the
[wxWidgets](http://en.wikipedia.org/wiki/WxWidgets) and [Apache Xerces](
http://xerces.apache.org/xerces-c/)) [dynamic libraries](
http://en.wikipedia.org/wiki/Dynamic_loading#In_C/C++) you can extend the
[PATH](http://en.wikipedia.org/wiki/PATH_\(variable\)#DOS,_OS/2,_and_Windows)
[environment variable](
http://en.wikipedia.org/wiki/Environment_variable#Assignment:_DOS,_OS/2_and_Windows)
before calling the [executable](https://en.wikipedia.org/wiki/Executable)
([service](http://en.wikipedia.org/wiki/Windows_service) or [GUI](
http://en.wikipedia.org/wiki/Graphical_user_interface)).

example:

```
PATH=%PATH%;E:\Libs\WinRing0_1_3_1b\release;E:\wxWidgets\MSW-3.1.0_gcc510TDM_x64_Dev\lib\gcc_dll
CPUinfoAndControl_wxGUI_dbg.exe
```

## Limitation of Liability

You execute the software at your own risk. The manufacturer is not liable for
any harm the software application may cause in any case.

These may especially/for instance be:

- a too low voltage so the CPU malfunctions or computer hangs up/crashes
- a too high voltage so the CPU gets broken
- wrong representation of data in User Interface
- writing log files to disk may cause a worsening of a data carrier's health
state
- any other misbehaviour

## Authors

[Stefan Gebauer, M. Sc. Comp. Sc. (TU Berlin)](https://github.com/st-gb)

## License

Copyright by Stefan Gebauer, no commercial use without grant.
If you want to use the code in a commercial product then contact the author.

The source code is also provided to enable customization of the software.

You may use the code free of charge

- to study the implementation and quality (of one of the authour's software
  products) so bugs can be reported.
- to build targets (wxWidgets GUI, Linux service [...] ) of the software in
  order to test it, especially if a binary is not provided for another
  platform (CPU, Operating System)
- in a non-commercial product if it is not in competition with an author's
  software product (repositories from http://github.com/st-gb).
  
  If in doubt which applications this affects then ask the author.

It is appreciated to reference the author (e.g. in the "about" dialog or
README.txt) if using this code. Therefore please refer to this github user page
and/or to "it-gebauer.de".
