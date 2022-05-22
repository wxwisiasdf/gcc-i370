#!/bin/sh

# Run this file script using the following directory hierachy:
# /
#   build-modern-gccmvs.sh
#   gcc/
#       gcc/
#       ...
#   ...
#
# Make sure your home directory has an ~/opt/cross folder!
# And remember that the compiler is pretty sussy so expect some crashes :^)

# Compile gcc

export PATH="$PATH:$HOME/opt/cross/bin"
export PREFIX="$HOME/opt/cross"
export TARGET=i370-mvs-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p build-$TARGET-gcc && cd build-$TARGET-gcc

# C++ support, MVS++ confirmed? 100% real no fake? :D
../gcc/configure \
    --build=`config.guess` --target=$TARGET --prefix="$PREFIX" \
    --disable-nls --enable-languages=c,c++ --disable-bootstrap \
    --without-headers --enable-sjlj-exceptions \
    --disable-threads --disable-shared --disable-libmpx --disable-libcc1 \
    --disable-libmudflap --disable-libssp --disable-libgomp \
    --disable-decimal-float --disable-libquadmath \
    --enable-checking=$CHECKING --disable-libatomic --disable-libvtv \
    --disable-libada

make all-gcc -j$(nproc) || exit
make install-gcc || exit

cd ..
