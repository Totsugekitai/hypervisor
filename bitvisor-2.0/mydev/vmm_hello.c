#include "../core/config.h"
#include "../core/constants.h"
#include "../core/current.h"
#include "../core/debug.h"
#include "../core/initfunc.h"
#include "../core/panic.h"
#include "../core/printf.h"
#include "../core/process.h"
#include "../core/regs.h"
#include "../core/thread.h"
#include "../core/spinlock.h"
#include "../core/vmmcall.h"

static void
hello (void)
{
    ulong rbx;
    ulong ret = 10;

    // 引数の受け取り
    current->vmctl.read_general_reg(GENERAL_REG_RBX, &rbx);

    printf("Hello BitVisor!, arg1 = %ld\n", rbx);

    // 戻り地値の格納
    current->vmctl.write_general_reg(GENERAL_REG_RAX, (ulong)ret);
}

static void
vmmcall_hello_init (void)
{
    vmmcall_register("hello", hello);
}

INITFUNC ("vmmcal0", vmmcall_hello_init);
