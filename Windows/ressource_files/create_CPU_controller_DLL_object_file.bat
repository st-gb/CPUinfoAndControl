@REM changing the directory to the one where this files should be created is 
@REM important if this batch file is being executed from another 
@REM (application's current working) directory
@REM cd %X86IANDC_ROOT_SOURCE_PATH%/Windows/ressource_files
@REM set drive=%X86IANDC_ROOT_SOURCE_PATH:~0,2%
@REM @echo drive: %drive%
@REM Finally move to to drive where %X86IANDC_ROOT_SOURCE_PATH% resides.
@REM %drive%

%X86IANDC_ROOT_SOURCE_PATH%/Windows/ressource_files/create_DLL_object_file.bat %1 %2