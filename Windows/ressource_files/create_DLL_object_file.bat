
REM from http://blogs.msdn.com/b/oldnewthing/archive/2005/01/28/362565.aspx
set originalDir=%CD%
REM set PATH = %1
echo 1st arg: %1
set out_file_name_prefix=%1
set file_description=\"%~2\"

@REM from http://stackoverflow.com/questions/8145744/how-to-invoke-a-parameter-in-a-batch-file-with-comma-delimiter:
@REM "In Batch files the parameter delimiters are, besides spaces, comma, semicolon and equal sign, so there is no direct way to do that. The only option is to enclose the parameter between quotes: c:\text.bat "this,is,sample" and take the parameter with %~1 to eliminate the quotes: set test=%~1."
echo 2nd arg:%~2
echo file_description:%file_description%
echo 3rd arg:%~3

@REM changing the directory to the one where this files should be created is 
@REM important if this batch file is being executed from another 
@REM (application's current working) directory
cd %X86IANDC_ROOT_SOURCE_PATH%/Windows/ressource_files
set drive=%X86IANDC_ROOT_SOURCE_PATH:~0,2%
@echo drive: %drive%
@REM Finally move to to drive where %X86IANDC_ROOT_SOURCE_PATH% resides.
%drive%

@REM from http://de.wikibooks.org/wiki/Batch-Programmierung:_Programmierungshilfen#.C3.84ndern_des_Editors_zum_Bearbeiten_von_Batchdateien
set current_date=%date:~0%

@REM from http://de.wikibooks.org/wiki/Batch-Programmierung:_Programmierungshilfen#Datum_und_Uhrzeit_anzeigen
@REM %date:~0% %time:~0,8%
@REM "-4" = "last 4 digits" = "the year"
set current_year=%current_date:~-4%
set current_month=%current_date:~3,2%
set current_day=%current_date:~0,2%
@echo %current_date%  
@echo %current_year%
@echo %current_day%

REM from http://stackoverflow.com/questions/2323292/windows-batch-assign-output-of-a-program-to-a-variable:
for /f %%i in ('g++ -dumpversion') do set gpp_version=%%i
echo %gpp_version% > gpp_version.txt

@REM replace   with high comma
@REM set gpp_version="built with g++ %gpp_version%"
set built_with_string=\"built_with_g++_version_%gpp_version%\"
@REM set built_with_string="builtwithgppversion"

set file_version_numeric=%current_year%,%current_month%,%current_day%,0
@REM file version date/ field needs .... format ?!
set file_version_string=\"%current_year%.%current_month%.%current_day%.0\"
echo %file_version_numeric% > current_date.txt

@REM cp version_info_GUI_original.rc version_info_GUI_.rc

@REM read from file version_info_GUI_.rc -> replace strings -> finally write to file version_info_GUI_.rc
@REM sed 's/BUILD_WITH_STRING/%gpp_version%/g' version_info_GUI_.rc | sed 's/VER_FILEVERSION_STRING/%file_version_string%/g' | sed 's/VER_FILEVERSION_NUMBER/%file_version_numeric%/g' > version_info_GUI.rc

set copyright_string=\"Copyright(C)2010-%current_year%\"

@REM see http://msdn.microsoft.com/en-us/library/windows/desktop/aa381058%28v=vs.85%29.aspx:
@REM  a "\0" is appended:  "#define VER_FILEVERSION_STR         "3.10.349.0\0" "
@REM sed 's/VER_FILEVERSION_STRING/"%file_version_string%\0"/g' version_info_GUI_.rc > version_info_GUI.rc

REM windres.exe -D VER_FILEVERSION=%current_year%,%current_month%,%current_day%,0 -DBUILD_WITH=%gpp_version% all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc 
REM windres.exe -D VER_FILEVERSION=%file_version_numeric% --define BUILD_WITH=%gpp_version% all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc 

REM windres.exe -I C:\Libraries\wxWidgets-2.8.10\include\wx\msw all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc 

set define_file_version_numeric=-D FILE_VERSION_NUMBER=%file_version_numeric%
set define_file_version_string=-D FILE_VERSION_STRING=%file_version_string%
set define_comments=-D BUILD_WITH_STRING=%built_with_string%
set define_copyright_string=-D COPYRIGHT_STRING=%copyright_string%

@REM include "%WX2.8.10_MINGW_GCC3.4.5_UNICODE_RELEASE_STATIC_SETUP_H_PATH%" : for "wx/msw/rc"
@REM WXWIDGETS_2.8.10%\include : for "wx/msw/hand.cur" 
set include_wx_setup_h_dir=-I %WX2.8.10_MINGW_GCC3.4.5_UNICODE_RELEASE_STATIC_SETUP_H_PATH%
set include_wx_include_dir=-I %WXWIDGETS_2.8.10%\include

@REM windres prints something like "Usage: windres [option(s)] [input-file] [output-file] ..." when it gets too much command line args?
REM windres %define_file_version_numeric% %define_file_version_string% %define_comments% --use-temp-file --define __WXMSW__ --define _UNICODE %include_wx_setup_h_dir% %include_wx_include_dir% --define NOPCH all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc

echo %define_file_version_numeric% > file_version_numeric.txt
echo %define_file_version_string% > file_version_string.txt

@REM windres gets problems with spaces in "define" declarations.
REM windres %define_file_version_numeric% %define_file_version_string% %define_comments% %define_copyright_string% --use-temp-file --define __WXMSW__ --define _UNICODE %include_wx_setup_h_dir% %include_wx_include_dir% --define NOPCH all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc

@REM windres %define_file_version_numeric% %define_comments% --use-temp-file --define __WXMSW__ %include_wx_setup_h_dir% %include_wx_include_dir% all_ressources_do_not_requ_admin_GUI_preprocessed.rc all_ressources_do_not_requ_admin_GUI.opc

@REM better than replacing via "windres" ("gcc -E" is called by "windres")?!
gcc -E -xc -DRC_INVOKED -DCOPYRIGHT_STRING=%copyright_string% -DBUILD_WITH_STRING=%built_with_string% -DFILE_VERSION_NUMBER=%file_version_numeric% -DFILE_VERSION_STRING=%file_version_string% -DFILE_DESCRIPTION=%file_description% x86IandC_version_info.rc -o %out_file_name_prefix%_preprocessed.rc

windres --use-temp-file %out_file_name_prefix%_preprocessed.rc %out_file_name_prefix%.opc

REM windres --use-temp-file -D FILE_VERSION_NUMBER=%file_version_numeric% --define __WXMSW__ --define _UNICODE %include_wx_setup_h_dir% %include_wx_include_dir% --define NOPCH all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc

cd %originalDir%
set drive=%originalDir:~0,2%
@echo drive: %drive%
@REM Finally move to to drive where %X86IANDC_ROOT_SOURCE_PATH% resides.
%drive%

@REM call bla.bat

pause