#!/bin/bash

./build_exynos.sh tags
./build_exynos.sh cscope
./build_exynos.sh gtags
./build_exynos.sh -p V=1

if [ -f make.log ]; then
	mv make.log make.db
fi
