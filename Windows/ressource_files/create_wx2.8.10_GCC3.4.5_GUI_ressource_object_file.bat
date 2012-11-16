
REM CALL: "wx_setup_h_dir" "wx_include_dir"
@echo 1st arg: %1
@echo 2nd arg: %2
@echo 3rd arg: %3

set OLD_PATH=%PATH%
set PATH=C:\MinGW_gcc.3.4.5\bin;%PATH%
@REM use "CMD /C" else after executing the batch file nothing more is executed
CMD /C create_GUI_ressource_object_file.bat %WX2.8.10_MINGW_GCC3.4.5_UNICODE_RELEASE_STATIC_SETUP_H_PATH% "C:\Libraries\wxWidgets-2.8.10\include"
echo after
set PATH=%OLD_PATH%