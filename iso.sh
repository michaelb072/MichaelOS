#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/michaelos.kernel isodir/boot/michaelos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "michaelos" {
	multiboot /boot/michaelos.kernel
}
EOF
grub-mkrescue -o michaelos.iso isodir
