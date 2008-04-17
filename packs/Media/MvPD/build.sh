#!/bin/sh
#
# MvPD Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "MvPD Auto-Building Script"
echo ""
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
fi
# Make new compiling directory
echo "> Setting up build directory..."
mkdir build
cd build
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="ttk launch"
for lib in $LIBS
do
	if [ ! -d $LIBSDIR/$lib ]; then
		cd $LIBSDIR
		echo "  - Building "$lib"..."
		./src/$lib.sh
		cd $DIR
	fi
	ln -s $LIBSDIR/$lib ./
done
# Launch module
echo "> Building ZeroLauncher launch module..."
cp -rf ../src/launcher ./
cd launcher
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make -f ../launch/launch.mk >> ../build.log
cd ..
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
mkdir schemes
cd schemes
for scheme in ../../src/mod/schemes/*
do
	tar -xf $scheme
done
cd ..
mkdir videos
cd videos
for video in ../../src/mod/videos/*
do
	tar -xf $video
done
cd ..
cd release
# Files
PACK=ZeroSlackr/opt/MvPD
cp -rf ../../src/hex/* $PACK/
cp -rf ../schemes/* $PACK/Schemes/
cp -rf ../videos/* $PACK/Videos/Clips/
mkdir $PACK/Launch
cp -rf ../launcher/* $PACK/Launch/
# Documents
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="