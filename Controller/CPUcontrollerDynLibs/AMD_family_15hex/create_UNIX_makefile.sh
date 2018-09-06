NUMBER_OF_SHELL_ARGS=$#

echo "start--number of shell script args:" $NUMBER_OF_SHELL_ARGS
echo "pass the FULL path to the C++ compiler as command line argument to this script"

MINIMAL_NUM_ARGS_NEEDED=2
NUM_ARGS_NEEDED=3

#Edit the following variables if needed!
#If using a non-absolute path when setting the C Compiler causes this: http://public.kitware.com/Bug/view.php?id=13756
#So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294 proposes
C_COMPILER=/usr/bin/gcc
CPP_COMPILER=/usr/bin/g++

#change if necessary
COMMON_SOURCECODE_ROOT_PATH=../../../common_sourcecode

if [ $# -ge $MINIMAL_NUM_ARGS_NEEDED ]; then
  CPP_COMPILER=$1
  C_COMPILER=$2
  if [ $# -ge $NUM_ARGS_NEEDED ]; then
    COMMON_SOURCECODE_ROOT_PATH=$3
  fi
else
  echo "You did not specify the path to the C & C++ compiler->using defaults"
fi
echo "Using path to C compiler:" $C_COMPILER
echo "Using path to C++ compiler:" $CPP_COMPILER
echo "Using path to common_sourcecode directory:" $COMMON_SOURCECODE_ROOT_PATH

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug \
-DCOMMON_SOURCECODE_ROOT_PATH=$COMMON_SOURCECODE_ROOT_PATH \
-DCMAKE_C_COMPILER=${C_COMPILER} \
-DCMAKE_CXX_COMPILER=${CPP_COMPILER} \
-DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" \
-DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2" \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON .
