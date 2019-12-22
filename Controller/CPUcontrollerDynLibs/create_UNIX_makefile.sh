
#change if necessary
MINIMAL_NUM_ARGS_NEEDED=2
NUM_ARGS_NEEDED=3

#IDEA: pass as args: <NAME=VALUE>, e.g. COMMON_SOURCECODE_ROOT_PATH=../common_sourcecode
if [ $# -ge $MINIMAL_NUM_ARGS_NEEDED ]; then
  DYN_LIB_MAIN_SOURCE_FILE=$1
  DYN_LIB_FILE_NAME=$2
  echo DYN_LIB_MAIN_SOURCE_FILE: $DYN_LIB_MAIN_SOURCE_FILE
  echo DYN_LIB_FILE_NAME: $DYN_LIB_FILE_NAME

if [ $# -ge $NUM_ARGS_NEEDED ]; then
  echo COMMON_SOURCECODE_ROOT_PATH: $COMMON_SOURCECODE_ROOT_PATH
  COMMON_SOURCECODE_ROOT_PATH=$3
fi

if [ $# -ge 4 ]; then
  C_COMPILER=$4
  CPP_COMPILER=$5
else
#Edit the following variables if needed!
#If using a non-absolute path when setting the C Compiler causes this:
# http://public.kitware.com/Bug/view.php?id=13756
#So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294 proposes
C_COMPILER=/usr/bin/gcc
CPP_COMPILER=/usr/bin/g++
fi

echo C compiler: $C_COMPILER
echo C++ compiler: $CPP_COMPILER

#change if necessary
#COMMON_SOURCECODE_ROOT_PATH=../../../common_sourcecode

cmake -G "Unix Makefiles" \
-DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_C_COMPILER=${C_COMPILER} \
-DCMAKE_CXX_COMPILER=${CPP_COMPILER} \
-DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" \
-DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2" \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON . \
-DDYN_LIB_MAIN_SOURCE_FILE:STRING=$DYN_LIB_MAIN_SOURCE_FILE \
-DDYN_LIB_FILE_NAME:STRING=$DYN_LIB_FILE_NAME \
-DCOMMON_SOURCECODE_ROOT_PATH:STRING=$COMMON_SOURCECODE_ROOT_PATH
else
  echo "ERROR:at least" $MINIMAL_NUM_ARGS_NEEDED " args needed:"
  echo "specifiy options for this script:<dynamic library source files> <dynanmic library file name> <COMMON_SOURCECODE_ROOT_PATH>"
  echo "e.g.:$0 HaswellControllerDynLib/dllmain.cpp IntelHaswell /home/sg/SourceCodeManagement/common_sourcecode"
fi
