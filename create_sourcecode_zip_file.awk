
#
# T:\SourceCodeManagement\IDE_projects\eclipse_workspace\.metadata\.plugins\org.eclipse.core.runtime\.settings\org.eclipse.core.resources.prefs

# pathvariable.x86InfoAndControlRootPath=T\:/SourceCodeManagement/X86Info_and_Control
# pathvariable.common_sourcecode=T\:/SourceCodeManagement/common_sourcecode

# Replace with real pathes from the linked ressources.
# Get all files from project's "linked folders"

# ".cproject" XML file :
# Get "<cconfiguration " with name="wanted_configuration"
# get all entry "<entry excluding="  "

# /cconfiguration/

# put "awk -F 0" at the beginning because:
# http://en.wikipedia.org/wiki/AWK#Self-contained_AWK_scripts :
# "As with many other programming languages, self-contained AWK script can be constructed using the so-called "shebang" syntax."
# "The -f tells awk that the argument that follows is the file to read the awk program from, which is placed there by the shell when running."
awk -F: ' #{ print $0 }' 
     # $0 ~ /.*<cconfiguration.*/  # whole line matches "cconfiguration"
    { # for (i = 1; i<=NF ; i++)
	# { # nicht "print", sondern "printf" verwenden, da "print" immer ein newline ausgibt
	print $0
	# }
}' ../x86IandC.cproject