sh ./configure CXX=g++-3 CXXFLAGS=-mno-cygwin LDFLAGS=-no-undefined --enable-transcoder-windows --disable-network --enable-msgloader-inmemory --host=mingw32

! add archive libcrtdll.a as library because it contains errno, assert etc. that are needed