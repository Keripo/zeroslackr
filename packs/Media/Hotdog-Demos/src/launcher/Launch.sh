#!/bin/sh
#set -vx
#exec >> "/opt/Media/Hotdog-Demos/Misc/$1-$2.log" 2>&1

# Format: $dir/binary
killall -15 ZeroLauncher
cd "/opt/Media/Hotdog-Demos/$1"
exec "/opt/Media/Hotdog-Demos/$1/$2"
