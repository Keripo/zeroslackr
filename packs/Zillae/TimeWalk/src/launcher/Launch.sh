#!/bin/sh
#set -vx
exec >> "/opt/Zillae/TimeWalk/Misc/$1.log" 2>&1

# Format: $folder $binary
cd "/opt/Zillae/TimeWalk/$1"
exec "/opt/Zillae/TimeWalk/$1/$2"
