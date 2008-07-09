#!/bin/sh
#
# SDL_image Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 8, 2008
#
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo ""
	echo "==========================================="
	echo ""
	echo "SDL_image Auto-Compiling Script"
	echo ""
	echo "[SDL_image doesn't seem to compile"
	echo " nicely on Cygwin - skipping]"
	echo ""
	echo "==========================================="
	echo ""
	exit
fi
# Requires zlib, libjpeg, libpng, libtiff and SDL
LIBS="zlib libjpeg libpng libtiff SDL"
for lib in $LIBS; do
	if [ ! -d $lib ]; then
		echo ""
		echo "[SDL_image requires $lib]"
		./src/$lib.sh
		echo ""
	fi
done
echo ""
echo "==========================================="
echo ""
echo "SDL_image Auto-Compiling Script"
echo ""
# Cleanup
if [ -d SDL_image ]; then
	echo "> Removing old SDL_image directory..."
	rm -rf SDL_image
fi
# Extract source
echo "> Extracting source..."
tar -xf src/SDL_image/SDL_image-1.2.6.tar.gz
mv SDL_image-1.2.6 SDL_image
# Compiling
cd SDL_image
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure CC=arm-uclinux-elf-gcc --host=arm-uclinux-elf --with-sdl-prefix=$(pwd)/../SDL --prefix=$(pwd) >> build.log 2>&1
patch -p0 -t -i ../src/SDL_image/Makefile.patch >> build.log
make install >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="