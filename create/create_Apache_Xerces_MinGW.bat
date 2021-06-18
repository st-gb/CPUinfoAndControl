REM -DCMAKE_C_COMPILER=E:/compilers/TDM-GCC-64/bin/gcc.exe ^
REM -DCMAKE_CXX_COMPILER=E:/compilers/TDM-GCC-64/bin/g++.exe ^

REM " install TARGETS given no ARCHIVE DESTINATION for static library target"

REM xercesc/util/JanitorExports.cpp:22:16: error: duplicate explicit instantiation o
REM f 'class xercesc_3_2::ArrayJanitor<unsigned char>' [-fpermissive]
REM  template class ArrayJanitor<XMLByte>;
                ^
REM xercesc/util/JanitorExports.cpp:23:16: error: duplicate explicit instantiation o
REM f 'class xercesc_3_2::ArrayJanitor<wchar_t>' [-fpermissive]
REM  template class ArrayJanitor<XMLCh>;
    configure CXXFLAGS=-permissive            ^
				
cmake ^
-G "MinGW Makefiles" ^
-DCMAKE_SH="CMAKE_SH-NOTFOUND" ^
-Wno-dev ^
-DCMAKE_INSTALL_PREFIX=. ^
-Dnetwork:BOOL=OFF ^
-Dtranscoder=windows ^
-Dmessage-loader=inmemory ^
-Dxmlch-type=wchar_t ^
-Dmutex-manager=windows ^
.