#include "../core/mm.h"
#include "../include/share/vmm_types.h"
#include "../core/spinlock.h"
#include "../core/mmio.h"
#include "../core/printf.h"
#include "../core/current.h"
#include "../core/gmm_access.h"
#include "../core/initfunc.h"

typedef union ioapic_select_reg {
    struct {
        u8 reg_addr;
        u32 rsv : 24;
    } s;
    u32 v;
} ioapic_select_reg;

typedef union ioapic_ver_reg {
    struct {
        u8 apic_version;
        u8 rsv0;
        u8 max_redirection_entry;
        u8 rsv1;
    } s;
    u32 v;
} ioapic_ver_reg;

typedef union ioapic_id_reg {
    struct {
        u32 rsv0 : 24;
        u32 id : 4;
        u32 rsv1 : 4;
    } s;
    u32 v;
} ioapic_id_reg;

static int
ioapic_mm_handler (void *data, phys_t gphys, bool wr, void *buf, uint len, u32 flags)
{
    ioapic_select_reg select;
    u32 data1;
    phys_t hphys;
    ioapic_ver_reg vreg;
    ioapic_id_reg ireg;

    hphys = current->gmm.gp2hp(gphys, NULL);

    if (len == 4) {
        read_hphys_l(hphys, &data1, flags);
    } else {
        return 0;
    }

    if(buf == NULL){
        printf("buf is NULL pointer\n");
        return 0;
    }

    read_hphys_l(hphys - 0x10, &select, flags);

    if (select.s.reg_addr == 0x1 && !wr) {
        printf("===== IOAPIC version read =====\n");
        printf("guest's physical address is 0x%016lx\n", (unsigned long)gphys);
        printf("VMM's physical address is 0x%016lx\n", (unsigned long)hphys);
        printf("I/O type: %s\n", (wr) ? "WRITE" : "READ");
        printf("Length: %d\n", len);
        printf("Flag: 0x%016x\n", flags);

        read_hphys_l(hphys, &data1, flags);
        printf("[by totsugeki] Read data is 0x%08x\n", data1);
        vreg.v = (u32)data1;
        printf("[by totsugeki] IOAPIC version is %d\n", vreg.s.apic_version);
        u8 prev = vreg.s.apic_version;
        vreg.s.apic_version = 0x11; // hack!!!
        printf("[by totsugeki] IOAPIC version hack: %d -> %d\n", prev, vreg.s.apic_version);
        *((unsigned int *)buf) = (unsigned int)vreg.v;
    } else if (select.s.reg_addr == 0x0 && !wr) {
        printf("===== IOAPIC ID read =====\n");
        printf("guest's physical address is 0x%016lx\n", (unsigned long)gphys);
        printf("VMM's physical address is 0x%016lx\n", (unsigned long)hphys);
        printf("I/O type: %s\n", (wr) ? "WRITE" : "READ");
        printf("Length: %d\n", len);
        printf("Flag: 0x%016x\n", flags);

        read_hphys_l(hphys, &data1, flags);
        printf("[by totsugeki] Read data is 0x%08x\n", data1);
        ireg.v = (u32)data1;
        printf("[by totsugeki] IOAPIC ID is %d\n", ireg.s.id);
    } else {
        return 0;
    }

    return 1;
}

void
reg_ioapic_mm_handler(void)
{
    mmio_register(0xFEC00010, 0x4, ioapic_mm_handler, NULL);
}

