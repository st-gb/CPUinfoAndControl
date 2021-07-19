this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# CPU core voltage and frequency setting application 

A tool to do dynamic voltage and frequency scaling on its own via dynamic (link)
libraries that are often specific to a CPU model (and maybe stepping).
The settings (interval, which dynamic library for which CPU model etc.) can be
specified in configuration files.

![here the screenshot of wxWidgets GUI in Linux Mint should appear](http://it-gebauer.de/CPUinfoAndCtl/CPUinfoAndCtl_i5-8250U_wxGUI_main_dialog_LinuxMint.png "screenshot of wxWidgets GUI's main dialog in Linux Mint")

## Building

see [BUILD.md](BUILD.md)

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
