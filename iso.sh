#!/bin/sh
set -e
. ./build.sh
 
mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
 
cp sysroot/boot/MichaelOS.kernel isodir/boot/MichaelOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "MichaelOS" {
    multiboot /boot/MichaelOS.kernel
}
EOF
grub-mkrescue -o MichaelOS.iso isodir
