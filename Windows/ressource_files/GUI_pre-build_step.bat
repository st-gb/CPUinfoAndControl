echo updating build time string
touch -m %COMMON_SOURCECODE_ROOT_PATH%\preprocessor_macros/BuildTimeString.h
set setup_h_path=%WX2.8.10_MINGW_GCC3.4.5_UNICODE_RELEASE_STATIC_SETUP_H_PATH%
set create_GUI_ressource_object_file_bat_path="%X86IANDC_ROOT_SOURCE_PATH%\Windows\ressource_files\create_GUI_ressource_object_file.bat"
echo updating version info
cmd /C %create_GUI_ressource_object_file_bat_path% %setup_h_path% C:\Libraries\wxWidgets-2.8.10\include