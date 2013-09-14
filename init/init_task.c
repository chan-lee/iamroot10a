#include <linux/init_task.h>
#include <linux/export.h>
#include <linux/mqueue.h>
#include <linux/sched.h>
#include <linux/sched/sysctl.h>
#include <linux/sched/rt.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>

#include <asm/pgtable.h>
#include <asm/uaccess.h>

static struct signal_struct init_signals = INIT_SIGNALS(init_signals);
static struct sighand_struct init_sighand = INIT_SIGHAND(init_sighand);

/* Initial task structure */
struct task_struct init_task = INIT_TASK(init_task);
EXPORT_SYMBOL(init_task);

// EXPORT_SYMBOL(sym)
/* extern typeof(sym) sym;						 */
/* extern void *__crc_##sym __attribute__((weak)); */
/* static const unsigned long __kcrctab_##sym __attribute__((__used__)) __attribute__((section("___kcrctab"   "" "+" #sym), unused))	= (unsigned long) &__crc_##sym;					 */
/* static const char __kstrtab_##sym[] __attribute__((section("__ksymtab_strings"), aligned(1))) = #sym;	 */
/* static const struct kernel_symbol __ksymtab_##sym __attribute__((__used__)) __attribute__((section("___ksymtab"  "" "+" #sym), unused))		= { (unsigned long)&sym, __kstrtab_##sym }	 */
// EXPORT_SYMBOL(sym)

/*
 * Initial thread structure. Alignment of this is handled by a special
 * linker map entry.
 */
union thread_union init_thread_union __init_task_data =
	{ INIT_THREAD_INFO(init_task) };
