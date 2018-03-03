/*
 * Copyright (c) 2002-3 Patrick Mochel
 * Copyright (c) 2002-3 Open Source Development Labs
 *
 * This file is released under the GPLv2
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/memory.h>

#include "base.h"

/**
 * driver_init - initialize driver model.
 *
 * Call the driver model init functions to initialize their
 * subsystems. Called early from init/main.c.
 */
void __init driver_init(void)
{
	//@@ [2017.03.03] Start
	/* These are the core pieces */
	//@@ devtmpfs 을 등록하고 devtmpfsd daemon을 띄운다.
	//@@ devtmpfsd 는 디바이스 add를 처리하는 deamon 이다.
	devtmpfs_init();
	//@@ devices와 dev/block dev/char kobject를 생성하고 구성한다.
	//@@ 나중에 /sys 에 kobject구조는 반영될 것으로 예상한다.
	devices_init();
	//@@ bus와 devices/system 을 생성한다.
	buses_init();
	//@@ class kset를 생성한다.
	classes_init();
	//@@ firmware kobject를 생성한다.
	firmware_init();
	//@@ hypervisor kobject를 생성한다.
	hypervisor_init();

	/* These are also core pieces, but must come after the
	 * core core pieces.
	 */
	//@@ fix된 platform 디바이스 드리이버를 구성하기 위해 bus/platform 드라이버를 등록하고 bus/platform/devices 와 bus/platform/drivers 를 등록한다.
	platform_bus_init();
	//@@ cpu device 를 devices/system/cpu에 등록한다.
	cpu_dev_init();
	//@@ memory를 sector별(???)로 나누어 devices/system/memory 에 등록한다.
	memory_dev_init();
	//@@ [2017.03.03] End
}
