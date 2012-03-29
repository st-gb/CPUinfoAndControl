
@REM changing the directory to the one where this files should be created is 
@REM important if this batch file is being executed from another 
@REM (application's current working) directory
cd %X86IANDC_ROOT_SOURCE_PATH%/Windows/ressource_files

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

set file_version_numeric=%current_year%,%current_month%,%current_day%,0
set file_version_string="%current_year%.%current_month%.%current_day%.0"

set copyright_string="Copyright (C) 2010-%current_year%"

windres.exe -D FILE_VERSION_NUMBER=%file_version_numeric% -D FILE_VERSION_STRING=%file_version_string%  all_ressources_with_require_admin_manifest_service.rc all_ressources_with_require_admin_manifest_service.o 

pause