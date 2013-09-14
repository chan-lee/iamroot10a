#!/bin/bash

case $OSTYPE in
	linux*)
		export CROSS_COMPILE=arm-linux-gnueabi-
		#export HOST_EXTRACFLAGS="-v -save-temps"
		;;
	darwin*)
		export CROSS_COMPILE=~/kernel/android/toolchain/arm-linux-androideabi-4.7/bin/arm-linux-androideabi-
		export HOST_EXTRACFLAGS="-I ~/kernel/linux_host_include"
		#export HOST_EXTRACFLAGS="-I ~/kernel/linux_host_include -v --save-temps"
		;;
esac

export ARCH=arm
export SUBARCH=arm

export TARGET=$*

if [ ! -f .config ]; then
	make exynos_defconfig
fi

make $TARGET 2>&1 | tee make.log

#vim "+/\(Kernel: .* is ready\|error:.*\|.*syntax error\|.*오류 [1-9]\)" make.log
