#This worked for Ubuntu 10?:
#echo powersave | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor >/dev/null

#https://www.reddit.com/r/linuxmint/comments/9f8585/how_to_disable_ondemand_cpu_scaling_daemon/

#https://wiki.archlinux.org/index.php/CPU_frequency_scaling

#from https://wiki.ubuntuusers.de/Prozessortaktung/ :
#"Die Umleitung über tee ist für das Wechseln des Governors notwendig, da ein
# direktes echo conservative > /sys/... nicht erlaubt ist und mit einer 
# Fehlermeldung beendet wird."
echo powersave | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
