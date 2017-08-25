sudo apt-get install acpidump

sudo acpidump > acpidata.dat
acpixtract -sDSDT acpidata.dat
iasl -d dsdt.dat
