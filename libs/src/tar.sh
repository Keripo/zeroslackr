#!/bin/sh
#
# tar Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 27, 2008
#
echo ""
echo "==========================================="
echo ""
echo "tar Auto-Compiling Script"
echo ""
if [ -d tar ]; then
	echo "> Removing old tar directory..."
	rm -rf tar
fi
# Extract source
echo "> Extracting source..."
tar -xf src/tar/tar-1.16.2-20070123.tar.gz
mv tar-1.16.2-20070123 tar
# Compiling
cd tar
echo "> Compiling..."
if [ -e /bin/cygwin1.dll ]; then
	echo "  Note: tar building on Cygwin is a pain;"
	echo "  expect lots of problems and it to fail."
fi
export PATH=/usr/local/bin:$PATH
./configure CC=arm-elf-gcc LDFLAGS=-elf2flt --host=arm-elf --disable-largefile >> build.log 2>&1
make >> build.log 2>&1
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="