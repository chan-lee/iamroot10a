/*
 * Many of the syscalls used in this file expect some of the arguments
 * to be __user pointers not __kernel pointers.  To limit the sparse
 * noise, turn off sparse checking for this file.
 */
#ifdef __CHECKER__
#undef __CHECKER__
#warning "Sparse checking disabled for this file"
#endif

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/minix_fs.h>
#include <linux/romfs_fs.h>
#include <linux/initrd.h>
#include <linux/sched.h>
#include <linux/freezer.h>
#include <linux/kmod.h>

#include "do_mounts.h"

unsigned long initrd_start, initrd_end;
int initrd_below_start_ok;
unsigned int real_root_dev;	/* do_proc_dointvec cannot handle kdev_t */
static int __initdata mount_initrd = 1;

static int __init no_initrd(char *str)
{
	mount_initrd = 0;
	return 1;
}

__setup("noinitrd", no_initrd);

static int init_linuxrc(struct subprocess_info *info, struct cred *new)
{
	sys_unshare(CLONE_FS | CLONE_FILES);
	/* stdin/stdout/stderr for /linuxrc */
	sys_open("/dev/console", O_RDWR, 0);
	sys_dup(0);
	sys_dup(0);
	/* move initrd over / and chdir/chroot in initrd root */
	sys_chdir("/root");
	sys_mount(".", "/", NULL, MS_MOVE, NULL);
	sys_chroot(".");
	sys_setsid();
	return 0;
}

static void __init handle_initrd(void)
{
	struct subprocess_info *info;
	static char *argv[] = { "linuxrc", NULL, };
	extern char *envp_init[];
	int error;

  //@@ ramdisk 안에 /root 용 장치인 /dev/root.old 를
  //@@ 만들어 임시로 /root 와 마운트함.
  //@@ 추후 저장 장치로 부터 읽은 root 로 /root 가 교체될 것으로 예측됨.
  //@@ 추정 : / => /old, /dev/root => /dev/root.old
	real_root_dev = new_encode_dev(ROOT_DEV);
	create_dev("/dev/root.old", Root_RAM0);.
	/* mount initrd on rootfs' /root */
	mount_block_root("/dev/root.old", root_mountflags & ~MS_RDONLY);
	sys_mkdir("/old", 0700);
	sys_chdir("/old");

	/* try loading default modules from initrd */
  //@@ i/o elevator module 이 설정되어 있으면 모듈을 올린다.
	load_default_modules();
  //@@ 2018.05.19 end

	/*
	 * In case that a resume from disk is carried out by linuxrc or one of
	 * its children, we need to tell the freezer not to wait for us.
	 */
	current->flags |= PF_FREEZER_SKIP;

	//@@ subprocess_info 구조체를 할당하고, 내부에 path, argument,
	//@@ 환경변수, init 함수, cleanup 함수 등을 설정하여 리턴
	info = call_usermodehelper_setup("/linuxrc", argv, envp_init,
					 GFP_KERNEL, init_linuxrc, NULL, NULL);
	if (!info)
		return;
	//@@ 해당 user process를 실행하고 종료까지 기다린 후 리턴
	call_usermodehelper_exec(info, UMH_WAIT_PROC);

	current->flags &= ~PF_FREEZER_SKIP;

	/* move initrd to rootfs' /old */
	sys_mount("..", ".", NULL, MS_MOVE, NULL);
	/* switch root and cwd back to / of rootfs */
	sys_chroot("..");

	//@@ new_decode_dev(): 디바이스 파일의 major 및 minor 넘버 디코딩
	//@@ new_decode_dev() 결과가 Root_RAM0와 같을 때.. -> 정확히 어떤 의미???
	if (new_decode_dev(real_root_dev) == Root_RAM0) {
		sys_chdir("/old");
		return;
	}

	sys_chdir("/");
	ROOT_DEV = new_decode_dev(real_root_dev);

	//@@ ROOT_DEV가 NFS 또는 FD 인지 확인 후 마운트
	mount_root();

	printk(KERN_NOTICE "Trying to move old root to /initrd ... ");
	error = sys_mount("/old", "/root/initrd", NULL, MS_MOVE, NULL);
	if (!error)
		printk("okay\n");
	else {
		int fd = sys_open("/dev/root.old", O_RDWR, 0);
		if (error == -ENOENT)
			printk("/initrd does not exist. Ignored.\n");
		else
			printk("failed\n");
		printk(KERN_NOTICE "Unmounting old root\n");
		sys_umount("/old", MNT_DETACH);
		printk(KERN_NOTICE "Trying to free ramdisk memory ... ");
		if (fd < 0) {
			error = fd;
		} else {
			error = sys_ioctl(fd, BLKFLSBUF, 0);
			sys_close(fd);
		}
		printk(!error ? "okay\n" : "failed\n");
	}
}

int __init initrd_load(void)
{
	if (mount_initrd) {
		create_dev("/dev/ram", Root_RAM0);
		//@@[2018.04.28] - end
    //@@[2018.05.19] - start
		/*
		 * Load the initrd data into /dev/ram0. Execute it as initrd
		 * unless /dev/ram0 is supposed to be our actual root device,
		 * in that case the ram disk is just set up here, and gets
		 * mounted in the normal path.
		 */
		if (rd_load_image("/initrd.image") && ROOT_DEV != Root_RAM0) {
			sys_unlink("/initrd.image");
			handle_initrd();
			//@@ [2018.05.26] end
			return 1;
		}
	}
	sys_unlink("/initrd.image");
	return 0;
}
