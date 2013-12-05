echo updating build time string
REM touch -m %COMMON_SOURCECODE_ROOT_PATH%\preprocessor_macros/BuildTimeString.h
@REM force rebuild of the build time string by updating modification time by "touch -m"
touch -m %X86IANDC_ROOT_SOURCE_PATH%\Controller\BuildTimeString.cpp
@REM set wxWidgets_setup_h_path=%WXWIDGETS2.8.10_MINGW_GCC3.4.5_32BIT_UNICODE_RELEASE_STATIC_SETUP_H_PATH%
set wxWidgets_setup_h_path=%1
set wxWidgets_include_path=%2
set create_GUI_ressource_object_file_bat_path="%X86IANDC_ROOT_SOURCE_PATH%\Windows\ressource_files\create_GUI_ressource_object_file.bat"
echo updating version info
cmd /C %create_GUI_ressource_object_file_bat_path% %wxWidgets_setup_h_path% %wxWidgets_include_path%