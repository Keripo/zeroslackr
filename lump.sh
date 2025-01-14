#!/bin/sh
#
# Auto-Lumping Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Oct 31, 2008
#
echo ""
echo "==========================================="
echo "==========================================="
echo ""
echo "      Full ZeroSlackr Lumping Script"
echo "         [ because I'm lazy ; ) ]"
echo ""
echo "    This script will compile everything"
echo "    and stick it in a convenient '_lump'"
echo "    folder that can be copied directly"
echo "    to your iPod."
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	CYGWIN=1
fi
if [ $CYGWIN ]; then
	echo ""
	echo "    Building is recommended to be done on"
	echo "    Linux. If you are using Cygwin and"
	echo "    also have TortoiseSVN installed, SVN"
	echo "    downloading may sometimes fail. This"
	echo "    is a known TortoiseSVN issue. You will"
	echo "    have to either try recompiling the"
	echo "    packs disabling TortoiseSVN."
fi
if [ $SANSA ]; then
	echo ""
	echo "    Building all for SansaLinux target."
	echo "    Keep in mind that not all Slackr"
	echo "    packs have been ported and thus some"
	echo "    will be skipped."
fi
echo ""
echo "==========================================="
echo "==========================================="
echo ""
# permissions
chmod ugo+rwx ./chmod-all.sh
./chmod-all.sh
# lump
SVNROOT=$(pwd)
LUMP=$SVNROOT"/_lump"
echo "> Creating "$LUMP" folder..."
if [ -d $LUMP ]; then
	rm -rf $LUMP
fi
mkdir $LUMP
# libs
echo "> Building libs..."
cd $SVNROOT
cd libs
./build.sh
# base
echo ""
echo "> Building base..."
cd $SVNROOT
cd base
for folder in *
do
	cd $folder
	if [ -e SKIP.txt ]; then
		echo ""
		echo "  - Skipping building of "$folder"..."
		if [ $SANSA ] && [ -d build-sansa ]; then
			cp -rf build-sansa/release/* $LUMP/
		else
			cp -rf build/release/* $LUMP/
		fi
	else
		./build.sh
		if [ -d build/release ]; then
			cp -rf build/release/* $LUMP/
		fi
		rm -rf build
	fi
	cd ..
done
# packs
echo ""
echo "> Building packs..."
cd $SVNROOT
cd packs
for folder in *
do
	echo ""
	echo "  - Building packs in "$folder"..."
	cd $folder
	for pack in *
	do
		cd $pack
		if [ -e SKIP.txt ]; then
			echo ""
			echo "  - Skipping building of "$folder"/"$pack"..."
			if [ $SANSA ] && [ -d build-sansa ]; then
				cp -rf build-sansa/release/* $LUMP/
			else
				cp -rf build/release/* $LUMP/
			fi
		else
			./build.sh
			if [ -d build/release ]; then
				cp -rf build/release/* $LUMP/
			fi
			rm -rf build
		fi
		cd ..
	done
	cd ..
done
# libs cleanup
echo ""
echo "> Cleaning up lib..."
cd $SVNROOT
cd libs
./clean.sh
# .svn cleanup
echo "> Cleaning up .svn files..."
cd $LUMP
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# permissions
echo "> Setting all permissions..."
cd $LUMP
sh -c "find . -type f -exec chmod +x {} \;" >> /dev/null 2>&1
# done
cd $SVNROOT
echo ""
echo ""
echo "==========================================="
echo "==========================================="
echo ""
echo "                   Fin!"
echo ""
echo "    Now just copy the entire content of"
if [ $SANSA ]; then
	echo "    the '_lump' folder to your Sansa, run"
else
	echo "    the '_lump' folder to your iPod, run"
fi
echo "    the 'patch.bat' or 'patch.sh' file,"
echo "    and everything will be 'installed' ; )"
if [ $CYGWIN ]; then
	echo ""
	echo "    If you plan on copying over the files"
	echo "    through Windows, please make sure to"
	echo "    uncheck the \"Read-only\" attribute in"
	echo "    the file/folder Properties window."
fi
echo ""
echo "       Auto-Lumping Script by Keripo"
echo ""
echo "==========================================="
echo "==========================================="
echo ""