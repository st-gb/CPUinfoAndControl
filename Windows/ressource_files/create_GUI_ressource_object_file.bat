REM echo "" >> current_date_and_time.txt

@REM changing the directory to the one where this files should be created is 
@REM important if this batch file is being executed from another 
@REM (application's current working) directory
cd %X86IANDC_ROOT_SOURCE_PATH%/Windows/ressource_files
@REM rm current_date_and_time.txt

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
@REM output current date and time
REM date /T | tr -d \r\n >> current_date_and_time.txt 

@REM from http://de.wikibooks.org/wiki/Batch-Programmierung:_Programmierungshilfen#.C3.84ndern_des_Editors_zum_Bearbeiten_von_Batchdateien
set current_date=%date:~0%

@REM from http://de.wikibooks.org/wiki/Batch-Programmierung:_Programmierungshilfen#Datum_und_Uhrzeit_anzeigen
@REM %date:~0% %time:~0,8%
@REM "-4" = "last 4 digits" = "the year"
set current_year=%current_date:~-4%
set current_month=%current_date:~3,2%
set current_day=%current_date:~0,2%
REM set yearr=%time:~0,8%
@echo %current_date%  
@echo %current_year%
@echo %current_day%
@REM set year = current_date
@REM set month = 

@REM echo \b current_date_and_time.txt
@REM echo \b current_date_and_time.txt

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
REM time /T | tr -d \r\n >> current_date_and_time.txt

REM rm bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
REM echo -D VER_FILEVERSION_STR= | tr -d \r\n >> bla.bat
@REM less current_date_and_time.txt >>> bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
REM date /T | tr -d \r\n >> bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
@REM time /T | tr -d \r\n >> bla.bat

REM from http://stackoverflow.com/questions/2323292/windows-batch-assign-output-of-a-program-to-a-variable:
for /f %%i in ('g++ -dumpversion') do set gpp_version=%%i
REM replace   with high comma
set gpp_version="built with g++ %gpp_version%"
set fileversion=%current_year%,%current_month%,%current_day%,0
echo %gpp_version% > gpp_version.txt
echo %fileversion% > current_date.txt


cp version_info_GUI_original.rc version_info_GUI_.rc

REM BatchSubstitute.bat BUILD_WITH %gpp_version% version_info_GUI.rc
REM sed -c s/BUILD_WITH/%gpp_version%/g version_info_GUI.rc
sed 's/BUILD_WITH_STRING/%gpp_version%/g' version_info_GUI_.rc | sed 's/VER_FILEVERSION_STRING/"%fileversion%"/g' > version_info_GUI.rc
REM sed version_info_GUI.rc, BUILD_WITH, %gpp_version%
REM sed version_info_GUI.rc, VER_FILEVERSION, %fileversion%

@REM echo "\"all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc" bla.bat
REM windres.exe -D VER_FILEVERSION=%current_year%,%current_month%,%current_day%,0 -DBUILD_WITH=%gpp_version% all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc 
REM windres.exe -D VER_FILEVERSION=%fileversion% --define BUILD_WITH=%gpp_version% all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc 

REM windres.exe -I C:\Libraries\wxWidgets-2.8.10\include\wx\msw all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc 

REM windres.exe -D COMPILE_WITHOUT_CURSOR -I C:/Libraries/wxWidgets-2.8.10/include/wx/msw -I C:/Libraries/wxWidgets-2.8.10/include -I C:\Libraries\wxWidgets-2.8.10\lib\MinGW_GCC3.4.5-unicode-release-static\mswu -I C:\MinGW_gcc.3.4.5\include\c++\3.4.5 -I C:\MinGW_gcc.3.4.5\include\c++\3.4.5\mingw32 all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI_without_cursor.opc

windres --use-temp-file --define __WXMSW__ --define _UNICODE -I C:\Libraries\wxWidgets-2.8.10\lib\MinGW_GCC3.4.5-unicode-release-static\mswu -I C:/Libraries/wxWidgets-2.8.10/include --define NOPCH all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc

@REM call bla.bat

pause