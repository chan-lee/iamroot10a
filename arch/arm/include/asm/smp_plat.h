/*
 * ARM specific SMP header, this contains our implementation
 * details.
 */
#ifndef __ASMARM_SMP_PLAT_H
#define __ASMARM_SMP_PLAT_H

#include <linux/cpumask.h>
#include <linux/err.h>

#include <asm/cputype.h>

/*
 * Return true if we are running on a SMP platform
 */
static inline bool is_smp(void) //bool type은 C99 에서 정의되어 있음.
{
#ifndef CONFIG_SMP
	return false;
#elif defined(CONFIG_SMP_ON_UP) // 우리는 이곳으로 들어간다.
	extern unsigned int smp_on_up;
	return !!smp_on_up; // arch/arm/kernel/head.S  smp_on_up에 .global 로 선언되어 있다.(1로 초기화)
	// 그래서 c 파일에서 extern 변수를 써서 참조한다.
	// !! 을 써서 어떤 수가 들어오든 1을 리턴하기 위함.
#else
	return true;
#endif
}

/* all SMP configurations have the extended CPUID registers */
#ifndef CONFIG_MMU
#define tlb_ops_need_broadcast()	0
#else
static inline int tlb_ops_need_broadcast(void)
{
	if (!is_smp())
		return 0;

	return ((read_cpuid_ext(CPUID_EXT_MMFR3) >> 12) & 0xf) < 2;
}
#endif

#if !defined(CONFIG_SMP) || __LINUX_ARM_ARCH__ >= 7
#define cache_ops_need_broadcast()	0
#else
static inline int cache_ops_need_broadcast(void)
{
	if (!is_smp())
		return 0;

	return ((read_cpuid_ext(CPUID_EXT_MMFR3) >> 12) & 0xf) < 1;
}
#endif

/*
 * Logical CPU mapping.
 */
extern u32 __cpu_logical_map[];
#define cpu_logical_map(cpu)	__cpu_logical_map[cpu]
/*
 * Retrieve logical cpu index corresponding to a given MPIDR[23:0]
 *  - mpidr: MPIDR[23:0] to be used for the look-up
 *
 * Returns the cpu logical index or -EINVAL on look-up error
 */
static inline int get_logical_index(u32 mpidr)
{
	int cpu;
	for (cpu = 0; cpu < nr_cpu_ids; cpu++)
		if (cpu_logical_map(cpu) == mpidr)
			return cpu;
	return -EINVAL;
}

/*
 * NOTE ! Assembly code relies on the following
 * structure memory layout in order to carry out load
 * multiple from its base address. For more
 * information check arch/arm/kernel/sleep.S
 */
struct mpidr_hash {
	u32	mask; /* used by sleep.S */
	u32	shift_aff[3]; /* used by sleep.S */
	u32	bits;
};

extern struct mpidr_hash mpidr_hash;

static inline u32 mpidr_hash_size(void)
{
	return 1 << mpidr_hash.bits;
}

extern int platform_can_cpu_hotplug(void);

#endif
