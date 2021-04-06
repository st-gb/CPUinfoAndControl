#e.g. call this program with 2 arguments: /usr/include/wx-3.0 /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0

#Edit the following variables if needed!
#change if necessary
NUM_ARGS_NEEDED=4
MINIMAL_NUM_ARGS_NEEDED=2
numCmdLineArgs=$#

echo "# command line args:" $numCmdLineArgs

#IDEA: pass as args: <NAME=VALUE>, e.g. COMMON_SOURCECODE_ROOT_PATH=../common_sourcecode
if [ $numCmdLineArgs -ge $MINIMAL_NUM_ARGS_NEEDED ]; then
  WXWIDGETS_INCLUDE_DIR=$1
  WXWIDGETS_SETUP_H_PATH=$2
  echo WXWIDGETS_INCLUDE_DIR: $WXWIDGETS_INCLUDE_DIR
  echo WXWIDGETS_SETUP_H_PATH: $WXWIDGETS_SETUP_H_PATH

if [ $numCmdLineArgs -ge $NUM_ARGS_NEEDED ]; then
  PLATFORM_STL_ROOT_PATH=$3
  echo PLATFORM_STL_ROOT_PATH: $PLATFORM_STL_ROOT_PATH
  COMMON_SOURCECODE_ROOT_PATH=$4
  echo COMMON_SOURCECODE_ROOT_PATH: $COMMON_SOURCECODE_ROOT_PATH
fi

if [ $numCmdLineArgs -ge 5 ]; then
  #Needs to be in brackets because it may consist of > 1 token
  BuildSysGenerator="$5"
else
  BuildSysGenerator="Unix Makefiles"
fi

cmake -G "$BuildSysGenerator" \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DEXE_TYPE:STRING=wxGUI \
-DWXWIDGETS_INCLUDE_DIR:STRING=$WXWIDGETS_INCLUDE_DIR \
-DWXWIDGETS_SETUP_H_PATH:STRING=$WXWIDGETS_SETUP_H_PATH \
-DPLATFORM_STL_ROOT_PATH:STRING=$PLATFORM_STL_ROOT_PATH \
-DCOMMON_SOURCECODE_ROOT_PATH:STRING=$COMMON_SOURCECODE_ROOT_PATH \
.
else
  echo "ERROR:at least" $MINIMAL_NUM_ARGS_NEEDED " args needed:"
  echo "specifiy options for this script:<WXWIDGETS_INCLUDE_DIR> <WXWIDGETS_SETUP_H_PATH> <PLATFORMSTL_ROOT_PATH> <COMMON_SOURCECODE_ROOT_PATH>"
  echo "e.g. for Ubuntu Linux 32 bit:"$0 /usr/include/wx-3.0 /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /usr/include/stlsoft-1.9.124/include ../common_sourcecode 
  echo "e.g. for 64 bit Linux Mint:"/usr/include/wx-3.0 /usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /usr/include/stlsoft-1.9.124/include ../common_sourcecode 
fi

