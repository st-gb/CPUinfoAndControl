@REM changing the directory to the one where this files should be created is 
@REM important if this batch file is being executed from another 
@REM (application's current working) directory
cd %X86IANDC_ROOT_SOURCE_PATH%/Windows/ressource_files
set drive=%X86IANDC_ROOT_SOURCE_PATH:~0,2%
@echo drive: %drive%
@REM Finally move to to drive where %X86IANDC_ROOT_SOURCE_PATH% resides.
%drive%

create_DLL_object_file.bat %1 %2