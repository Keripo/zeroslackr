Last updated: Aug 6, 2008
~Keripo

Build individual libraries running
"./src/$lib.sh" while in "libs" or
run "./build.sh" to build everything
(or "./clean.sh" to remove all built
libraries)

bzip2 source downloaded from
http://www.bzip.org/downloads.html

freetype source download from
http://download.savannah.gnu.org/releases/freetype/

libjpeg source download from
ftp://ftp.uu.net/graphics/jpeg/

libmad source download from
http://sourceforge.net/projects/mad/

libpng source download from
http://sourceforge.net/projects/libpng/

libtiff source download from
ftp://ftp.remotesensing.org/pub/libtiff/

ncurses source download from
http://ftp.gnu.org/pub/gnu/ncurses/

SDL source download from
http://www.libsdl.org/download-1.2.php

SDL_image source download from
http://www.libsdl.org/projects/SDL_image/

SDL_mixer source download from
http://www.libsdl.org/projects/SDL_mixer/

SDL_ttf source download from
http://www.libsdl.org/projects/SDL_ttf/

tar source downloaded from
ftp://download.gnu.org.ua/pub/alpha/tar
(version 1.16.2-20070123 had to be used
since version 1.18.90 or higher don't
seem to compile nicely)

zlib source downloaded from
http://www.zlib.net/zlib-1.2.3.tar.gz

hotdog and ttk source downloaded from
the official SVN. The Noblesse artwork
was drawn by calico_pirate (original:
http://i49.photobucket.com/albums/
f258/calico_pirate/keripoani.gif). I
do not remember where the Ren picture
is from but its probably from an old
screenshot I took of a doujinshi.

The "pz0" libraries should not be touched
since they are precompiled (I've long
forgotten how to compile them). The
itunesdb and mikmod libraries are
depreciated and do not work but they're
only used by PZ0 and for historical
purposes.

The "launch" library is just shared files
(consisting of a stripped-down PZ2 makefile
and the "pz.h" file) used by all packs for
compiling the pack's launch module outside
of PZ2. All packs symlink the "launch" and
"ttk" libraries for this.

