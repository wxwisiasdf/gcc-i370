# Compile gcc

export PATH="$PATH:$HOME/opt/cross/bin"
export PREFIX="$HOME/opt/cross"
export TARGET=i370-mvs-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p build-$TARGET-gcc && cd build-$TARGET-gcc

#../gcc/configure \
#    --build=`config.guess` --target=$TARGET --prefix="$PREFIX" \
#    --disable-nls --enable-languages=c,c++ --disable-bootstrap \
#    --without-headers --enable-sjlj-exceptions \
#    --disable-threads --disable-shared --disable-libmpx --disable-libcc1 \
#    --disable-libmudflap --disable-libssp --disable-libgomp \
#    --disable-decimal-float --disable-libquadmath \
#    --enable-checking=$CHECKING --disable-libatomic --disable-libvtv \
#    --disable-libada

make all-gcc -j$(nproc) || exit
make install-gcc || exit

cd ..
