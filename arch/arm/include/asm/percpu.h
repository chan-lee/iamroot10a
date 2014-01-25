/*
 * Copyright 2012 Calxeda, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _ASM_ARM_PERCPU_H_
#define _ASM_ARM_PERCPU_H_

/*
 * Same as asm-generic/percpu.h, except that we store the per cpu offset
 * in the TPIDRPRW. TPIDRPRW only exists on V6K and V7
 */
#if defined(CONFIG_SMP) && !defined(CONFIG_CPU_V6)
static inline void set_my_cpu_offset(unsigned long off)
{
	/* Set TPIDRPRW */
	//@@ The TPIDRPRW provides a location where software executing at PL1 or higher can store thread identifying information that is not visible to software executing at PL0, for OS management purposes.
	asm volatile("mcr p15, 0, %0, c13, c0, 4" : : "r" (off) : "memory");
	//@@ 현재 CPU OFFSET(0)을 PL1 Thread ID에 쓴다.
	//@@ TODO 이유는???
	//@@ http://www.iamroot.org/xe/index.php?mid=Kernel_10_ARM&document_srl=184082&sort_index=readed_count&order_type=desc 참고.
}

static inline unsigned long __my_cpu_offset(void)
{
	unsigned long off;
	register unsigned long *sp asm ("sp");

	/*
	 * Read TPIDRPRW.
	 * We want to allow caching the value, so avoid using volatile and
	 * instead use a fake stack read to hazard against barrier().
	 */
	asm("mrc p15, 0, %0, c13, c0, 4" : "=r" (off) : "Q" (*sp));

	return off;
}
#define __my_cpu_offset __my_cpu_offset()
#else
#define set_my_cpu_offset(x)	do {} while(0)

#endif /* CONFIG_SMP */

#include <asm-generic/percpu.h>

#endif /* _ASM_ARM_PERCPU_H_ */
