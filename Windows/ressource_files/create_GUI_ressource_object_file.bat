REM echo "" >> current_date_and_time.txt

@REM changing the directory to the one where this files should be created is 
@REM important if this batch file is being executed from another 
@REM (application's current working) directory
cd %X86IANDC_ROOT_SOURCE_PATH%/Windows/ressource_files
@REM rm current_date_and_time.txt

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
@REM output current date and time
date /T | tr -d \r\n >> current_date_and_time.txt 
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
time /T | tr -d \r\n >> current_date_and_time.txt

@REM windres.exe -D VER_FILEVERSION= -D VER_FILEVERSION_STR= %X86IANDC_ROOT_SOURCE_PATH%"/Windows/ressource_files/all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc


@REM windres.exe -D VER_FILEVERSION='less current_date_and_time.txt' -D VER_FILEVERSION_STR= all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc

rm bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
echo windres.exe | tr -d \r\n >> bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
echo -D VER_FILEVERSION_STR= | tr -d \r\n >> bla.bat
@REM less current_date_and_time.txt >>> bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
date /T | tr -d \r\n >> bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
@REM time /T | tr -d \r\n >> bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
echo all_ressources_do_not_requ_admin_GUI.rc | tr -d \r\n >> bla.bat

@REM from http://www.devdaily.com/unix/edu/un010011/
@REM "tr -d \r\n" : delete newline and carriage return
echo all_ressources_do_not_requ_admin_GUI.opc >> bla.bat

echo pause | tr -d \r\n >> bla.bat

@REM echo "\"all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc" bla.bat
windres.exe -D	VER_FILEVERSION=%current_year%,%current_month%,%current_day%,0 all_ressources_do_not_requ_admin_GUI.rc all_ressources_do_not_requ_admin_GUI.opc 

@REM call bla.bat

pause