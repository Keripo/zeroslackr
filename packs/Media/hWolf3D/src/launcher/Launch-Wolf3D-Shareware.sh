#!/bin/sh
#set -vx
#exec >> /opt/Media/hWolf3D/Misc/Launch.log 2>&1

killall -15 ZeroLauncher >> /dev/null 2>&1
cpu_speed 78
backlight brightness 12
cd /opt/Media/hWolf3D/Wolf3D-Shareware
exec /opt/Media/hWolf3D/Wolf3D-Shareware/Wolf3D-Shareware