dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
#echo "directory path of this script file:" ${dirPathOfThisScript}

#https://www.fastwebhost.in/blog/how-to-find-if-linux-is-running-on-32-bit-or-64-bit/
machHWname=`uname -m`
echo machHWname: $machHWname

#wxWidgets debug libraries/(setup.h) header file(s) needs package
# ~"libwxgtk3.0-0v5-dbg"
#To install:
#-Debian-based Linux: "sudo apt-get install libwxgtk3.0-0v5-dbg"

#https://stackoverflow.com/questions/10849297/compare-a-string-using-sh-shell
if [ "$machHWname" = "i686" ]; then #If build for 32 bit: uname -m = "i686").
  machHWname="i386"
fi;
wxSetupHpath=/usr/lib/$machHWname-linux-gnu/wx/include/gtk2-unicode-debug-3.0
echo "path containing wx/setup.h: $wxSetupHpath"

CMakeBuildGenerator=$1

bash $dirPathOfThisScript/create_wxGUI_Linux.sh \
"$CMakeBuildGenerator" \
"-Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug -DdirectSMARTaccess=ON" \
$wxSetupHpath

